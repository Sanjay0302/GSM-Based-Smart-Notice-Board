// previous commit not working

#define BLYNK_TEMPLATE_ID "TMPL358V3gw6r"
  #define BLYNK_TEMPLATE_NAME "v101"
  #define BLYNK_AUTH_TOKEN "CLfyXI8VGnmuN1NjhnmEJTVO9Ff3h6oo"

  #define BLYNK_PRINT Serial

  #include <WiFi.h>
  #include <BlynkSimpleEsp32.h>
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

  // Your WiFi credentials.
  // Set password to "" for open networks.
  char ssid[] = "sanjay";
  char pass[] = "12345678";
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

  struct animations
  {
    textEffect_t   anim_in;     //animation type
    textEffect_t   anim_out;    //animation type
    const char *   textOut;     //text to display
    uint16_t       speed;       //animation speed (multiplier for library default)
    uint16_t       pause;       //delay (multiplier * 500ms)
    textPosition_t just;        //justify
  };

  animations animList[] = 
  {
      { PA_SCROLL_LEFT,PA_SCROLL_LEFT ,message1,4,0,PA_LEFT},
      { PA_SLICE,PA_GROW_DOWN, message2,3,0,PA_CENTER},
      { PA_RANDOM,PA_GROW_DOWN, message3,2,0,PA_CENTER},
      // { PA_SCROLL_LEFT,PA_SCROLL_LEFT, "SC_LT",5,0,PA_LEFT},
      // { PA_BLINDS,PA_GROW_DOWN, "BLINDs",2,2,PA_CENTER},
      // { PA_SCROLL_DOWN,PA_SCROLL_DOWN_LEFT, "see",4,2,PA_LEFT},
      // { PA_SCROLL_UP_RIGHT,PA_SCROLL_UP_RIGHT, "you",4,2,PA_RIGHT},
      // { PA_SCROLL_DOWN_RIGHT,PA_SCROLL_DOWN_RIGHT, "soon",4,2,PA_CENTER}

  };
  void setup()
  {
    // Debug console
    Serial.begin(115200);

    //Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
    //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

    P.begin();
      for(uint8_t i=0; i<ARRAY_SIZE(animList);i++)
    {
        animList[i].speed *= P.getSpeed(); animList[i].pause*=500;

    }

  }

  uint8_t i=0; //text effect index

  void loop() {
    Blynk.run(); // Run the Blynk library's internal tasks
    // P.displayText("", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); // Initialize all text output to be in the middle
    // P.displayAnimate();

    // char* messages[] = {message1, message2, message3}; // Array of message pointers
    // int numMessages = sizeof(messages) / sizeof(messages[0]); // Calculate the number of messages
    if(P.displayAnimate()) //animate and return true when an animation is completed
    {
      if (i == ARRAY_SIZE(animList))i=0; //reset loop index
      P.displayText(animList[i].textOut,animList[i].just,animList[i].speed,animList[i].pause,animList[i].anim_in,animList[i].anim_out);
      delay(500);
    i++;  //then start next text effect
    }



  }
