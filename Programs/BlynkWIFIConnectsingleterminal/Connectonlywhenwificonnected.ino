// connectes only after wifi is connected

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

void setup()
{
  // Debug console
  Serial.begin(115200);

  // Start Wi-Fi connection
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, pass);

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Wi-Fi connected
  Serial.println("\nWi-Fi connected");
 wifiblynk();


}
void wifiblynk(){
    // Configure Blynk connection
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Connect to Blynk
  Blynk.connect();
}
void loop()
{
  Blynk.run();

  // Check if Wi-Fi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected");
    // Handle reconnection or other actions
  }
}

