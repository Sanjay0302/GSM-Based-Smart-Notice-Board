

//resolved the delay caused between led and virtual display 
// reduced the input taking virtual pin condition
// added superchart widget for no. of message count 
// TOTAL_MSG_ACCEPTED 3  
// MSG_ACCEPTED_PER_SESSION 50
// msg count feature make a count of all msg recieved when the count reaches  MSG_ACCEPTED_PER_SESSION then stop accepting the input and only allow the further input only after he resets the count
// reset button always clears the msgs but when  MSG_ACCEPTED_PER_SESSION is reached until then only messages are reset but the count is still counting

#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
#define BLYNK_TEMPLATE_NAME "v101"
#define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

#define BLYNK_PRINT Serial

#include <string.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_MESSAGE_LENGTH 100
#define TOTAL_MSG_ACCEPTED 3  //if u change this number u have to change some lines the program too
#define MSG_ACCEPTED_PER_SESSION 50
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  //If you're only using a single 32x8 display then please set this to 4.

#define CLK_PIN 18   //yellow
#define DATA_PIN 19  //green
#define CS_PIN 5     //orange

// MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); //@sanjaybyranna
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "sanjaysb";
char pass[] = "123456789";
char auth[] = BLYNK_AUTH_TOKEN;
char mode[] = "wifi";




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

// delay between messages
int delayValue;  //default value
int delayValue_ms;
int messagecount = 0;
BLYNK_WRITE(V4) {
  delayValue = param.asInt();
  delayValue_ms = 1000 * delayValue;
  Blynk.virtualWrite(V2, "Delay changed");
  // we convert this to milli sec later in loop
}

BLYNK_WRITE(V5) {
  messagecount++;
  Blynk.virtualWrite(V3, messagecount);

  if (messagecount == MSG_ACCEPTED_PER_SESSION) {
    Blynk.virtualWrite(V3, "You Reached the daily limit");
    Blynk.virtualWrite(V5, "You Reached the daily limit for this session and further inputs are neglected.\nYou can reset the device to start the new counting.\n\n");
  } else {
    bool isFilled = false;
    char* messages[] = { message1, message2, message3 };

    if (messagecount <= MSG_ACCEPTED_PER_SESSION) {
      for (int i = 0; i < TOTAL_MSG_ACCEPTED; i++) {
        if (!isFilled) {
          if (!strlen(messages[i])) {
            strncpy(messages[i], param.asStr(), MAX_MESSAGE_LENGTH);

            Serial.println(String(" Message ") + messagecount + String(" Received"));
            Blynk.virtualWrite(V2, String(" Message ") + messagecount + String(" Received"));      //screen display
            Blynk.virtualWrite(V5, String(" Message ") + messagecount + String(" Received\n\n"));  //terminal display
            isFilled = true;
          }
        } else {
          break;
        }
      }

      if (!isFilled) {
        rotateMessages(param.asStr());
        Serial.println(String(" Message ") + messagecount + String(" Received"));
        Blynk.virtualWrite(V2, String(" Message ") + messagecount + String(" Received"));    // screen output
        Blynk.virtualWrite(V5, String(" Message ") + messagecount + String(" Received\n"));  //terminal display
      }
    }
  }
}

// Reset the mesages
BLYNK_WRITE(V7) {
  int reset = param.asInt();

  if (reset == 1) {
    Serial.println("Reset Performed");
    strncpy(message1, " ", MAX_MESSAGE_LENGTH);
    strncpy(message2, " ", MAX_MESSAGE_LENGTH);
    strncpy(message3, " ", MAX_MESSAGE_LENGTH);
    Blynk.virtualWrite(V8, " "); //clear the virtual display
    // Only reset count after the MSG_accepted for session is above the limit
    if (messagecount >= MSG_ACCEPTED_PER_SESSION) {
      messagecount = 0;
      Blynk.virtualWrite(V5, "Counting Reset\n\n");
    }  // Only reset count after the MSG_accepted for session is above the limit
  }
  Blynk.virtualWrite(V5, reset == 0 ? "Reset performed\n\n" : "");  //terminal display
  Blynk.virtualWrite(V2, reset == 0 ? "Reset performed\n\n" : "");
}

