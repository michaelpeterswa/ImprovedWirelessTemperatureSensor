#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <InfluxDb.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define INFLUXDB_URL "**INFLUXDB url**" //Enter IP of device running Influx Database
#define INFLUXDB_NAME "**INFLUXDB database name **"
#define WIFI_SSID "**"              //Enter SSID of your WIFI Access Point
#define WIFI_PASS "**"                    //Enter Password of your WIFI Access Point

Adafruit_BME280 bme; // i2c
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_NAME);
unsigned long delayTime;

void setup() {
  Serial.begin(9600);

  bool status;

  Serial.println("Starting Connection:");
  WiFi.begin(WIFI_SSID, WIFI_PASS);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print("+"); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  sendToInfluxDB();
  
  ESP.deepSleep(3e8);
}


void loop() {}

void sendToInfluxDB() {
  
  Point pointDevice("bme280_1");
  pointDevice.addTag("device", "ESP8266");
  pointDevice.addTag("SSID", WiFi.SSID());
  pointDevice.addField("RSSI", WiFi.RSSI());
  pointDevice.addField("temperature", ((bme.readTemperature() * 1.8) + 32));
  pointDevice.addField("humidity", bme.readHumidity());
  pointDevice.addField("pressure", (bme.readPressure()/ 100.0F));
  Serial.println("Sending to InfluxDB");
  client.writePoint(pointDevice);

}
