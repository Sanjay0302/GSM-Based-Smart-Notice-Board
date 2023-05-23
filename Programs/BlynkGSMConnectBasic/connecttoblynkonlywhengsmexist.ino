// connect to blynk only when the gprs is connected if not connected reconnecting process is there 

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

// Your GPRS credentials
// Leave empty if missing user or pass
char apn[] = "www";
char user[] = "";
char pass[] = "";

#include <HardwareSerial.h>
HardwareSerial SerialAT(2); // RX, TX

TinyGsm modem(SerialAT);

void setup()
{
  // Debug console
  Serial.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  // Start GSM connection
  Serial.println("Connecting to GSM network...");
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println("Failed to connect to GSM network");
    // Handle reconnection or other actions
  } else {
    Serial.println("GSM network connected");

    // Configure Blynk connection
    Serial.println("Configuring Blynk...");
    Blynk.begin(BLYNK_AUTH_TOKEN, modem, apn, user, pass);

    // Connect to Blynk
    Serial.println("Connecting to Blynk...");
    Blynk.connect();
  }
}

void loop()
{
  Blynk.run();

  // Check if GSM network is still connected
  if (!modem.isGprsConnected()) {
    Serial.println("GSM network disconnected");
    // Disconnect from Blynk
    Blynk.disconnect();
    // Handle reconnection or other actions

    // Reconnect to GSM network
    Serial.println("Reconnecting to GSM network...");
    if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println("Failed to reconnect to GSM network");
      // Handle reconnection or other actions
    } else {
      Serial.println("GSM network reconnected");

      // Configure Blynk connection
      Serial.println("Configuring Blynk...");
      Blynk.begin(BLYNK_AUTH_TOKEN, modem, apn, user, pass);

      // Reconnect to Blynk
      Serial.println("Connecting to Blynk...");
      Blynk.connect();
    }
  }
}
