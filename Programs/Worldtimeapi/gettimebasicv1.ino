#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* apiUrl = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";

int previousMinute = -1; // Variable to store the previous minute value

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK) {
      String response = http.getString();
      DynamicJsonDocument json(1024);
      deserializeJson(json, response);

      const char* currentTime = json["datetime"].as<const char*>();

      // Extract hours and minutes from the time string
      int hours = atoi(currentTime + 11);
      int minutes = atoi(currentTime + 14);

      // Convert 24-hour format to 12-hour format
      int am_pm = hours >= 12 ? 1 : 0;
      hours = hours % 12;
      if (hours == 0) {
        hours = 12;
      }

      // Check if the minute value has changed
      if (minutes != previousMinute) {
        Serial.print("Current time: ");
        Serial.print(hours);
        Serial.print(":");
        if (minutes < 10) {
          Serial.print("0");
        }
        Serial.print(minutes);
        Serial.print(" ");
        Serial.println(am_pm ? "PM" : "AM");

        // Update the previousMinute variable
        previousMinute = minutes;
      }
    }
    else {
      Serial.print("HTTP request failed with error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(1000); // Wait for 1 second before making the next request
}
