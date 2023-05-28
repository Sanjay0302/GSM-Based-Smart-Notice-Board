// modular program 
// display the usage and response in blynk iot application

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3JjrXZeCd"
#define BLYNK_TEMPLATE_NAME "New Tempkete"
#define BLYNK_AUTH_TOKEN "GwxIiAezvT6on24nZKE6RhytixgbMCWn"
// function declaration
String performOpenAIRequest();
void parseOpenAIResponse(const String& response);
WidgetTerminal terminal(V10);
const char* ssid = "sanjay";
const char* password = "12345678";
const char* chatgpt_token = "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";
const int MAX_TOKENS = 20;
const char* MODEL_NAME = "text-davinci-003";
const float TEMPERATURE = 0.0;

String chatgpt_Q = "Are you there";
String previousText;



BLYNK_WRITE(V10) {
  chatgpt_Q = param.asStr();
  terminal.println("Your text");
  terminal.println(chatgpt_Q + "\n");
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  connectToBlynk();
}

void loop() {
  Blynk.run();

  if (chatgpt_Q != previousText) {
    String response = performOpenAIRequest();
    parseOpenAIResponse(response);
  }
}


// function defination
void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial)
    ;

  // Wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToBlynk() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

String performOpenAIRequest() {
  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);

    String payload = "{\"model\": \"" + String(MODEL_NAME) + "\", \"prompt\": \"" + chatgpt_Q + "\", \"temperature\": " + String(TEMPERATURE) + ", \"max_tokens\": " + String(MAX_TOKENS) + "}";

    Serial.print("[HTTPS] POST...\n");
    int httpCode = https.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String response = https.getString();
      // Serial.println(response);
      https.end();
      return response;
    } else {
      Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
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

  terminal.println("Usage : \n");
  terminal.println("Prompt Tokens: " + String(promptTokens));
  terminal.println("Completion Tokens: " + String(completionTokens));
  terminal.println("Total Tokens: " + String(totalTokens));
  terminal.println("Response is:");
  terminal.println(text);

  previousText = chatgpt_Q;
}

