void init_light(){
    pinMode(LIGHT, INPUT);
}
bool is_dark(){
    return analogRead(LIGHT) > LIGHT_THRESHOLD;
}
