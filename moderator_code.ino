#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

WebServer server(80);

Servo fanServo;

/* Pins */
int ledPin = 2;
int pirPin = 15;
int buzzerPin = 23;
int servoPin = 18;

bool lightState = false;

/* -------- OLED FUNCTION -------- */

void showMessage(String msg){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println(msg);
  display.display();
}

/* -------- LIGHT CONTROL -------- */

void lightOn(){
  digitalWrite(ledPin, HIGH);
  lightState = true;
  showMessage("LIGHT ON");
  server.send(200, "text/plain", "Light ON");
}

void lightOff(){
  digitalWrite(ledPin, LOW);
  lightState = false;
  showMessage("LIGHT OFF");
  server.send(200, "text/plain", "Light OFF");
}

/* -------- FAN / SERVO -------- */

void fanOn(){
  fanServo.write(180);
  showMessage("FAN ON");
  server.send(200,"text/plain","Fan ON");
}

void fanOff(){
  fanServo.write(0);
  showMessage("FAN OFF");
  server.send(200,"text/plain","Fan OFF");
}

/* -------- BUZZER -------- */

void buzzerOn(){
  digitalWrite(buzzerPin, HIGH);
  showMessage("ALARM");
  server.send(200,"text/plain","Buzzer ON");
}

void buzzerOff(){
  digitalWrite(buzzerPin, LOW);
  showMessage("SAFE");
  server.send(200,"text/plain","Buzzer OFF");
}

/* -------- SETUP -------- */

void setup(){

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  fanServo.attach(servoPin);

  /* OLED INIT */

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("OLED failed");
  }

  showMessage("BOOTING");

  /* WIFI CONNECT */

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  Serial.println(WiFi.localIP());

  showMessage("WELCOME");

  /* API ROUTES */

  server.on("/light/on", lightOn);
  server.on("/light/off", lightOff);

  server.on("/fan/on", fanOn);
  server.on("/fan/off", fanOff);

  server.on("/buzzer/on", buzzerOn);
  server.on("/buzzer/off", buzzerOff);

  server.begin();
}

/* -------- LOOP -------- */

void loop(){

  server.handleClient();

  int motion = digitalRead(pirPin);

  if(motion == HIGH && !lightState){
    digitalWrite(ledPin, HIGH);
    showMessage("MOTION!");
    delay(3000);
  }

}