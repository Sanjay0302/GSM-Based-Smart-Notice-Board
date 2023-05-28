// /mt and /t command to set maxtoken and temperature
// wifi connecting blink led 
// reduced number of line

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
int MAX_TOKENS = 5;
float TEMPERATURE = 0.0;
const char* MODEL_NAME = "text-davinci-003";

String chatgpt_Q = "Tell Hi";
String previousText;

String valueString;
int value;

BLYNK_WRITE(V10) {
  String command = param.asStr();
  if (command.startsWith("/mt ")) {
    valueString = command.substring(4);
    value = valueString.toInt();
    if (value >= 5) {
      terminal.println("Maxtoken set to : " + String(value));
      MAX_TOKENS = value;
    } else terminal.println("Minumum MAXToken is 5");
  } else if (command.startsWith("/t ")) {
    valueString = command.substring(3);
    float floatValue = valueString.toFloat();

    if (floatValue < 0 && floatValue > 1) terminal.println("temperature must be within 0 to 1\n");
    else {
      terminal.println("temperature set to : " + String(floatValue));
      TEMPERATURE = floatValue;
    }
  } else {

    chatgpt_Q = param.asStr();

    terminal.println("\nResponse is being Generated\n");
  }
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


void blinkled() {
  digitalWrite(2, HIGH);  // Turn on the LED
  delay(1000);            // Wait for 1 second
  digitalWrite(2, LOW);   // Turn off the LED
  delay(1000);
}

// function defination
void setupWiFi() {
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial)
    ;

  // Wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    blinkled();
    Serial.print(".");
  }
  terminal.println("Connected to WiFi \n");
  terminal.println("\ninitially MAX_TOKEN is SET to" + String(MAX_TOKENS) + "\nYou can change it anytime using /mt command with a value ranging between 0 and 1");
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
  Serial.println("Prompt Tokens: " + String(promptTokens) + "\nCompletion Tokens: " + String(completionTokens) + "\nTotal Tokens: " + String(totalTokens) + "\nText: " + text);
  terminal.println("\nUsage : \n Prompt Tokens: " + String(promptTokens) + "\nCompletion Tokens: " + String(completionTokens) + + "\nTotal Tokens: " + String(totalTokens));
  terminal.println("Your text : " + chatgpt_Q);
  terminal.println("Response is:" + text);
  previousText = chatgpt_Q;
}
