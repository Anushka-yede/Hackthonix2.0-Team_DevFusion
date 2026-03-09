#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);
  dht.begin();

}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C   ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(2000);
}
