#include <SimpleFOC.h>

BLDCMotor motor = BLDCMotor(14);
BLDCDriver3PWM driver = BLDCDriver3PWM(6, 10, 3, 7);
int potPin = A5;

void setup() {
  Serial.begin(115200);

  pinMode(potPin, INPUT);

  driver.init();
  driver.voltage_power_supply = 13;

  motor.linkDriver(&driver);

  // Open-loop control setup
  motor.controller = MotionControlType::angle_openloop;


  motor.voltage_limit = 5;  // Safe starting voltage, adjust based on your setup

  motor.init();
  // No need to call motor.initFOC() for open-loop control

  Serial.println("Motor setup complete. Use potentiometer to control speed.");
}

void loop() {
  int potValue = analogRead(potPin);
  float angleIncrement = map(potValue, 0, 1023, 0, 10);
  // Continuously increase the motor angle at a rate that results in constant rotation
  motor.shaft_angle += angleIncrement;

  motor.move(motor.shaft_angle);

  delay(10);  // Adjust delay for rotation speed - smaller delay for faster rotation
}
