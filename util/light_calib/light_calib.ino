#define LIGHT A2
#define LIGHT_THRESHOLD 300
void setup(){
    Serial.begin(9600);
    pinMode(LIGHT, INPUT);
}
bool is_dark(){
    int light_value = analogRead(LIGHT);
    Serial.println(light_value);
    if(light_value > LIGHT_THRESHOLD) return true;
    return false;
}
void loop(){
    bool dark = is_dark();
//    Serial.println(dark);
    delay(500);
}
