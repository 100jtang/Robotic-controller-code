#include <SimpleFOC.h>

BLDCMotor motor = BLDCMotor(11); // Number of pole pairs for your motor
BLDCDriver3PWM driver = BLDCDriver3PWM(6, 10, 3, 7); // PWM pins and Enable pin
Encoder encoder = Encoder(A2, 2, 500, A0); // Encoder setup

void setup() {
  Serial.begin(115200);
  
  // Initialize the encoder
  encoder.init();
  encoder.enableInterrupts(); // Assuming default interrupt handlers are suitable

  // Initialize the driver
  driver.voltage_power_supply = 13; // Match with your power supply voltage
  driver.init();
  motor.linkDriver(&driver);
  motor.linkSensor(&encoder);

  // Configure the motor for velocity control
  motor.controller = MotionControlType::velocity;

  // PID tuning for stable and responsive control at higher speeds
  motor.PID_velocity.P = 0.2; // May need further adjustment for stability at higher speeds
  motor.PID_velocity.I = 20; // Integral term for error correction over time
  motor.PID_velocity.D = 0.01; // Derivative term to dampen oscillations

  // Set motor limits to allow for faster rotation
  motor.velocity_limit = 300; // Increase maximum velocity limit
  motor.voltage_limit = 13; // Ensure this is safe for your motor and power supply

  // Initialize motor and FOC
  motor.init();
  motor.initFOC();

  Serial.println("Setup complete. Expect faster and more intense rotation.");
}

void loop() {
  // Execute control loop and set a higher target velocity for faster rotation
  motor.loopFOC();
  motor.move(300); // Set higher target velocity for more intensity
}













