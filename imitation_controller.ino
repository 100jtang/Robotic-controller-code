// These define's must be placed at the beginning before #include "Portenta_H7_TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _TIMERINTERRUPT_LOGLEVEL_ 4

//#include <Portenta_H7_TimerInterrupt.h>
//#include <Portenta_H7_ISR_Timer.h>
//#include <Portenta_H7_ISR_Timer.hpp>

// ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0
// ping 192.168.1.107
// run powershell as admin: usbipd attach -b 2-2 --auto-attach --wsl



// In Portenta_H7, avoid doing something fancy in ISR, for example Serial.print
// Or you can get this run-time error / crash

//#include <Arduino.h>
#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <sensor_msgs/msg/joint_state.h>
#include <std_msgs/msg/float64_multi_array.h>

#include <stdio.h>

#define ARRAY_LEN 200
#define JOINT_DOUBLE_LEN 40


#define TIMER0_INTERVAL_MS 1000
#define TIMER0_DURATION_MS 10000

#include <math.h>

// --------------------
// PIN initialization
// --------------------
#define button_dockneedle 2
#define button_dockcomplete 3


// ----------------------
// Error handling macros
// ----------------------

#define RCCHECK(fn) \
  { \
    rcl_ret_t temp_rc = fn; \
    if ((temp_rc != RCL_RET_OK)) { error_loop(); } \
  }
#define RCSOFTCHECK(fn) \
  { \
    rcl_ret_t temp_rc = fn; \
    if ((temp_rc != RCL_RET_OK)) {} \
  }

#include "SerialTransfer.h"

const float steps_per_revolution = 2048.0;
const float radians_per_step = (2 * M_PI) / steps_per_revolution;

// Declarations
sensor_msgs__msg__JointState joint_states_msg;


rcl_publisher_t publisher;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

const int timeout_ms = 1000;


static int64_t time_ms;
static int64_t time_ns;
static time_t time_seconds;
char time_str[25];


double needle_rotation_pos;     // = 0.0;
double sheath_rotation_pos;     // = 0.0;
double needle_translation_pos;  // = 0.0;
double sheath_translation_pos;  // = 0.0;
bool needle_status, sheath_status;



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

} receiver;



// Error handle loop
void error_loop() {
  while (1) {
    //Breakout.digitalWrite(led_test, !Breakout.digitalRead(led_test));
    delay(1000);
  }
}


void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);

  if (timer != NULL) {
    if (myTransferDue.available()) {

      uint16_t recSize = 0;
      recSize = myTransferDue.rxObj(receiver, recSize);

      // Convert encoder values to radians
      needle_rotation_pos = (double)receiver.counter_needle * radians_per_step;
      sheath_rotation_pos = (double)receiver.counter_sheath * radians_per_step;
      sheath_translation_pos = 0.01;
      needle_translation_pos = 0.01;

      joint_states_msg.position.data[0] = sheath_translation_pos;
      joint_states_msg.position.data[1] = sheath_rotation_pos;
      joint_states_msg.position.data[2] = needle_translation_pos;
      joint_states_msg.position.data[3] = needle_rotation_pos;

      // Update time stamp
      // Synchronize session time sync for microcontroller and RViz
      RCCHECK(rmw_uros_sync_session(timeout_ms));
      time_ms = rmw_uros_epoch_millis();
      time_ns = rmw_uros_epoch_nanos();
      joint_states_msg.header.stamp.sec = time_ns / 1000000000;
      joint_states_msg.header.stamp.nanosec = time_ns % 1000000000;

      // RCSOFTCHECK(rcl_publish(&publisher, &joint_states_msg, NULL));
      // Check if any of the joint values are non-zero
      // if (needle_rotation_pos != 0.0 && sheath_rotation_pos != 0.0 &&
      //     needle_translation_pos != 0.0 && sheath_translation_pos != 0.0) {
      RCSOFTCHECK(rcl_publish(&publisher, &joint_states_msg, NULL));
    }
  }
}



// void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
// {

//   RCLC_UNUSED(last_call_time);

//   if (timer != NULL) {

//     if (myTransferDue.available()) {
//       //GIGA Receiver code from encoder
//       myTransferDue.rxObj(receiver);

//       // Debug print to USB Serial
//       // Serial.print("Received guidewire: ");
//       // Serial.println(receiver.guidewire);
//       // delay(2500);

//       needle_rotation_pos = receiver.counter_needle;
//       sheath_rotation_pos = receiver.counter_sheath;


//       // Serial.println(receiver.counter_needle);
//       // Serial.print("Sheath");
//       // Serial.println(receiver.counter_sheath);
//       joint_states_msg.position.data[0] = (double)sheath_translation_pos;
//       joint_states_msg.position.data[1] = (double)sheath_rotation_pos;
//       joint_states_msg.position.data[2] = (double)needle_translation_pos;
//       joint_states_msg.position.data[3] = (double)needle_rotation_pos;

