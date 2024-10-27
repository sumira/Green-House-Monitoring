#define BLYNK_TEMPLATE_ID "***********"
#define BLYNK_TEMPLATE_NAME "Green House"
#define BLYNK_AUTH_TOKEN "****************"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* ssid = "CISLabs";
const char* pass = "f6De1707";

#define DHT11_PIN D5
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

#include <BH1750.h>
#include <Wire.h>
BH1750 lightMeter;


const int sensor_pin = A0;  



void setup() {
  Serial.begin(115200);
  delay(100);
  // Connect to Blynk
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Wire.begin();
  lightMeter.begin();
  pinMode(sensor_pin, INPUT);
  

}

void loop() {

  Blynk.run();


  //DHT 11 segment
  DHT.read(DHT11_PIN);
  

  //Soil moisture segment
  float moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  

  //BH1750 segment
  float lux = lightMeter.readLightLevel();

  if(DHT.temperature > 40){

    Serial.println("High Temperature");
    Blynk.logEvent("temperature_event", String("Green house temperature is greater than 40C!"));
 
  }
   if(DHT.humidity > 70){

    Serial.println("High Humidity");
    Blynk.logEvent("humidity_event", String("Green house humidity is greater than 70%!"));
 
  }

   if(moisture_percentage > 50){

    Serial.println("High temperature");
    Blynk.logEvent("soil_moisture_event", String("Green house soil moisture is greater than 50%!"));
 
  }
    if(lux > 900){

    Serial.println("Extreme Sun light");
    Blynk.logEvent("light_intensity_event", String("Green house light intensity is in a critical level!"));
 
  }

  Serial.print("Green House Temperature is: ");
  Serial.print(DHT.temperature);
  Serial.print(" Humidity is: ");
  Serial.print(DHT.humidity);
  Serial.print(" | Soil Moisture precentage = ");
  Serial.print(moisture_percentage);
  Serial.println("%");
  Serial.print(" | Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  Blynk.virtualWrite(V0, DHT.temperature);
  Blynk.virtualWrite(V1, DHT.humidity);
  Blynk.virtualWrite(V2, lux);
  Blynk.virtualWrite(V3, moisture_percentage);


  delay(1000);

}