struct animations {
  textEffect_t anim_in;   //animation type
  textEffect_t anim_out;  //animation type
  const char* textOut;    //text to display
  uint16_t speed;         //animation speed (multiplier for library default)
  uint16_t pause;         //delay (multiplier * 500ms)
  textPosition_t just;    //justify
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

void setup() {
  // Debug console
  Serial.begin(115200);

  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  Blynk.virtualWrite(V8, " "); //clear the virtual display
  Blynk.virtualWrite(V1, mode);  // wifi or gsm
  Blynk.virtualWrite(V5, "Using WiFi\nConnected to Blynk...\n\n");
  Blynk.virtualWrite(V5, "This Program is configured to dispaly only three recent messages\nYou can now enter the messages\n\n");
  Blynk.virtualWrite(V2, "Blynk Started");
  P.begin();

  for (uint8_t i = 0; i < ARRAY_SIZE(animList); i++) {
    animList[i].speed *= P.getSpeed();
    animList[i].pause *= 1000;
  }
}

uint8_t i = 0;  //text effect index

void loop() {
  Blynk.run();  // Run the Blynk library's internal tasks


  char* messages[] = { message1, message2, message3 };  // Array of message pointers
  // int numMessages = sizeof(messages) / sizeof(messages[0]);  // Calculate the number of messages
  if (P.displayAnimate())  //animate and return true when an animation is completed
  {
    if (i == TOTAL_MSG_ACCEPTED) i = 0;  //reset loop index
    //Dont display the message if the recieved new message is null value or a space and also avoid the repeated messages
    if (messages[i][0] != '\0' && messages[i][0] != ' ' ) {
      P.displayText(animList[i].textOut, animList[i].just, animList[i].speed, animList[i].pause, animList[i].anim_in, animList[i].anim_out);
      delay(delayValue_ms);
      Blynk.virtualWrite(V8, messages[i]);
    }

    i++;  //then start next text effect
  }
}




// To optimize the provided program, you can consider the following suggestions:

// Reduce memory usage:

// Instead of using fixed-size character arrays for messages (message1, message2, message3), you can use dynamic memory allocation to save memory. For example, you can use String objects or dynamically allocated character arrays (using new and delete operators).
// Instead of using a fixed-size array (animList[]) for animations, you can use a dynamic data structure like a linked list or vector to add or remove animations as needed.
// Avoid using String class:

// The String class in Arduino uses dynamic memory allocation, which can lead to memory fragmentation and instability. It's better to use character arrays (char[]) for storing and manipulating strings.
// Avoid concatenating strings with the + operator in print statements. Instead, use snprintf or strcpy to copy strings into a single buffer and then print the buffer.
// Optimize Blynk updates:

// Instead of sending virtual writes for each message count update, you can update the value periodically or only when it changes significantly.
// Consider reducing the number of virtual writes to the terminal display (V5) and using a buffer to accumulate multiple messages before sending them together.
// Minimize code duplication:

// You can avoid duplicating code for handling messages by using loops and arrays more effectively. For example, you can store the messages in an array and iterate over them instead of explicitly checking each message variable (message1, message2, message3).
// Use PROGMEM for constant data:

// If you have constant data that doesn't change during runtime (e.g., animation text), you can store it in program memory (PROGMEM) instead of RAM to save space.
// Profile and optimize the animations:

// Measure the execution time of the animation functions and optimize them if necessary. Look for any delays or bottlenecks that can be eliminated or reduced.
// Consider using interrupts or timers:

// If possible, use interrupts or timers instead of delay functions to allow for better multitasking and responsiveness in your program.




