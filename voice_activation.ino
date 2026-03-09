
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

// Servo
Servo doorServo;
int servoPin = 16;

// Ultrasonic
int trigPin = 5;
int echoPin = 17;

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing
unsigned long hallOffTime = 0;
bool hallAutoOn = false;
unsigned long doorCloseTime = 0;
bool doorOpenFlag = false;
unsigned long welcomeUntil = 0;

float getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2.0;
}

void openDoor() {
  doorServo.write(90);
  doorOpenFlag = true;
  doorCloseTime = millis() + 5000;
}

void closeDoor() {
  doorServo.write(0);
  doorOpenFlag = false;
}

void showWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME HOME");
  lcd.setCursor(0, 1);
  lcd.print("Door Opened");
  welcomeUntil = millis() + 2000;
}

void updateLCD(float temp, float hum) {
  if (millis() < welcomeUntil) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (isnan(temp)) lcd.print("--");
  else lcd.print(temp, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  if (isnan(hum)) lcd.print("--");
  else lcd.print(hum, 1);
  lcd.print("%");
}

void setup() {
  Serial.begin(115200);

  pinMode(bedroomLED, OUTPUT);
  pinMode(hallLED, OUTPUT);
  pinMode(kitchenLED, OUTPUT);
  pinMode(irSensor, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(bedroomLED, LOW);
  digitalWrite(hallLED, LOW);
  digitalWrite(kitchenLED, LOW);

  doorServo.setPeriodHertz(50);
  doorServo.attach(servoPin, 500, 2400);
  doorServo.write(0);

  dht.begin();

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SMART HOME");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }

  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1500);

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  int motion = digitalRead(irSensor);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  float distance = getDistanceCM();

  // IR automation
  if (motion == HIGH) {
    digitalWrite(hallLED, HIGH);
    hallAutoOn = true;
    hallOffTime = currentMillis + 30000;

    openDoor();
    showWelcome();
  }

  // Ultrasonic optional door open
  if (distance > 0 && distance < 20) {
    openDoor();
  }

  // Auto hall off
  if (hallAutoOn && currentMillis > hallOffTime) {
    digitalWrite(hallLED, LOW);
    hallAutoOn = false;
  }

  // Auto close door
  if (doorOpenFlag && currentMillis > doorCloseTime) {
    closeDoor();
  }

  // LCD refresh
  static unsigned long lastLCD = 0;
  if (currentMillis - lastLCD > 2000) {
    lastLCD = currentMillis;
    updateLCD(temp, hum);
  }

  // Web server
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

    // LED commands
    if (req.indexOf("/BEDON") != -1) digitalWrite(bedroomLED, HIGH);
    if (req.indexOf("/BEDOFF") != -1) digitalWrite(bedroomLED, LOW);

    if (req.indexOf("/HALLON") != -1) digitalWrite(hallLED, HIGH);
    if (req.indexOf("/HALLOFF") != -1) digitalWrite(hallLED, LOW);

    if (req.indexOf("/KITCHENON") != -1) digitalWrite(kitchenLED, HIGH);
    if (req.indexOf("/KITCHENOFF") != -1) digitalWrite(kitchenLED, LOW);

    // Door commands
    if (req.indexOf("/OPEN") != -1) openDoor();
    if (req.indexOf("/CLOSE") != -1) closeDoor();

    // HTML page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<title>Smart Home</title>");
    client.println("<style>");
    client.println("body{font-family:Arial;text-align:center;background:#0f172a;color:white;margin:0;padding:20px;}");
    client.println("h1{color:#38bdf8;}");
    client.println(".card{background:#1e293b;padding:20px;margin:15px auto;border-radius:15px;max-width:360px;box-shadow:0 4px 12px rgba(0,0,0,0.3);}");
    client.println("button{padding:14px 24px;font-size:17px;margin:8px;border:none;border-radius:10px;background:#38bdf8;color:white;cursor:pointer;}");
    client.println(".off{background:#ef4444;}");
    client.println("input{padding:12px;width:80%;font-size:16px;border-radius:10px;border:none;margin:10px;}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");

    client.println("<h1>SMART HOME</h1>");

    client.print("<p>Temperature: ");
    if (isnan(temp)) client.print("--");
    else client.print(temp, 1);
    client.println(" C</p>");

    client.print("<p>Humidity: ");
    if (isnan(hum)) client.print("--");
    else client.print(hum, 1);
    client.println(" %</p>");

    client.print("<p>Distance: ");
    if (distance < 0) client.print("--");
    else client.print(distance, 1);
    client.println(" cm</p>");

    client.println("<div class='card'>");
    client.println("<h2>Bedroom Light</h2>");
    client.println("<a href='/BEDON'><button>ON</button></a>");
    client.println("<a href='/BEDOFF'><button class='off'>OFF</button></a>");
    client.println("</div>");

    client.println("<div class='card'>");
    client.println("<h2>Hall Light</h2>");
    client.println("<a href='/HALLON'><button>ON</button></a>");
    client.println("<a href='/HALLOFF'><button class='off'>OFF</button></a>");
    client.println("</div>");

    client.println("<div class='card'>");
    client.println("<h2>Kitchen Light</h2>");
    client.println("<a href='/KITCHENON'><button>ON</button></a>");
    client.println("<a href='/KITCHENOFF'><button class='off'>OFF</button></a>");
    client.println("</div>");

    client.println("<div class='card'>");
    client.println("<h2>Door Control</h2>");
    client.println("<a href='/OPEN'><button>OPEN</button></a>");
    client.println("<a href='/CLOSE'><button class='off'>CLOSE</button></a>");
    client.println("</div>");

    client.println("<div class='card'>");
    client.println("<h2>Voice Command</h2>");
    client.println("<p>Use phone keyboard mic and say: bedroom on, hall off, open door</p>");
    client.println("<input id='cmd' type='text' placeholder='Speak or type command'>");
    client.println("<br><button onclick='sendCommand()'>Send Command</button>");
    client.println("</div>");

    client.println("<script>");
    client.println("function sendCommand(){");
    client.println("  var command = document.getElementById('cmd').value.toLowerCase();");
    client.println("  if(command.includes('bedroom on') || command.includes('bedroom light on')) window.location='/BEDON';");
    client.println("  else if(command.includes('bedroom off') || command.includes('bedroom light off')) window.location='/BEDOFF';");
    client.println("  else if(command.includes('hall on') || command.includes('hall light on')) window.location='/HALLON';");
    client.println("  else if(command.includes('hall off') || command.includes('hall light off')) window.location='/HALLOFF';");
    client.println("  else if(command.includes('kitchen on') || command.includes('kitchen light on')) window.location='/KITCHENON';");
    client.println("  else if(command.includes('kitchen off') || command.includes('kitchen light off')) window.location='/KITCHENOFF';");
    client.println("  else if(command.includes('open door') || command.includes('door open')) window.location='/OPEN';");
    client.println("  else if(command.includes('close door') || command.includes('door close')) window.location='/CLOSE';");
    client.println("  else alert('Command not recognized');");
    client.println("}");
    client.println("</script>");

    client.println("</body>");
    client.println("</html>");

    delay(1);
    client.stop();
  }
}
