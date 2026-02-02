#include "SerialTransfer.h"
#include <Arduino.h>
#include <EncButton.h>
#include <SimpleTimer.h>


SerialTransfer myTransferGIGA;
SerialTransfer myTransferTeensy;


EncButton eb1(30, 31);
EncButton eb2(24, 25);

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
  unsigned long timestamp;

} sender;

STRUCT receiver;

// char arr[] = "hello";

void isr1() {
  eb1.tickISR();
}

void isr2() {
  eb2.tickISR();
}


void setup() {
  Serial.begin(115200);
  Serial1.begin(250000);
  Serial2.begin(250000);

  myTransferGIGA.begin(Serial1);
  myTransferTeensy.begin(Serial2);


  eb1.setEncReverse(0);
  eb1.setEncType(EB_STEP4_HIGH);
  eb1.setFastTimeout(10);

  // сбросить счётчик энкодера
  eb1.counter = 0;

  eb2.setEncReverse(0);
  eb2.setEncType(EB_STEP4_HIGH);
  eb2.setFastTimeout(10);

  // сбросить счётчик энкодера
  eb2.counter = 0;

  attachInterrupt(digitalPinToInterrupt(30), isr1, CHANGE);  // Assuming pins 30 & 31 for eb1
  attachInterrupt(digitalPinToInterrupt(31), isr1, CHANGE);

  attachInterrupt(digitalPinToInterrupt(24), isr2, CHANGE);  // Assuming pins 24 & 25 for eb2
  attachInterrupt(digitalPinToInterrupt(25), isr2, CHANGE);

  eb1.setEncISR(true);
  eb2.setEncISR(true);
}


void loop() {
  bool dataChanged = false;

  if (myTransferGIGA.available()) {
    uint16_t recSize = 0;

    // DUE receiver code from display
    recSize = myTransferGIGA.rxObj(receiver, recSize);

    Serial.print("GW: ");
    Serial.print(receiver.guidewire);
    Serial.print("| Needle: ");
    Serial.print(receiver.needle);
    Serial.print("| Sheath: ");
    Serial.println(receiver.sheath);
  }

  if (myTransferTeensy.available()) {
  }

  // send data to teensy
  uint16_t TeensySize = myTransferTeensy.txObj(receiver);  // Correctly calculate send size here
  myTransferTeensy.sendData(TeensySize);

  eb1.tick();
  eb2.tick();

  sender.dir_needle = eb1.dir();
  Serial.println(sender.dir_needle);
  sender.counter_needle = eb1.counter;
  Serial.println(sender.counter_needle);

  sender.dir_sheath = eb2.dir();
  Serial.println(sender.dir_sheath);
  sender.counter_sheath = eb2.counter;
  Serial.println(sender.counter_sheath);

  uint16_t myPos = myTransferTeensy.txObj(sender);
  myTransferTeensy.sendData(myPos);

  // use this variable to keep track of how many
  // bytes we're stuffing in the transmit buffer

  sender.guidewire = random(2);
  sender.needle = random(4);
  sender.sheath = random(3);
  sender.RobotConnected = random(6);
  sender.SimulationConnected = random(4);
  sender.toolSwitch = random(3);

  // uint16_t sendSize = 0;

  // ///////////////////////////////////////// Stuff buffer with struct
  // sendSize = myTransfer.txObj(sender, sendSize);

  // ///////////////////////////////////////// Stuff buffer with array
  // // sendSize = myTransfer.txObj(arr, sendSize);

  // ///////////////////////////////////////// Send buffer
  // myTransfer.sendData(sendSize);
  // delay(2500);
  uint16_t sendSize = 0;
  sender.timestamp = millis();
  sendSize = myTransferGIGA.txObj(sender, sendSize);
  myTransferGIGA.sendData(sendSize);

}
