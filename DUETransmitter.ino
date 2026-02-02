#include "SerialTransfer.h"
#include <Arduino.h>
#include <EncButton.h>


// SoftwareSerial serialEsp(4,5); // 4-> D2 (RX) 5-> D1 (TX)

SerialTransfer myTransfer;
EncButton eb(2, 3);

struct STRUCT {
  bool guidewire;
  bool needle;
  bool sheath;

  bool RobotConnected;
  bool SimulationConnected;

  bool toolSwitch;

  int dir; // Direction of rotation (-1, 0, 1)
  bool fast; // Was the rotation fast?
  bool pressing; // Was the encoder pressed?
  int counter; // Encoder counter value
  
} sender;

STRUCT receiver;

// char arr[] = "hello";

void isr() {
  eb.tickISR();
}


void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);

}


void loop()
{
  if(myTransfer.available()){
    uint16_t recSize = 0;

    // DUE receiver code from display
    recSize = myTransfer.rxObj(receiver, recSize);
    Serial.print("GW: ");
    Serial.print(receiver.guidewire);
    Serial.print("| Needle: ");
    Serial.print(receiver.needle);
    Serial.print("| Sheath: ");
    Serial.println(receiver.sheath);
  }
  
  eb.tick();

  // Check for encoder turn and update sender struct
  if (eb.turn()) {
    // Update the STRUCT with encoder data
    sender.dir = eb.dir();
    sender.fast = eb.fast();
    sender.pressing = eb.pressing();
    sender.counter = eb.counter;

    // Then transmit the updated struct as before
    uint16_t sendSize = 0;
    sendSize = myTransfer.txObj(sender, sendSize);
    myTransfer.sendData(sendSize);
  }

  // use this variable to keep track of how many
  // bytes we're stuffing in the transmit buffer

  sender.guidewire = random(2);
  sender.needle = random(4);
  sender.sheath = random(3);
  sender.RobotConnected = random(6);
  sender.SimulationConnected = random(4);
  sender.toolSwitch = random(3);
  
  uint16_t sendSize = 0;

  ///////////////////////////////////////// Stuff buffer with struct
  sendSize = myTransfer.txObj(sender, sendSize);

  ///////////////////////////////////////// Stuff buffer with array
  // sendSize = myTransfer.txObj(arr, sendSize);

  ///////////////////////////////////////// Send buffer
  myTransfer.sendData(sendSize);
  // delay(2500);
}
