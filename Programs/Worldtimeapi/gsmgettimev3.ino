// third program in folder
// this will connect to the internet using gsm and also get the time data

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define TINY_GSM_MODEM_SIM800

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

const char* apiUrl = "http://worldtimeapi.org/api/timezone/Asia/Kolkata";
const int switchPin = 25;

int previousMinute = -1; // Variable to store the previous minute value

// Blynk credentials and GSM configuration
#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

char apn[] = "www";
char user[] = "";
char pass[] = "";

HardwareSerial SerialAT(2); // RX, TX
TinyGsm modem(SerialAT);

void setup() {
  Serial.begin(115200);
  SerialAT.begin(115200);
  delay(3000);
  Serial.println("Initializing modem...");
  modem.restart();
  Blynk.begin(BLYNK_AUTH_TOKEN, modem, apn, user, pass);
  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  Blynk.run();
  
  if (digitalRead(switchPin) == LOW) {
    fetchTime();
  }
}

void fetchTime() {
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

