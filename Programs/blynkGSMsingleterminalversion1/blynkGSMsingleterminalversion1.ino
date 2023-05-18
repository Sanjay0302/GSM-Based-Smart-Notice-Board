// connect through gsm and display on max led's
#define BLYNK_PRINT Serial


#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "www";
char user[] = "";
char pass[] = "";

// or Software Serial on Uno, Nano
#include <HardwareSerial.h>
HardwareSerial SerialAT(2); // RX, TX

TinyGsm modem(SerialAT);

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_MESSAGE_LENGTH 100

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 //If you're only using a single 32x8 display then please set this to 4.

#define CLK_PIN   18 //green
#define DATA_PIN  19 //orange
#define CS_PIN    5 //yellow

// MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); //@sanjaybyranna
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


char auth[] = BLYNK_AUTH_TOKEN;


int delayValue;
int delayValue_ms;

bool isMessage1Filled = false;
bool isMessage2Filled = false;
bool isMessage3Filled = false;

char message1[MAX_MESSAGE_LENGTH];
char message2[MAX_MESSAGE_LENGTH];
char message3[MAX_MESSAGE_LENGTH];

void rotateMessages(const char* newMessage) {
  // Rotate messages: move message2 to message1, message3 to message2, and add new message to message3
  strncpy(message1, message2, MAX_MESSAGE_LENGTH);
  strncpy(message2, message3, MAX_MESSAGE_LENGTH);
  strncpy(message3, newMessage, MAX_MESSAGE_LENGTH);
}


BLYNK_WRITE(V4) {
  delayValue = param.asInt();
  if(delayValue == 0) delayValue = 1;
  delayValue_ms = (delayValue * 1000);
}
BLYNK_WRITE(V5) {
//   pinValue1 = param.asString();
     if (!isMessage1Filled) {
    // message1 is empty, so store new message there
    strncpy(message1, param.asStr(), MAX_MESSAGE_LENGTH);
     Serial.println("Msg 1 Ready");

    isMessage1Filled = true;
  } else if (!isMessage2Filled) {
    // message1 is filled, but message2 is empty, so store new message there
    strncpy(message2, param.asStr(), MAX_MESSAGE_LENGTH);
     Serial.println("Msg 2 Ready");
    isMessage2Filled = true;
  } else if (!isMessage3Filled) {
    // message1 and message2 are filled, but message3 is empty, so store new message there
    strncpy(message3, param.asStr(), MAX_MESSAGE_LENGTH);
     Serial.println("Msg 3 Ready");
    isMessage3Filled = true;
  } else {
    // All messages are filled, so rotate messages and store new message in message3
    rotateMessages(param.asStr());
     Serial.println("rotate Ready");
  }

}

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

  Blynk.begin(auth, modem, apn, user, pass);

  P.begin();

}

void loop()
{
  Blynk.run();
  
  P.displayText("", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); //initialise all text output to be in the middle
  P.displayAnimate();

  P.print(message1); //Output the string value found in the 1st Blynk IoT app terminal to the matrix display 
  Blynk.virtualWrite(V8, message1); //Display the 1st string value from the matrix display to the V8 label (on IoT app)
  delay(delayValue_ms); //pause between switching from value 1 to value 2
  
  P.print(message2); //Output the string value found in the 2nd Blynk IoT app terminal to the matrix display 
  Blynk.virtualWrite(V8, message2); //Display the 2nd string value from the matrix display to the V8 label (on IoT app)
  delay(delayValue_ms); //pause between switching from value 2 to value 3
  
  P.print(message3); //Output the string value found in the 3rd Blynk IoT app terminal to the matrix display
  Blynk.virtualWrite(V8, message3); //Display the 3rd string value from the matrix display to the V8 label (on IoT app)
  delay(delayValue_ms); //pause between switching from value 3 and back to value 1
  
}
