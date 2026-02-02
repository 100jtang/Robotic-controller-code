#include "SerialTransfer.h"

SerialTransfer myTransferDue;
struct STRUCT {
  bool guidewire;
  bool needle;
  bool sheath;

  bool RobotConnected;
  bool SimulationConnected;

  bool toolSwitch;

  int dir_needle;        // Direction of rotation (-1, 0, 1)
  bool fast_needle;      // Was the rotation fast?
  bool pressing_needle;  // Was the encoder pressed?
  int counter_needle;    // Encoder counter value

  int dir_sheath;        // Direction of rotation (-1, 0, 1)
  bool fast_sheath;      // Was the rotation fast?
  bool pressing_sheath;  // Was the encoder pressed?
  int counter_sheath;    // Encoder counter value
  unsigned long timestampTeen;

} receiver;

long counter = 0;

unsigned long previousMillis = 0;  // Stores the last time the function was called
const long interval = 1000;        // Interval at which to call the function (milliseconds)

void setup() {
  Serial.begin(115200);
  Serial2.begin(250000); // Communication with Due
  myTransferDue.begin(Serial2);
}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the function was called
    previousMillis = currentMillis;

    // Call your function here
    // Serial.print("Teensy Latency: ");
    Serial.println(counter);
    counter = 0;
  

    // Add more debugging prints as necessary
  }

  if (myTransferDue.available()) {
    //GIGA Receiver code from encoder
    myTransferDue.rxObj(receiver);
    counter++;
  }
    // Debug print to USB Serial
    // Serial.print("Received guidewire: ");
    // Serial.println(receiver.guidewire);
    // delay(2500);

    // Serial.print("Needle: ");
    // Serial.println(receiver.counter_needle);
    // Serial.print("Sheath");
    // Serial.println(receiver.counter_sheath);

}

