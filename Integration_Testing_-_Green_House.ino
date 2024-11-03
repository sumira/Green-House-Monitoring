#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Green House"
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* ssid = "TCL 30E";
const char* pass = "dee04300";

#define DHT11_PIN D5
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

#include <BH1750.h>
#include <Wire.h>
BH1750 lightMeter;

const int sensor_pin = A0;
const int led_pin = D4; 
const int motorPin = D7;
const int fanPin = D6;

unsigned long motorStartTime = 0;  
unsigned long fanStartTime = 0;    
unsigned long lastUpdateTime = 0;    
bool motorState = LOW;             
bool fanState = LOW;  
bool motorOn = false;
bool fanOn = false; 

void setup() {
  Serial.begin(115200);
  delay(100);

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
  pinMode(led_pin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(motorPin,OUTPUT);
}

void loop() {
  Blynk.run();

  DHT.read(DHT11_PIN);

  float moisture_percentage = (100.00 - ((analogRead(sensor_pin) / 1023.00) * 100.00));
  float lux = lightMeter.readLightLevel();

  // Temperature monitoring and controlling
  if(DHT.temperature > 33 && !fanOn) {
    Serial.println("Starting the fan");
    fanOn = true;
    fanState = HIGH;
    fanStartTime = millis();
    digitalWrite(fanPin, fanState);
    Blynk.logEvent("temperature_event", "Green house temperature is greater than 32C!");
  }
  if(fanState == HIGH && millis() - fanStartTime >= 3000) {
    fanState = LOW;
    digitalWrite(fanPin, fanState);
    fanOn = false;
  }

  if(DHT.humidity > 80) {
    Serial.println("High Humidity detected");
    Blynk.logEvent("humidity_event", "Green house humidity is greater than 70%!");
  }

  // Soil moisture monitoring and controlling
  if(moisture_percentage > 50) {
    Serial.println("High soil moisture detected");
    Blynk.logEvent("soil_moisture_event", "Green house soil moisture is greater than 50%!");
  } else if(moisture_percentage < 10) {
    Serial.println("Low soil moisture detected");
    Blynk.logEvent("low_soil_moisture_event", "Green house soil moisture is lower than 10%!");
  }

  if(moisture_percentage < 30 && !motorOn) {
    Serial.println("Starting the motor");
    motorOn = true;
    motorState = HIGH;
    motorStartTime = millis();
    digitalWrite(motorPin, motorState);
  }
  if(motorState == HIGH && millis() - motorStartTime >= 2000) {
    motorState = LOW;
    digitalWrite(motorPin, motorState);
    motorOn = false;
  }

  if(lux > 900) {
    Serial.println("Extreme Sunlight detected");
    Blynk.logEvent("light_intensity_event", "Green house light intensity is in a critical level!");
  }

  // Print and log data every second
  if(millis() - lastUpdateTime >= 1000) {
    lastUpdateTime = millis();  

    Serial.print("Green House Temperature is: ");
    Serial.print(DHT.temperature);
    Serial.print(" Humidity is: ");
    Serial.print(DHT.humidity);
    Serial.print(" | Soil Moisture percentage = ");
    Serial.print(moisture_percentage);
    Serial.println("%");
    Serial.print(" | Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    // Send data to Blynk every second
    Blynk.virtualWrite(V0, DHT.temperature);
    Blynk.virtualWrite(V1, DHT.humidity);
    Blynk.virtualWrite(V2, lux);
    Blynk.virtualWrite(V3, moisture_percentage);
  }
}
