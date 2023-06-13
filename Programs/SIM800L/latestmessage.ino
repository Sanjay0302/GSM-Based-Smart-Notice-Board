#include <HardwareSerial.h>
HardwareSerial mySerial(2);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

  Serial.println("Goodnight moon!");

  mySerial.begin(38400);
  mySerial.println("AT");  // Send AT command to check if the module is responding
  delay(1000);  // Wait for the module to respond
  while (mySerial.available())  // Read and print the response from the module
  {
    Serial.write(mySerial.read());
  }

  mySerial.println("AT+CMGF=1");  // Set SMS text mode
  delay(1000);
  while (mySerial.available())
  {
    Serial.write(mySerial.read());
  }

  mySerial.println("AT+CMGL=\"REC UNREAD\",1");  // Read the latest unread message
  delay(1000);
  while (mySerial.available())
  {
    Serial.write(mySerial.read());
  }
}

void loop()
{
  // Your main program logic goes here
}
