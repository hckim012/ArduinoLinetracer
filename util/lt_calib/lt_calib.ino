#include <TimerOne.h>

// directions
#define FORWARD 1
#define BACKWARD 2
#define TURN_RIGHT 3
#define TURN_LEFT 4
#define STOP 5

// line tracer pins
#define LT_MODULE_L A5
#define LT_MODULE_F A0
#define LT_MODULE_R A4

// motor pins
#define ENA 6
#define IN1 7
#define IN2 3
#define IN3 4
#define IN4 2
#define ENB 5

// time delay const
#define TICK 20
#define TOCK 10

// power of motor
int speed = 120;
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

void car_update(int direction){
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
            analogWrite(ENA, speed);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
            analogWrite(ENB, speed);
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

float readSensor(int pin, bool verbose){
    int min = 1000;
    int max = 0;
    long avg = 0;
    for(int i=0;i<400;i++){
        int ret = analogRead(pin);
        avg += ret;
        if(ret>max) max = ret;
        if(ret<min) min = ret;
        delay(5);
    }
    if(verbose){
        Serial.print("min : ");
        Serial.print(min);
        Serial.print(", max : ");
        Serial.print(max);
        Serial.print(", mean : ");
        Serial.println(avg/400.0);
    }
    return avg/400.0;
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
    Serial.println("Sensor calibration start!");
    Serial.println("left module");
    int left = readSensor(LT_MODULE_L, true);
//    Serial.println("front module");
//    int forward = readSensor(LT_MODULE_F, true);
    Serial.println("right module");
    int right = readSensor(LT_MODULE_R, true);
    
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
    white_threshold = forward + 10;
    black_threshold = forward * 2;
    Serial.print("white_threshold");
    Serial.println(white_threshold);
    Serial.print("black_threshold");
    Serial.println(black_threshold);
    Timer1.initialize(10000);
    Timer1.attachInterrupt(sense);

}

void loop(){
    // Serial.print(readSensor(LT_MODULE_L, false));
    // Serial.print(", ");
    // Serial.print(readSensor(LT_MODULE_F, false));
    // Serial.print(", ");
    // Serial.println(readSensor(LT_MODULE_R, false));
        while(forward<black_threshold || left<black_threshold || right<black_threshold){
          car_update(FORWARD);
          delay(TICK);
          car_update(STOP);
          delay(TOCK);
        }
        while(forward>white_threshold){
          car_update(FORWARD);
          delay(TICK);
          car_update(STOP);
          delay(TOCK);
        }
        while(forward<black_threshold){
          car_update(BACKWARD);
          delay(TICK);
          car_update(STOP);
          delay(TOCK);
        }
        while(forward>white_threshold){
          car_update(BACKWARD);
          delay(TICK);
          car_update(STOP);
          delay(TOCK);
        }
//        for(int i=0;i<90;i+=10){
//            delay(1000);
//            Serial.println(i);
//            analogWrite(ENA, i);
//            analogWrite(ENB, i);
//        }

    
}
