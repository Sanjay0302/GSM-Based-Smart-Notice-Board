
#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
// Select your modem:
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

char apn[] = "www";
char user[] = "";
char pass[] = "";


#include <HardwareSerial.h>
HardwareSerial SerialAT(2);  // RX, TX

TinyGsm modem(SerialAT);

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_MESSAGE_LENGTH 100
#define TOTAL_MSG_ACCEPTED 3  // if u change this number u have to change some lines the program too

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // If you're only using a single 32x8 display then please set this to 4.

#define CLK_PIN 18   // yellow
#define DATA_PIN 19  // green
#define CS_PIN 5     // orange
int MSG_ACCEPTED_PER_SESSION = 50;
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

char auth[] = BLYNK_AUTH_TOKEN;

// delay between messages
int delayValue;  // default value
int delayValue_ms;
int messagecount = 0;
// these 2 are for command exception
String valueString;
int value;

bool isMessage1Filled = false;
bool isMessage2Filled = false;
bool isMessage3Filled = false;

char message1[MAX_MESSAGE_LENGTH];
char message2[MAX_MESSAGE_LENGTH];
char message3[MAX_MESSAGE_LENGTH];

void rotateMessages(const char *newMessage) {
  // Rotate messages: move message2 to message1, message3 to message2, and add new message to message3
  strncpy(message1, message2, MAX_MESSAGE_LENGTH);
  strncpy(message2, message3, MAX_MESSAGE_LENGTH);
  strncpy(message3, newMessage, MAX_MESSAGE_LENGTH);
}

BLYNK_WRITE(V4) {
  delayValue = param.asInt();
  delayValue_ms = 1000 * delayValue;
  Blynk.virtualWrite(V2, "Delay changed");
}

void updateAcceptedPerSession(int value) {
  if (value > 0) {
    MSG_ACCEPTED_PER_SESSION = value;
    Blynk.virtualWrite(V5, "MSG_ACCEPTED_PER_SESSION updated to " + String(value) + "\n");
  } else {
    Blynk.virtualWrite(V5, "Invalid value. Please enter a positive integer.\n");
  }
}

void promtback() {
  // Serial.println(String(" Message ") + messagecount + String(" Received"));
  Blynk.virtualWrite(V2, String(" Message ") + messagecount + String(" Received"));      // screen output
  Blynk.virtualWrite(V5, String(" Message ") + messagecount + String(" Received\n\n"));  // terminal display
}

BLYNK_WRITE(V5) {
  String command = param.asStr();
  if (command.startsWith("/restart")) {
    Blynk.virtualWrite(V5, "Restarting...\n");
    ESP.restart();
  }
  // message count reset
  else if (command.startsWith("/mcr")) {
    messagecount = 0;
    Blynk.virtualWrite(V5, "messagecount reset success\n");
  } else if (command.startsWith("/maps ")) {
    valueString = command.substring(6);
    value = valueString.toInt();
    // if messagecount exceed the updated value then
    if (value <= messagecount) {
      Blynk.virtualWrite(V5, "messagecount exceeds the value you gave.\nreset and try again\n");
    } else
      updateAcceptedPerSession(value);
  } else {
    messagecount++;
    Blynk.virtualWrite(V3, messagecount);

    if (messagecount == MSG_ACCEPTED_PER_SESSION) {

      Blynk.virtualWrite(V5, "You Reached the daily limit for this session and further inputs are neglected.\nYou can reset the device to start the new counting.\nor send '/mcr' command to clear message count only ");
    } else {
      bool isFilled = false;
      char *messages[] = { message1, message2, message3 };

      if (messagecount <= MSG_ACCEPTED_PER_SESSION) {
        for (int i = 0; i < TOTAL_MSG_ACCEPTED; i++) {
          if (!isFilled) {
            if (!strlen(messages[i])) {
              strncpy(messages[i], param.asStr(), MAX_MESSAGE_LENGTH);

              promtback();
              isFilled = true;
            }
          } else {
            break;
          }
        }
        if (!isFilled) {
          rotateMessages(param.asStr());
          promtback();
        }
      } else {
        Blynk.virtualWrite(V5, "limit reached first reset\n\n");
      }
    }
  }
}

// Reset the mesages
BLYNK_WRITE(V7) {
  MSG_ACCEPTED_PER_SESSION = 50;  // reset /maps value to default as 50
  int reset = param.asInt();
  if (reset == 1) {
    Serial.println("Reset Performed");
    strncpy(message1, " ", MAX_MESSAGE_LENGTH);
    strncpy(message2, " ", MAX_MESSAGE_LENGTH);
    strncpy(message3, " ", MAX_MESSAGE_LENGTH);
    Blynk.virtualWrite(V8, "/Cleared");  // clear the virtual display
    // Only reset count after the MSG_accepted for session is above the limit
    if (messagecount >= MSG_ACCEPTED_PER_SESSION) {
      messagecount = 0;
      Blynk.virtualWrite(V5, "Counting Reset\n");
    }  // Only reset count after the MSG_accepted for session is above the limit
  }
  Blynk.virtualWrite(V5, reset == 0 ? "Reset performed\n\n" : "");  // terminal display
  Blynk.virtualWrite(V2, reset == 0 ? "Reset performed\n" : "");
}

