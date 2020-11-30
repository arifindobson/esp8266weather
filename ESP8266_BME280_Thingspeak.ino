#include <ESP8266WiFi.h>
#include <math.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

/*Put your SSID & Password*/
const char* ssid = "Nama-Wifi Anda";  // Enter SSID here
const char* password = "password wifi";  //Enter Password here

//Thingspeak and Firebase Authentication
String apiKey = "Write_API";
const char* server = "api.thingspeak.com";           

WiFiClient client;

int delayMinute = 4 * 60000;
int ctt=0;

unsigned long currentMillis;
unsigned long previousMillisA = 0;
unsigned long previousMillisB = 0;

unsigned long intervalCheck   = 4*60000; 


void setup() {
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  ctt++;
  if (ctt>60){
    ESP.restart();
  }
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

}
void loop() {
  unsigned long currentMillis = millis();
  
  if ((currentMillis - previousMillisA) >= intervalCheck) {
  previousMillisA = currentMillis;
    
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  //altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
    Serial.println("Sending Package");  
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temperature);
    postStr +="&field2=";
    postStr += String(humidity);
    postStr +="&field3=";
    postStr += String(pressure);
//    postStr +="&field4=";
//    postStr += String(tpl);
//    postStr +="&field5=";
//    postStr += String(litre);
    postStr += "\r\n\r\n";

   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   Serial.println("Uploading…");
   client.stop();
   Serial.println("Data Sent! Yeay~");
   }
  else {
    Serial.println("Cannot Reach Thingspeak");
    ESP.restart();
   }

  
  }
  
}

