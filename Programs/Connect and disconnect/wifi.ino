/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "sanjay";
char pass[] = "12345678";

const int switchPin = 25;
void connectwifi() 
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, pass);
  Serial.println("Blynk wifi connect in progress..");
  // Configure Blynk connection
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Connect to Blynk
  Blynk.connect();
  Serial.println("blynk wifi connection success.");
}

void disconnectwifi()
 {
  // Switch is high, disconnect Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Disconnecting Wi-Fi...");
    WiFi.disconnect();
      Serial.println("blynk....");
    Blynk.disconnect();
    delay(100);
  }
 }

  void setup() {
    // Debug console
    Serial.begin(115200);

    pinMode(switchPin, INPUT_PULLUP);
    int switchState = digitalRead(switchPin);
    if (switchState == LOW) {

      Serial.println("Connecting to Wi-Fi...");
      WiFi.begin(ssid, pass);
      connectwifi();
    }
    else delay(500);
  }


  void loop() 
  {

    // Check the switch state
    int switchState = digitalRead(switchPin);
    if (switchState == LOW) 
    {
      // Switch is low, connect to Wi-Fi
      if (WiFi.status() != WL_CONNECTED) {
        connectwifi();
      }
    } 
    else 
    {
      disconnectwifi();
    }
   if(WiFi.status() == WL_CONNECTED){
     Blynk.run();
   }
  }


