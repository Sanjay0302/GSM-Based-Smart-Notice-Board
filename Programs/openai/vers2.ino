// Filtered output.
// But still we have trailing spaces in the resulted in text.

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Library for parsing JSON

const char* ssid = "sanjay";
const char* password = "12345678";
const char* chatgpt_token = "your_api";
const char* chatgpt_Q = "\"Who are you\"";

// Function declarations
void setupWiFi();
void printConnectionInfo();
String performOpenAIRequest();
void parseOpenAIResponse(const String& response);
String timestampToDateTime(unsigned long timestamp);

void setup() {
  Serial.begin(115200);
  setupWiFi();
  printConnectionInfo();
}

void loop() {
  String response = performOpenAIRequest();
  parseOpenAIResponse(response);

  Serial.println("Wait 10s before next round...");
  delay(10000);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial);
  WiFi.begin(ssid, password);
}

void printConnectionInfo() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String performOpenAIRequest() {
  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);

    String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": ") + chatgpt_Q + String(", \"temperature\": 0, \"max_tokens\": 7}");

    Serial.print("[HTTPS] POST...\n");
    int httpCode = https.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String response = https.getString();
      // Serial.println(response);
      https.end();
      return response;
    }
    else {
      Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  return "";
}

void parseOpenAIResponse(const String& response) {
  // Parse the JSON response
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, response);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract the required information
  int promptTokens = jsonDoc["usage"]["prompt_tokens"].as<int>();
  int completionTokens = jsonDoc["usage"]["completion_tokens"].as<int>();
  int totalTokens = jsonDoc["usage"]["total_tokens"].as<int>();
  String text = jsonDoc["choices"][0]["text"].as<String>();

  // Print the extracted information
  Serial.println("Prompt Tokens: " + String(promptTokens));
  Serial.println("Completion Tokens: " + String(completionTokens));
  Serial.println("Total Tokens: " + String(totalTokens));
  Serial.println("Text: " + text);
}


