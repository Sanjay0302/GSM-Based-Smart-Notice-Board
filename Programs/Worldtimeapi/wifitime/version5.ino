//0:A error exist but minimized
// functions created for every task
// wifi connection tracking logic

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
const char* timeApiUrl = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
int previousMinute = -1;

void setup() {
  Serial.begin(115200);
  P.begin();

  // Connect to Wi-Fi
  wificonnect();

  // Get and display current time
}
void wificonnect() {
  int takerest=1;
  int round=1;
  Serial.println("Connecting to WiFi...");
  Serial.print("Take\t");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
   
    Serial.print(takerest);
    if(takerest == 10) {delay(10000); takerest=0;Serial.println();Serial.print("Round : "); Serial.println(++round);}
    
    takerest++;
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Do nothing in the loop

myclock();
}

void myclock(){
  String currentTime = getTimeFromAPI();
  String formattedTime = formatTime(currentTime);

  int minutes = formattedTime.substring(3, 5).toInt();

  if (minutes != previousMinute) {
    displayTime(formattedTime);
    previousMinute = minutes;
  }
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

    // Check if the time string is in the expected format
    if (currentTime.length() >= 19) {
      currentTime.remove(0, 11);  // Remove date portion, keep only time
      currentTime.remove(5);      // Remove seconds
    } 
  } 
  else {
    Serial.print("Error retrieving time from API. HTTP error code: ");
    Serial.println(httpResponseCode);
    myclock();
  }
  return currentTime;

  http.end();
  // return currentTime;
}

String formatTime(const String& time) {
  int hours = time.substring(0, 2).toInt();
  String period = "A";

  if (hours >= 12) {
    period = "P";
    if (hours > 12) {
      hours -= 12;
    }
  }

  String formattedTime = String(hours) + ":" + time.substring(3) + period;
  return formattedTime;
}

void displayTime(const String& time) {

  Serial.println(time);  // Print time to serial monitor
  P.displayText(time.c_str(), PA_CENTER, P.getSpeed(), P.getPause(), PA_PRINT, PA_NO_EFFECT);
}
