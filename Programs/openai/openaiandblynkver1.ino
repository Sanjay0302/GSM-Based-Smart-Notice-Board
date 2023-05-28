// recieve text and display back to the terminal

#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// uncomment and replace id
// #define BLYNK_TEMPLATE_ID "TMPL3....."
// #define BLYNK_TEMPLATE_NAME "New Tempkete"
// #define BLYNK_AUTH_TOKEN "<Your_token>"
WidgetTerminal terminal(V10);
const char* ssid = "sanjay";
const char* password = "12345678";
const char* chatgpt_token = "your_api";
String chatgpt_Q = "Are you there";

String Previoustext;
BLYNK_WRITE(V10)
{
  chatgpt_Q = param.asStr();

   terminal.println("Your text") ;
  terminal.println(chatgpt_Q + "\n") ;
   
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (!Serial);

  // wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Blynk.config(BLYNK_AUTH_TOKEN);

  // Connect to Blynk
  Blynk.connect();

  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Blynk.run();
  if (chatgpt_Q != Previoustext) {
    callopenai();
  }
}

void callopenai() {
  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);

    // String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": ") + chatgpt_Q + String(", \"temperature\": 0, \"max_tokens\": 7}");
String payload = "{\"model\": \"text-davinci-003\", \"prompt\": \" " + chatgpt_Q + "\", \"temperature\": 0, \"max_tokens\": 7}";

    Serial.print("[HTTPS] POST...\n");

    // start connection and send HTTP header
    int httpCode = https.POST(payload);

    // httpCode will be negative on error
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String response = https.getString();
      https.end();

      // Parse the JSON response
      DynamicJsonDocument jsonDoc(1024);
      DeserializationError error = deserializeJson(jsonDoc, response);

      if (error) {
        Serial.println("Failed to parse JSON");
        return;
      }

      // Extract the desired values
      String promptTokens = jsonDoc["choices"][0]["prompt_tokens"].as<String>();
      String completionTokens = jsonDoc["choices"][0]["completion_tokens"].as<String>();
      String totalTokens = jsonDoc["choices"][0]["total_tokens"].as<String>();
      String text = jsonDoc["choices"][0]["text"].as<String>();

      // Print the extracted values
      Serial.println("Prompt Tokens: " + promptTokens);
      Serial.println("Completion Tokens: " + completionTokens);
      Serial.println("Total Tokens: " + totalTokens);
      Serial.println("Text: " + text);

      terminal.println("Responce is :") ;
      terminal.println(text) ;
    }
    else {
      Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  Serial.println("Wait 10s before next round...");
  delay(10000);

  Previoustext = chatgpt_Q;
}