//       // joint_states_msg.position.data[0] = (double)10.0;
//       // joint_states_msg.position.data[1] = (double)10.0;
//       // joint_states_msg.position.data[2] = (double)10.0;
//       // joint_states_msg.position.data[3] = (double)10.0;

//       // SET HEADER DATA
//       // Synchronize session time sync for microcontroller and RViz
//       RCCHECK(rmw_uros_sync_session(timeout_ms));
//       time_ms = rmw_uros_epoch_millis();
//       time_ns = rmw_uros_epoch_nanos();
//       joint_states_msg.header.stamp.sec = time_ns / 1000000000;
//       joint_states_msg.header.stamp.nanosec = time_ns % 1000000000;

//       RCSOFTCHECK(rcl_publish(&publisher, &joint_states_msg, NULL));

//       // Add more debugging prints as necessary
//     }




//   }

// }

void setup() {
  // Configure serial transport
  set_microros_transports();
  Serial.begin(250000);   // 921600
  Serial2.begin(250000);  // Communication with Due

  myTransferDue.begin(Serial2);

  //set_microros_serial_transports(Serial);
  delay(2000);

  // pinMode(button_dockneedle, INPUT_PULLUP);
  // pinMode(button_dockcomplete, INPUT_PULLUP);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, "imitation_controller_node", "", &support));

  // create joint_state publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
    "/joint_states"));

  // create timer,
  const unsigned int timer_timeout = 0.3;  //0.4; //0.05; //1000
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor: create an executor with two handles instead of one in order to have one subscriber and one publisher.
  RCCHECK(rclc_executor_init(&executor, &support.context, 4, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  char joint_states_msg_buffer[ARRAY_LEN];
  joint_states_msg.header.frame_id.data = joint_states_msg_buffer;
  joint_states_msg.header.frame_id.size = 0;      //20
  joint_states_msg.header.frame_id.capacity = 0;  //ARRAY_LEN;

  rosidl_runtime_c__String string_buffer[JOINT_DOUBLE_LEN];
  joint_states_msg.name.data = string_buffer;
  joint_states_msg.name.size = 4;
  joint_states_msg.name.capacity = JOINT_DOUBLE_LEN;


  for (int i = 0; i < JOINT_DOUBLE_LEN; i++) {
    joint_states_msg.name.data[i].data = (char *)malloc(ARRAY_LEN);
    //joint_states_msg.name.data[0].data = "joint1";
    joint_states_msg.name.data[i].size = 4;
    joint_states_msg.name.data[i].capacity = ARRAY_LEN;
  }

  double joint_states_position_buffer[JOINT_DOUBLE_LEN];
  joint_states_msg.position.data = joint_states_position_buffer;
  joint_states_msg.position.size = 4;
  joint_states_msg.position.capacity = JOINT_DOUBLE_LEN;

  double joint_states_velocity_buffer[JOINT_DOUBLE_LEN];
  joint_states_msg.velocity.data = joint_states_velocity_buffer;
  joint_states_msg.velocity.size = 0;
  joint_states_msg.velocity.capacity = JOINT_DOUBLE_LEN;

  double joint_states_effort_buffer[JOINT_DOUBLE_LEN];
  joint_states_msg.effort.data = joint_states_effort_buffer;
  joint_states_msg.effort.size = 0;
  joint_states_msg.effort.capacity = JOINT_DOUBLE_LEN;

  // SET JOINT NAMES
  struct rosidl_runtime_c__String joint_1 = { "joint1" };
  struct rosidl_runtime_c__String joint_2 = { "joint2" };
  struct rosidl_runtime_c__String joint_3 = { "joint3" };
  struct rosidl_runtime_c__String joint_4 = { "joint4" };

  struct rosidl_runtime_c__String frame_id = { "controller_id" };
  joint_states_msg.header.frame_id = frame_id;

  joint_states_msg.name.data[0] = joint_1;
  joint_states_msg.name.data[1] = joint_2;
  joint_states_msg.name.data[2] = joint_3;
  joint_states_msg.name.data[3] = joint_4;

  // INITIALISE JOINT_STATES_MSG AFTER HOMING MOTORS
  joint_states_msg.position.data[0] = (double)0.0;  //sheath_translation_pos;
  joint_states_msg.position.data[1] = (double)0.0;  //sheath_rotation_pos;
  joint_states_msg.position.data[2] = (double)0.0;  //needle_translation_pos;
  joint_states_msg.position.data[3] = (double)0.0;  //needle_rotation_pos;


  // Spin executor to recieve and publish messages
  rclc_executor_spin(&executor);
}

void loop() {

  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));  //RCL_MS_TO_NS(1)
}
