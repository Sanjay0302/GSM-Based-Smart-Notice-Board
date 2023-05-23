// when ground and swicth is connected then it means high and program starts connecting to network
// to avoid latency we used modem.init() instead of modem.restart();
// If high --> connect to gsm --> connect to blynk
// If low --> disconnect gsm --> disconnect to blynk

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
HardwareSerial SerialAT(2);  // RX, TX

TinyGsm modem(SerialAT);

const int switchPin = 25;

void setup() {
  // Debug console
  Serial.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);
  pinMode(switchPin, INPUT_PULLUP);
  int switchState = digitalRead(switchPin);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  if (switchState == HIGH) {
    Serial.println("Initializing modem...");
    modem.init();
  }

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
}

void connectToGPRS() {
  Serial.println("Initializing modem...");
      modem.init();
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

void disconnectFromGPRS() {
  // Disconnect from Blynk
  Blynk.disconnect();

  // Disconnect from GSM network
  modem.gprsDisconnect();
  Serial.println("GSM network disconnected");
}

void loop() {
  // Check if switch pin is high
  pinMode(switchPin, INPUT_PULLUP);
  int switchState = digitalRead(switchPin);

  if (switchState == HIGH) 
  {
    if (!modem.isGprsConnected()) 
    {  
      connectToGPRS();
    }

    Blynk.run();
  } 
  else 
  {
    if (modem.isGprsConnected()) 
    {
      disconnectFromGPRS();
    }
  }
}
