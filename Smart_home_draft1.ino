#include <WiFi.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "realme 9 pro plus";
const char* password = "vedant@1234";

WiFiServer server(80);

// LEDs
int bedroomLED = 23;
int hallLED = 19;
int kitchenLED = 18;

// IR Sensor
int irSensor = 34;

// Servo (door)
Servo doorServo;
int servoPin = 16;

// Ultrasonic
int trigPin = 5;
int echoPin = 17;

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

// LCD I2C
LiquidCrystal_I2C lcd(0x27,16,2);

// Timing
unsigned long hallOffTime = 0;
bool hallAutoOn = false;
unsigned long doorCloseTime = 0;
bool doorOpenFlag = false;

void setup() {

Serial.begin(115200);

pinMode(bedroomLED,OUTPUT);
pinMode(hallLED,OUTPUT);
pinMode(kitchenLED,OUTPUT);
pinMode(irSensor,INPUT);

pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);

doorServo.attach(servoPin);
doorServo.write(0);

lcd.init();
lcd.backlight();

lcd.print("SMART HOME");
lcd.setCursor(0,1);
lcd.print("Starting...");

dht.begin();

WiFi.begin(ssid,password);

while(WiFi.status() != WL_CONNECTED){
delay(500);
Serial.println("Connecting...");
}

Serial.println("WiFi Connected");
Serial.println(WiFi.localIP());

lcd.clear();
lcd.print("WiFi Connected");

server.begin();
}

void loop() {

unsigned long currentMillis = millis();

// ---- IR Motion Automation ----
int motion = digitalRead(irSensor);

if(motion == HIGH){

digitalWrite(hallLED,HIGH);
hallAutoOn = true;
hallOffTime = currentMillis + 30000;

doorServo.write(90);
doorOpenFlag = true;
doorCloseTime = currentMillis + 5000;

lcd.clear();
lcd.print("WELCOME HOME");
}

// Auto OFF Hall
if(hallAutoOn && currentMillis > hallOffTime){
digitalWrite(hallLED,LOW);
hallAutoOn = false;
}

// Auto Close Door
if(doorOpenFlag && currentMillis > doorCloseTime){
doorServo.write(0);
doorOpenFlag = false;
}

// ---- Ultrasonic Distance ----
digitalWrite(trigPin,LOW);
delayMicroseconds(2);

digitalWrite(trigPin,HIGH);
delayMicroseconds(10);

digitalWrite(trigPin,LOW);

long duration = pulseIn(echoPin,HIGH);

float distance = duration * 0.034 / 2;

// ---- Read DHT11 ----
float temp = dht.readTemperature();
float hum = dht.readHumidity();

// ---- LCD update every 2 sec ----
static unsigned long lastLCD = 0;

if(currentMillis - lastLCD > 2000){

lastLCD = currentMillis;

lcd.clear();
lcd.setCursor(0,0);
lcd.print("T:");
lcd.print(temp);
lcd.print("C");

lcd.setCursor(0,1);
lcd.print("H:");
lcd.print(hum);
lcd.print("%");
}

// ---- Web Server ----
WiFiClient client = server.available();

if(client){

String req = client.readStringUntil('\r');
client.flush();

if(req.indexOf("/BEDON")!=-1) digitalWrite(bedroomLED,HIGH);
if(req.indexOf("/BEDOFF")!=-1) digitalWrite(bedroomLED,LOW);

if(req.indexOf("/HALLON")!=-1) digitalWrite(hallLED,HIGH);
if(req.indexOf("/HALLOFF")!=-1) digitalWrite(hallLED,LOW);

if(req.indexOf("/KITCHENON")!=-1) digitalWrite(kitchenLED,HIGH);
if(req.indexOf("/KITCHENOFF")!=-1) digitalWrite(kitchenLED,LOW);

if(req.indexOf("/OPEN")!=-1) doorServo.write(90);
if(req.indexOf("/CLOSE")!=-1) doorServo.write(0);

// ---- Send Webpage ----
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("");

client.println("<!DOCTYPE html><html>");
client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");

client.println("<style>");
client.println("body{font-family:Arial;text-align:center;background:#0f172a;color:white;}");
client.println("button{padding:15px 30px;font-size:18px;margin:10px;border:none;border-radius:8px;background:#38bdf8;color:white;}");
client.println("</style>");

client.println("<h1>SMART HOME</h1>");

client.print("<h3>Temperature: ");
client.print(temp);
client.println("C</h3>");

client.print("<h3>Humidity: ");
client.print(hum);
client.println("%</h3>");

client.println("<h2>Bedroom</h2>");
client.println("<a href='/BEDON'><button>ON</button></a>");
client.println("<a href='/BEDOFF'><button>OFF</button></a>");

client.println("<h2>Hall</h2>");
client.println("<a href='/HALLON'><button>ON</button></a>");
client.println("<a href='/HALLOFF'><button>OFF</button></a>");

client.println("<h2>Kitchen</h2>");
client.println("<a href='/KITCHENON'><button>ON</button></a>");
client.println("<a href='/KITCHENOFF'><button>OFF</button></a>");

client.println("<h2>Door</h2>");
client.println("<a href='/OPEN'><button>OPEN</button></a>");
client.println("<a href='/CLOSE'><button>CLOSE</button></a>");

client.println("</html>");

delay(1);
client.stop();

}

}