  #include <HardwareSerial.h>

  HardwareSerial mySerial(2);
  String receivedMessage = "";
  String storedMessage = "";
  String extractedHeader = "";
  String extractedMessage = "";

  void setup() {
    Serial.begin(115200);
    while (!Serial) {
      ;
    }

    Serial.println("Reading SMS from SIM800L...");

    mySerial.begin(38400);
    mySerial.println("AT+CMGF=1"); // Set SMS text mode
    delay(1000);
    mySerial.println("AT+CMGR=1"); // Read the first SMS message
  }

  void loop() {
    if (mySerial.available()) {
      char response = mySerial.read();
            
      receivedMessage += response; // Append response characters to the receivedMessage variable
      if (response == '\n') {
        // End of message reached
        if (receivedMessage.startsWith("+CMGR:")) {
          storedMessage = receivedMessage; // Store the entire received message
        }
        else if (storedMessage != "") {
          extractedMessage = receivedMessage; // Store the extracted message
        }
        receivedMessage = ""; // Reset the receivedMessage variable
      }
    }

    if (extractedMessage != "") {
      // Extract the header from the stored message
      int headerEndIndex = storedMessage.indexOf('\n');
      extractedHeader = storedMessage.substring(0, headerEndIndex);

      Serial.println("Header is : " + extractedHeader); // Print the extracted header
      Serial.println("msg is : " + extractedMessage); // Print the extracted message
      sortedVariable(extractedHeader);
      extractedHeader = ""; // Reset the extractedHeader variable
      extractedMessage = ""; // Reset the extractedMessage variable
      storedMessage = ""; // Reset the storedMessage variable
    }

    if (Serial.available()) {
      char command = Serial.read();
      mySerial.write(command); // Send data from Serial Monitor to the SIM800L module
    }
  }

  void sortedVariable(const String& header) {
    // Extract phone number
    int firstCommaIndex = header.indexOf(',');
    int secondCommaIndex = header.indexOf(',', firstCommaIndex + 1);
    String phoneNumber = header.substring(firstCommaIndex + 3, secondCommaIndex - 1);

    // Extract date and time
    int dateTimeStartIndex = header.lastIndexOf(",\"") + 2;
    int dateTimeEndIndex = header.lastIndexOf('"');
    String dateTime = header.substring(dateTimeStartIndex, dateTimeEndIndex);

    // Split date and time
    int commaIndex = dateTime.indexOf(',');
    String date = dateTime.substring(0, commaIndex);
    String time = dateTime.substring(commaIndex + 1);

    Serial.println("Phone Number: +" + phoneNumber);
    Serial.println("Date: " + date);
    Serial.println("Time: " + time);
  }

