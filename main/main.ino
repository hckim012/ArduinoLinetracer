#include "include.h"
int direction, prev_direction;
int lt_state;

int mission_state;
int strategy;

void car_update(int direction, int num_ticks){
    if(digitalRead(SW)==LOW){
      prev_direction = STOP;
      roll(STOP);
      return;
    }
    if(num_ticks>0){
        // roll num_ticks times and stop
        for(int i=0;i<num_ticks;i++){
            roll(direction);
            delay(TICK);
            roll(STOP);
            delay(TOCK);
        }
    }
    else{
        // roll but not stop
        roll(direction);
    }
    prev_direction = direction;

}

unsigned long last_check_time = 0;
int check_mission(){
    //TODO
    last_check_time = millis();
//    Serial.println(last_check_time);
    return NOTHING;
}
int do_mission(int mission){
    //TODO
    // return 0 if success
    return 0;
}

void travel(){
    // high level code for moving around
    lt_state = lt_sense();
    if(lt_state==0b100||lt_state==0b010||lt_state==0b001||lt_state==0b000){
        linetrace_step();
        return;
    }
    // only considering good cases...
    //determine junction type
    Serial.println("Determine junction type..");
    int junction_type=0b000;
    if(lt_state&SENSOR_L) junction_type|= SENSOR_L;
    if(lt_state&SENSOR_R) junction_type|= SENSOR_R;

    car_update(FORWARD, 4);
    
    if(l_cnt<4*TICK)  junction_type|= SENSOR_L;
    if(r_cnt<4*TICK)  junction_type|= SENSOR_R;

    car_update(FORWARD, 6);
    car_update(STOP, 10);
    
    bool is_line_ahead = find_line();
    if(is_line_ahead){
        junction_type |= SENSOR_F;
    }
//    car_update(BACKWARD, 20);    
    Serial.print("Junction type: ");
    Serial.println(junction_type, BIN);
    car_update(STOP,20);
    
    // delay(FOREVER);
    // do junction turn
    if(strategy==LEFT_FIRST){
        if(junction_type & SENSOR_L) turn_left();
        else if(junction_type & SENSOR_F)   turn_forward();
        else if(junction_type & SENSOR_R)   turn_right();
    }
    else if(strategy==RIGHT_FIRST){
        if(junction_type & SENSOR_R) turn_right();
        else if(junction_type & SENSOR_F)   turn_forward();
        else if(junction_type & SENSOR_L)   turn_left();
    }
}
bool find_line(){
    int rot_steps = 0;
    bool found_line= false;
    //sway
    for(int i=0;i<5;i++){
        lt_state = lt_sense();
        if(lt_state & SENSOR_F) found_line=true;
        car_update(TURN_RIGHT, 1);
        rot_steps+= 1;
    }
    for(int i=0;i<10;i++){
        lt_state = lt_sense();
        if(lt_state & SENSOR_F) found_line=true;
        car_update(TURN_LEFT, 1);
        rot_steps-= 1;
    }
    for(int i=0;i<5;i++){
        lt_state = lt_sense();
        if(lt_state & SENSOR_F) found_line=true;
        car_update(TURN_RIGHT, 1);
        rot_steps+= 1;
    }
    car_update(STOP, 1);
    return found_line;
}
void turn_left(){
    car_update(TURN_LEFT, 10); // Force turn
    car_update(TURN_LEFT, 0);
    while(1){
      lt_state = lt_sense();
      if(lt_state & SENSOR_F) break;
      delay(10);
    }
    car_update(STOP,1);
    linetrace_step();
}
void turn_forward(){
    car_update(FORWARD, 10);
    linetrace_step();
}
void turn_right(){
    car_update(TURN_RIGHT, 10); // Force turn
    car_update(TURN_RIGHT, 0);
    while(1){
      lt_state = lt_sense();
      if(lt_state & SENSOR_F) break;
      delay(10);
    }
    car_update(STOP,1);
    linetrace_step();
}
void linetrace_step(){
    // Line tracing step code
    lt_state = lt_sense();

    if(lt_state & SENSOR_L){
      car_update(FORWARD, 2);
      car_update(TURN_LEFT, 1);
    }
    else if(lt_state & SENSOR_R){
      car_update(FORWARD, 2);
      car_update(TURN_RIGHT, 1);
    }
    else if(lt_state & SENSOR_F){
      car_update(FORWARD, 1);
    }
    else{
      car_update(prev_direction, 1);
    }
}

void setup(){
    Serial.begin(9600);
    init_lt_modules();
    init_motor();

    pinMode(SW, OUTPUT);
    //blink
    for(int i=0;i<3;i++){
        digitalWrite(SW, HIGH);
        delay(150);
        digitalWrite(SW,LOW);
        delay(150);
    }
    pinMode(SW, INPUT);

    direction = STOP;
    mission_state = NOTHING;
    strategy = LEFT_FIRST;
    set_lt_interrupt();
    Serial.println("Run start!");
}
void loop(){
    // First check for any mission
    while(1){
        mission_state = check_mission();
        if(mission_state!=NOTHING){
            do_mission(mission_state);
        }
        else{
            break;
        }
    }
    travel();
}
