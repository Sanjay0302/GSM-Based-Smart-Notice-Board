If you need to track the device's on-time, you can utilize the `millis()` function in Arduino to measure the elapsed time since the device was powered on. The `millis()` function returns the number of milliseconds that have passed since the device started running.

Here's an example code snippet that demonstrates how to use `millis()` to track the on-time of the device:

```cpp
unsigned long startTime; // Variable to store the start time

void setup() {
  Serial.begin(115200); // Initialize the serial communication
  startTime = millis(); // Record the start time
}

void loop() {
  unsigned long elapsedTime = millis() - startTime; // Calculate the elapsed time

  // Print the on-time in different units (seconds, minutes, hours)
  Serial.print("On-time: ");
  Serial.print(elapsedTime / 1000); // Convert milliseconds to seconds
  Serial.print(" seconds (");
  Serial.print(elapsedTime / (1000 * 60)); // Convert milliseconds to minutes
  Serial.print(" minutes or ");
  Serial.print(elapsedTime / (1000 * 60 * 60)); // Convert milliseconds to hours
  Serial.println(" hours)");

  delay(1000); // Delay for 1 second before printing the on-time again
}
```

In the above code, the `startTime` variable is initialized with the value of `millis()` in the `setup()` function. Then, in the `loop()` function, the elapsed time is calculated by subtracting `startTime` from the current value of `millis()`. The elapsed time is then printed in seconds, minutes, and hours using the Serial communication. The `delay(1000)` function is used to introduce a 1-second delay between each printout.

By running this code on your ESP32 device, you can monitor the on-time of the device through the serial monitor in your Arduino IDE or a serial communication program.

------------------------------------------------------------------

If you want to display the time on an ESP32 device only when the minute value changes, you can modify the code to keep track of the previous minute value and compare it with the current minute value. Here's an example:

```cpp
unsigned long startTime; // Variable to store the start time
int previousMinute = -1; // Variable to store the previous minute value

void setup() {
  Serial.begin(115200); // Initialize the serial communication
  startTime = millis(); // Record the start time
}

void loop() {
  unsigned long elapsedTime = millis() - startTime; // Calculate the elapsed time

  // Convert milliseconds to minutes
  int minutes = elapsedTime / (1000 * 60);

  // Check if the minute value has changed
  if (minutes != previousMinute) {
    previousMinute = minutes; // Update the previous minute value

    // Print the current time
    Serial.print("Time: ");
    Serial.print(minutes / 60); // Print hours
    Serial.print(":");
    if (minutes % 60 < 10) {
      Serial.print("0"); // Print leading zero if minute value is less than 10
    }
    Serial.print(minutes % 60); // Print minutes
    Serial.println();
  }

  // Other code and operations here...

  delay(1000); // Delay for 1 second before checking the time again
}
```

In this code, the variable `previousMinute` is initially set to -1 to ensure that the first time check will always be true. Inside the `loop()` function, the `minutes` variable is calculated by dividing the elapsed time by the number of milliseconds in a minute. Then, it compares the current minute value with the `previousMinute` value. If they are different, it means that the minute has changed, and the current time is printed. The `previousMinute` is then updated to the current minute value.

You can modify the code according to your requirements and add additional functionality or conditions as needed within the loop.
