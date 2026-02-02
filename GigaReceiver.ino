#include "SerialTransfer.h"



SerialTransfer myTransfer;

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


void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);
}


void loop()
{
  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    // DUE receiver code from display
    recSize = myTransfer.rxObj(receiver, recSize);
    Serial.print("GW: ");
    Serial.print(receiver.guidewire);
    Serial.print("| Needle: ");
    Serial.print(receiver.needle);
    Serial.print("| Sheath: ");
    Serial.println(receiver.sheath);
    // Serial.print(sender.RobotConnected);
    // Serial.print(sender.SimulationConnected);
    // Serial.print(sender.toolSwitch);
    // Serial.print(" | ");

    //GIGA Receiver code from encoder
    Serial.print("Encoder - Dir: ");
    Serial.print(receiver.dir);
    Serial.print(", Fast: ");
    Serial.print(receiver.fast);
    Serial.print(", Pressing: ");
    Serial.print(receiver.pressing);
    Serial.print(", Counter: ");
    Serial.println(receiver.counter);
  }
}
