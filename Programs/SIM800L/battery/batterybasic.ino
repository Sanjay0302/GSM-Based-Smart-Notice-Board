#include <HardwareSerial.h>

HardwareSerial mySerial(2);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  
  mySerial.begin(38400);
  mySerial.println("AT+CBC"); // Send the AT command to retrieve battery information
}

void loop() {
  if (mySerial.available()) {
    char response = mySerial.read();
    Serial.write(response); // Print the response to the Serial Monitor
  }

  if (Serial.available()) {
    char command = Serial.read();
    mySerial.write(command); // Send data from Serial Monitor to the SIM800L module
  }
}


// #battery #indicate
