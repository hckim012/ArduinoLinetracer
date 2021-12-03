#define TRIGGER_PIN A1
#define ECHO_PIN A0

void init_ultrasonic(){
    pinMode(TRIGGER_PIN,OUTPUT);
    pinMode(ECHO_PIN,INPUT);
}
bool is_ultrasonic(){
    // return true if cone is ahead.
    long duration, cm;
    digitalWrite(TRIGGER_PIN,HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN,LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
    cm = duration/29/2;
    Serial.println(cm);
    
    if (cm<=15) {
      return true;
    } else {
      return false;
    }
}
void setup(){
    Serial.begin(9600);
    init_ultrasonic();
}
void loop(){
    is_ultrasonic();
    delay(100);
}
