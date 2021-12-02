void init_light(){
    pinMode(LIGHT, INPUT);
}
bool is_dark(){
    // return true if light sensor is dark.
    return analogRead(LIGHT) > LIGHT_THRESHOLD;
}


void init_ultrasonic(){
    
}
bool is_ultrasonic(){
    // return true if cone is ahead.
    return false;
}

void init_bluetooth(){
  
}
bool is_bluetooth(){
    // return true if signal received.
    return false;
}
void send_bluetooth(){
    //send "EMERGENCY" to phone
}


void init_rfid(){
  
}
int read_rfid(){
    // wait until rfid tagged then read it.
    // return 0 or 1 or 2
}

void init_sensors(){
    init_light();
    init_ultrasonic();
    init_bluetooth();
    init_rfid();
}
