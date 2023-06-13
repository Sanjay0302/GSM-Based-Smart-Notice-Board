#include<HardwareSerial.h>
HardwareSerial mySerial (2);

void setup()
{
  Serial.begin(115200);
  while(!Serial){
    ;
  }

  Serial.println("Goodnight moon!");

  mySerial.begin(38400);
  mySerial.println("hello");
}

void loop(){
  if(mySerial.available()) Serial.write(mySerial.read());
  if(Serial.available()) mySerial.write(Serial.read());

}


// #basic
