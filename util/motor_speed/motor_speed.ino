#include <TimerOne.h>

// directions
#define FORWARD 1
#define BACKWARD 2
#define TURN_RIGHT 3
#define TURN_LEFT 4
#define STOP 5

// line tracer pins
#define LT_MODULE_L A1
#define LT_MODULE_F A0
#define LT_MODULE_R A2

// motor pins
#define ENA 6
#define IN1 7
#define IN2 3
#define IN3 4
#define IN4 2
#define ENB 5

// time delay const
#define TICK 44
#define TOCK 10

// power of motor
int speed = 110;
int backward_speed = 110;
int curve_speed = 200;

int black_threshold, white_threshold;

void init_line_tracer_modules(){
    pinMode(LT_MODULE_L, INPUT);
    pinMode(LT_MODULE_F, INPUT);
    pinMode(LT_MODULE_R, INPUT);
}
volatile int left, forward, right;
void sense(){
  left = analogRead(LT_MODULE_L);
  forward = analogRead(LT_MODULE_F);
  right = analogRead(LT_MODULE_R);
  //Serial.println(forward);
}

void init_motor(){
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);
}

void roll(int direction){
    if(digitalRead(8)==LOW) direction = 0;
    switch(direction){
        case FORWARD:
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            analogWrite(ENA, speed);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
            analogWrite(ENB, speed);
            break;
        case BACKWARD:
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            analogWrite(ENA, backward_speed);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
            analogWrite(ENB, backward_speed);
            break;
        case TURN_RIGHT:
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            analogWrite(ENA, curve_speed);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
            analogWrite(ENB, curve_speed);
            break;
        case TURN_LEFT:
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            analogWrite(ENA, curve_speed);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
            analogWrite(ENB, curve_speed);
            break;
        case STOP:
        default:
            analogWrite(ENA, 0);
            analogWrite(ENB, 0);
            break;
    }
}

void car_update(int direction, int num_ticks){
    for(int i=0;i<num_ticks;i++){
        roll(direction);
        delay(TICK);
        roll(STOP);
        delay(TOCK);
    }
}

float readSensor(int pin, bool verbose){
    int min = 1000;
    int max = 0;
    int avg = 0;
    for(int i=0;i<10;i++){
        int ret = analogRead(pin);
        avg += ret;
        if(ret>max) max = ret;
        if(ret<min) min = ret;
        delay(200);
    }
    if(verbose){
        Serial.print("min : ");
        Serial.print(min);
        Serial.print(", max : ");
        Serial.print(max);
        Serial.print(", mean : ");
        Serial.println(avg/10.0);
    }
    return avg/10.0;
}
/*
시계방향 - 감소
반시계방향 - 증가
흰 바닥에서 27~30사이,
검은 선에서 35이상
*/
void setup(){
    Serial.begin(9600);
    init_motor();
    init_line_tracer_modules();
    pinMode(8, INPUT);
    Serial.println(" ");
    Serial.println("Motor speed test start!");
    
    car_update(FORWARD, 30);
    // delay(1000);
    
    car_update(BACKWARD, 30);
    // delay(1000);
    car_update(STOP, 100);
    Serial.println("Test end");
    
//    Serial.println("Motor turn test start!");
//    for(int i=0;i<1;i++){
//        car_update(TURN_LEFT, 1);
//        car_update(TURN_RIGHT, 2);
//        car_update(TURN_LEFT, 1);
//        car_update(STOP, 1);
//      
//    }
//    Serial.println("Test end");

}

void loop(){

}
