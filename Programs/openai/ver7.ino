// implemented superchart for totaltokenused
// used led matrix widget to display total token and creditused
// added new command for /restart
// added sliders and command to manage the  MAXtoken and tempreture

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <EEPROM.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3....."
#define BLYNK_TEMPLATE_NAME "New Tempkete"
#define BLYNK_AUTH_TOKEN "GwxIi.............."

String performOpenAIRequest();
void parseOpenAIResponse(const String& response);
WidgetTerminal terminal(V10);

const char* ssid = "sanjay";
const char* password = "12345678";
const char* chatgpt_token = "sk-aa................";
const char* MODEL_NAME = "text-davinci-003";  // uses 0.02$ for 1000tokens

unsigned long previousMillis = 0;
unsigned long interval = 60 * 1000;  // Interval of 1 minute in milliseconds

float costPer1000Tokens = 0.02;
float creditUsed = 0.0;

String chatgpt_Q = "Tell Hi";
String previousText;

String valueString;
int value;

int MAX_TOKENS = 5;
double TEMPERATURE = 0.0;
//2000 is the price i used for 0.04$ which i am used already so i am setting it as offset
int overalltotalTokens = 2000;  // Variable to store the total token count



BLYNK_WRITE(V10) {
  String command = param.asStr();
  if (command.startsWith("/mt ")) {
    valueString = command.substring(4);
    value = valueString.toInt();
    if (value >= 5) {
      MAX_TOKENS = value;
      terminal.println("Maxtoken set to : " + String(MAX_TOKENS));
    } else
      terminal.println("Minimum MAXToken is 5");
  } else if (command.startsWith("/reboot")) {
    terminal.println("Rebooting ESP32..");
    delay(2000);
    ESP.restart();

  }

  else if (command == "/reset") {  // Handle the reset command
    terminal.println("Performing EEPROM reset...");
    overalltotalTokens = 0;
    EEPROM.put(0, overalltotalTokens);
    EEPROM.commit();
    Blynk.virtualWrite(V3, overalltotalTokens);
  } else if (command.startsWith("/t ")) {
    valueString = command.substring(3);
    float floatValue = valueString.toFloat();

    if (floatValue < 0 && floatValue > 1)
      terminal.println("Temperature must be within 0 to 1\n");
    else {
      TEMPERATURE = floatValue;
      terminal.println("Temperature set to : " + String(TEMPERATURE));
    }
  } else {
    chatgpt_Q = param.asStr();
    terminal.println("\nResponse is being generated\n");
  }
}

BLYNK_WRITE(V6) {

  MAX_TOKENS = param.asInt();
  terminal.println("MAX_TOKENS set to : " + String(MAX_TOKENS));
}
BLYNK_WRITE(V7) {
  TEMPERATURE = param.asDouble();
  terminal.println("TEMPERATURE set to : " + String(TEMPERATURE));
}



void setup() {
  Serial.begin(115200);

  setupWiFi();
  connectToBlynk();

  // Read the total token count from EEPROM
  EEPROM.begin(4);  // Reserve 4 bytes for the total token count
  // EEPROM.put(0, overalltotalTokens); //to put the value
  EEPROM.get(0, overalltotalTokens);  // to get the value
  EEPROM.end();
  terminal.println("\nTotal tokens used till now: " + String(overalltotalTokens));
  updatecredit();
}

void loop() {
  Blynk.run();

  if (chatgpt_Q != previousText) {
    String response = performOpenAIRequest();
    parseOpenAIResponse(response);
    updatecredit();
  }
}

void updatecredit() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { 
    previousMillis = currentMillis;

    // Calculate the credit used based on the overall token count
    creditUsed = (overalltotalTokens / 1000.0) * costPer1000Tokens;

    Serial.print("Credit used: $");
    Serial.printf("%.3f\n", creditUsed);                  // to display upto 3 decimal places
    Blynk.virtualWrite(V4, "$" + String(creditUsed, 3));  // to display upto 3 decimal places
  }
}

void blinkled() {
  digitalWrite(2, HIGH);  // Turn on the LED
  delay(1000);            // Wait for 1 second
  digitalWrite(2, LOW);   // Turn off the LED
  delay(1000);
}

void setupWiFi() {
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial)
    ;

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    blinkled();
    Serial.print(".");
  }
  terminal.println("Connected to WiFi \n");
  terminal.println("\nInitially MAX_TOKEN is set to\t" + String(MAX_TOKENS) + "\n\nYou can change it anytime using /mt command with a value ranging between 0 and 1");
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
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, response);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int promptTokens = jsonDoc["usage"]["prompt_tokens"].as<int>();
  int completionTokens = jsonDoc["usage"]["completion_tokens"].as<int>();
  int currenttotalTokens = jsonDoc["usage"]["total_tokens"].as<int>();
  overalltotalTokens += currenttotalTokens;  // Add the tokens used to the total token count
  Blynk.virtualWrite(V3, overalltotalTokens);
  // Save the updated total token count to EEPROM
  EEPROM.begin(4);
  EEPROM.put(0, overalltotalTokens);
  EEPROM.commit();
  EEPROM.end();

  String text = jsonDoc["choices"][0]["text"].as<String>();

  Serial.println("Prompt Tokens: " + String(promptTokens) + "\nCompletion Tokens: " + String(completionTokens) + "\nTotal Tokens: " + String(currenttotalTokens) + "\nText: " + text);
  terminal.println("\nUsage:\nPrompt Tokens: " + String(promptTokens) + "\nCompletion Tokens: " + String(completionTokens) + "\nTotal Tokens: " + String(currenttotalTokens));
  terminal.println("Your text: " + chatgpt_Q);
  terminal.println("Response: " + text);
  previousText = chatgpt_Q;
}
