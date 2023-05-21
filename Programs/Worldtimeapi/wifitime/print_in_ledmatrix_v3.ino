// this program prints the time on led matrix 
// but sometimes get blank spaces due to http request gets interupted sometime
// this program is 24 hourformat


#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 18
#define DATA_PIN 19
#define CS_PIN 5

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
const char* timeApiUrl = "http://worldtimeapi.org/api/ip";

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup() {
  Serial.begin(115200);
  P.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Get and display current time

}

void loop() {
  // Do nothing in the loop
  
  String currentTime = getTimeFromAPI();
  displayTime(currentTime);
  P.displayAnimate(); 
}

String getTimeFromAPI() {
  HTTPClient http;
  String currentTime;

  // Make a GET request to the WorldTimeAPI
  http.begin(timeApiUrl);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == HTTP_CODE_OK) {
    // Parse the response JSON
    DynamicJsonDocument jsonBuffer(1024);
    deserializeJson(jsonBuffer, http.getString());
    
    // Extract the time from the JSON
    currentTime = jsonBuffer["datetime"].as<String>();
    currentTime.remove(0, 11); // Remove date portion, keep only time
    currentTime.remove(5); // Remove seconds
  }

  http.end();
  return currentTime;
}

void displayTime(const String& time) {
  Serial.println(time); // Print time to serial monitor
  P.displayText(time.c_str(), PA_CENTER, P.getSpeed(), P.getPause(), PA_PRINT, PA_NO_EFFECT);
}
