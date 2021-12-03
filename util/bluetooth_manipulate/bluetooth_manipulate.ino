#include <SoftwareSerial.h>

#define ENA 6
#define EN1 7
#define EN2 3
#define ENB 5
#define EN3 4
#define EN4 2

SoftwareSerial bt_serial(9,8);

int speed = 120;
int tspeed = 250;

void setup() {
  // put your setup code here, to run once:
    bt_serial.begin(9600);
    pinMode(ENA,OUTPUT);
    pinMode(ENB,OUTPUT);
    pinMode(EN3,OUTPUT);
    pinMode(EN4,OUTPUT);
    pinMode(EN1,OUTPUT);
    pinMode(EN2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (bt_serial.available() > 0){
    byte data = (byte) bt_serial.read();
    if (data == 'e'){
      bt_serial.println("EMERGENCY");
      analogWrite(ENA,0);
      analogWrite(ENB,0);
      delay(3000);
    } else if (data == 'f'){
      bt_serial.println("FORWARD");
      digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
    delay(1000);
    analogWrite(ENA,0);
      analogWrite(ENB,0);
      } else if (data == 'l'){
      bt_serial.println("LEFT");
      digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    analogWrite(ENA, tspeed);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    analogWrite(ENB, tspeed);
    delay(1000);
    analogWrite(ENA,0);
      analogWrite(ENB,0);
      } else if (data == 'r'){
      bt_serial.println("RIGHT");
      digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    analogWrite(ENA, tspeed);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENB, tspeed);
    delay(1000);
    analogWrite(ENA,0);
      analogWrite(ENB,0);
      }
  }
}
