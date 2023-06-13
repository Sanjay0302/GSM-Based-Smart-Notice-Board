#include <HardwareSerial.h>

HardwareSerial mySerial(2);

int previousBatteryLevel = -1;
int batteryLevel;
int batteryVoltage;

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
    // Serial.write(response); // Print the response to the Serial Monitor

    // Check if the response contains "+CBC:"
    if (response == '+') {
      if (mySerial.find("CBC:")) {
        // Extract battery level and voltage values
        mySerial.find(',');
        batteryLevel = mySerial.parseInt();
        mySerial.find(',');
        batteryVoltage = mySerial.parseInt();

        // Print only when battery percentage changes
        if (batteryLevel != previousBatteryLevel) {
          Serial.print("Battery Level: ");
          Serial.print(batteryLevel);
          Serial.print("%, Voltage: ");
          Serial.print(batteryVoltage);
          Serial.println(" mV");
          previousBatteryLevel = batteryLevel;
          
        }
      }
    }
  }

  if (Serial.available()) {
    char command = Serial.read();
    mySerial.write(command); // Send data from Serial Monitor to the SIM800L module
  }
}
