#include <SoftwareSerial.h>

#ifndef PIN_LAYOUT
#define PIN_LAYOUT
// line tracer pins
#define LT_MODULE_L A5
#define LT_MODULE_R A4
#define LT_MODULE_F A2

// motor pins
#define ENA 6
#define IN1 7
#define IN2 3
#define IN3 4
#define IN4 2
#define ENB 5

//switch pin
#define SW 8

#define LIGHT A3

#endif

#define MIN_VOLTAGE (9.7)
#define INF 1000

// time delay const
// forward 1cm in 1 tick(depends on car speed)
#define TICK 30
#define TOCK 10
#define FOREVER 10000

// strategy
#define LEFT_FIRST 0
#define RIGHT_FIRST 1

//mission flags
#define NOTHING 0
#define DARK 1
#define PARKING 2
#define EMERGENCY 3

// directions
#define FORWARD 1
#define BACKWARD 2
#define TURN_RIGHT 3
#define TURN_LEFT 4
#define STOP 5

// power range of motor
#define MAX_WHEEL_SPEED 220
#define MIN_WHEEL_SPEED 80

// power of motor
int speed = 100;
const int backward_speed = 100;
const int curve_speed = 200;


// lt sensor values
#define SENSOR_L 0b010
#define SENSOR_F 0b100
#define SENSOR_R 0b001

#define LT_THRESHOLD_L 50
#define LT_THRESHOLD_F (33+100)
#define LT_THRESHOLD_R 50

volatile int l_cnt, f_cnt, r_cnt; // ms passed from last detection

#define LIGHT_THRESHOLD 500

#define SLOW false
#define DEBUG false
