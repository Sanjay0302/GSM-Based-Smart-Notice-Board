// this will print the default speed and pause of max7219 based led matrix
// for our module FC16_HW we have this
// speed : 10
// pause : 100

// this program has updown animation thus make the bounce animation
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   18
#define DATA_PIN  19
#define CS_PIN    5

// Hardware SPI connection
// MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup(void)
{   Serial.begin(115200);
  P.begin();
  int getpause = P.getPause();
  int getspeed = P.getSpeed();
  Serial.print("Speed : ");
  Serial.println(getspeed);
  Serial.print("Pause : ");
  Serial.println(getpause);
}

void loop(void)
{
  if (P.displayAnimate())
    P.displayText("hello how are u", PA_CENTER, P.getSpeed(), P.getPause(), PA_SCROLL_DOWN, PA_SCROLL_UP);

}
