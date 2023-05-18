#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   18
#define DATA_PIN  19
#define CS_PIN    5

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


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
    { PA_SCROLL_LEFT,PA_SCROLL_LEFT ,"MINI project scrolling animation",4,0,PA_LEFT},
    { PA_SLICE,PA_GROW_DOWN, "SLICE",3,2,PA_CENTER},
    { PA_RANDOM,PA_GROW_DOWN, "RANDOM",2,2,PA_CENTER},
    { PA_SCROLL_LEFT,PA_SCROLL_LEFT, "SC_LT",5,0,PA_LEFT},
    { PA_BLINDS,PA_GROW_DOWN, "BLINDs",2,2,PA_CENTER},
    { PA_SCROLL_DOWN,PA_SCROLL_DOWN_LEFT, "see",4,2,PA_LEFT},
    { PA_SCROLL_UP_RIGHT,PA_SCROLL_UP_RIGHT, "you",4,2,PA_RIGHT},
    { PA_SCROLL_DOWN_RIGHT,PA_SCROLL_DOWN_RIGHT, "soon",4,2,PA_CENTER}

};

void setup()
{
  P.begin();
  for(uint8_t i=0; i<ARRAY_SIZE(animList);i++)
  {
      animList[i].speed *= P.getSpeed(); animList[i].pause*=500;

  }
}


uint8_t i=0; //text effect index
void loop()
{

  if(P.displayAnimate()) //animate and return true when an animation is completed
  {
    if (i == ARRAY_SIZE(animList))i=0; //reset loop index
    P.displayText(animList[i].textOut,animList[i].just,animList[i].speed,animList[i].pause,animList[i].anim_in,animList[i].anim_out);
    delay(500);
  i++;  //then start next text effect
  }
}