struct animations {
  textEffect_t anim_in;   // animation type
  textEffect_t anim_out;  // animation type
  const char *textOut;    // text to display
  uint16_t speed;         // animation speed (multiplier for library default)
  uint16_t pause;         // delay (multiplier * 500ms)
  textPosition_t just;    // justify
};

animations animList[] = {
  //{anim_in,anim_out,textout,speed,pause,justify}

  { PA_SCROLL_LEFT, PA_SCROLL_LEFT, message1, 3, 0, PA_LEFT },
  { PA_SCROLL_LEFT, PA_SCROLL_LEFT, message2, 3, 0, PA_LEFT },
  { PA_SCROLL_LEFT, PA_SCROLL_LEFT, message3, 3, 0, PA_LEFT },
  // { PA_SCROLL_LEFT,PA_SCROLL_LEFT, "SC_LT",5,0,PA_LEFT},
  // { PA_BLINDS,PA_GROW_DOWN, "BLINDs",2,2,PA_CENTER},
  // { PA_SCROLL_DOWN,PA_SCROLL_DOWN_LEFT, "see",4,2,PA_LEFT},
  // { PA_SCROLL_UP_RIGHT,PA_SCROLL_UP_RIGHT, "you",4,2,PA_RIGHT},
  // { PA_SCROLL_DOWN_RIGHT,PA_SCROLL_DOWN_RIGHT, "soon",4,2,PA_CENTER}

};

// this should be after animlist[] structure member only esle we get error as not declared animlist[]
BLYNK_WRITE(V6) {
  int sliderValue = param.asInt();
  // map the slider value to the desired animation speed range
  int minSliderValue = 1;
  int maxSliderValue = 10;
  int minAnimationSpeed = 1;
  int maxAnimationSpeed = 10;

  // calculate the animation speed based on the mapped slider value
  int animationSpeed = map(sliderValue, minSliderValue, maxSliderValue, minAnimationSpeed, maxAnimationSpeed);
  char animtext[20] = "Speed set as:\t";
  Blynk.virtualWrite(V2, "Speed Updated");
  // adjust the animation speed for all animations in the animList
  for (uint8_t i = 0; i < ARRAY_SIZE(animList); i++) {
    animList[i].speed = animationSpeed;
  }
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V2, "Blynk Started");
  Blynk.virtualWrite(V8, "/Started");    // clear the virtual display
  Blynk.virtualWrite(V1, "WiFi / ...");  // wifi or gsm
  Blynk.virtualWrite(V5, "Using WiFi\nConnected to Blynk...\n\n");
  Blynk.virtualWrite(V5, "This Program is configured to dispaly only three recent messages\nYou can now enter the messages\n\n");
}

unsigned long startTime;
unsigned long previousMinute = 0;
void setup() {
  // Debug console
  Serial.begin(115200);
  startTime = millis();
  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(2000);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  // modem.restart();
  Serial.println("Initializing modem...");
  modem.init();

  Blynk.begin(auth, modem, apn, user, pass);

  P.begin();
  for (uint8_t i = 0; i < ARRAY_SIZE(animList); i++) {
    animList[i].speed *= P.getSpeed();
    animList[i].pause *= 1000;
  }
}

uint8_t i = 0;  // text effect index
void loop() {
  ontimedisplay();
  Blynk.run();

  char *messages[] = { message1, message2, message3 };  // Array of message pointers

  if (P.displayAnimate()) {  // Animate and return true when an animation is completed
    if (i == TOTAL_MSG_ACCEPTED)
      i = 0;  // Reset loop index

    if (messages[i][0] != '\0' && messages[i][0] != ' ') {
      unsigned long animationStartTime = millis();

      while (millis() - animationStartTime < delayValue_ms) {
        Blynk.run();
      }

      P.displayText(animList[i].textOut, animList[i].just, animList[i].speed, animList[i].pause, animList[i].anim_in, animList[i].anim_out);
      Blynk.virtualWrite(V8, messages[i]);
    }

    i++;  // Then start the next text effect
  }
}

void ontimedisplay() {
  unsigned long elapsedTime = millis() - startTime;  // Calculate the elapsed time

  // Calculate hours and minutes
  unsigned long hours = (elapsedTime / (1000 * 60 * 60)) % 24;
  unsigned long minutes = (elapsedTime / (1000 * 60)) % 60;

  // Check if the minute has changed
  if (minutes != previousMinute) {
    // Create a formatted string for the elapsed time
    char elapsedTimeStr[13];  // "HH:MM" + null terminator
    sprintf(elapsedTimeStr, "WiFi / %02lu:%02lu", hours, minutes);

    // Print the formatted elapsed time
    Serial.print("On-time: ");
    Serial.println(elapsedTimeStr);
    Blynk.virtualWrite(V1, elapsedTimeStr);
    previousMinute = minutes;  // Update the previous minute
  }
}