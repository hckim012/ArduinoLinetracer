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

int determine_junction_type(){
    // only considering good cases...
    int junction_type=0b000;
    if(lt_state&SENSOR_L) junction_type|= SENSOR_L;
    if(lt_state&SENSOR_R) junction_type|= SENSOR_R;

    car_update(FORWARD, 4);
    
    if(l_cnt<4*TICK)  junction_type|= SENSOR_L;
    if(r_cnt<4*TICK)  junction_type|= SENSOR_R;

    car_update(FORWARD, 6);
    car_update(STOP, 10);
    
    int line_pos = find_line(5, SENSOR_F, false);
    if(line_pos!=INF){
        junction_type |= SENSOR_F;
    }
//    car_update(BACKWARD, 20);
    car_update(FORWARD, 10);
    return junction_type;
}

void travel(){
    // high level code for moving around
    lt_state = lt_sense();
    if(lt_state==0b100||lt_state==0b010||lt_state==0b001||lt_state==0b000){
        linetrace_step();
        return;
    }
    //determine junction type
    Serial.println("Determine junction type..");
    car_update(STOP, 20);
    int junction_type = determine_junction_type();
    Serial.print("Junction type: ");
    Serial.println(junction_type, BIN);
    car_update(STOP,20);

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
    car_update(STOP,20);
}
int find_line(int max_turn_steps, int target_sensor, bool net_turn){
    // default max_turn_steps is 5
    // ideally no displacement nor rotation
    // simulates the behavior with many sensors
    // return INF if no line is found, or number of steps to line(+ is CLOCKWISE)
    int rot_steps = 0;
    int line_pos = INF;
    bool found_line= false;
    //sway right then left then right
    for(int i=0;i<max_turn_steps*TICK;i+=10){
        lt_state = lt_sense();
        if(lt_state & target_sensor){
          if(!found_line) line_pos = rot_steps;
          if(net_turn) return line_pos;
          found_line=true;
        }
        car_update(TURN_RIGHT, 0);
        delay(10);
        rot_steps+= 1;
    }
    if(max_turn_steps > 5)    car_update(STOP, 10);
    for(int i=0;i<2*max_turn_steps*TICK;i+=10){
        lt_state = lt_sense();
        if(lt_state & target_sensor){
          if(!found_line) line_pos = rot_steps;
          if(net_turn) return line_pos;
          found_line=true;
        }
        car_update(TURN_LEFT, 0);
        delay(10);
        rot_steps-= 1;
    }
    if(max_turn_steps > 5)    car_update(STOP, 10);
    for(int i=0;i<max_turn_steps*TICK;i+=10){
        lt_state = lt_sense();
        if(lt_state & target_sensor){
          if(!found_line) line_pos = rot_steps;
          if(net_turn) return line_pos;
          found_line=true;
        }
        car_update(TURN_RIGHT, 0);
        delay(10);
        rot_steps+= 1;
    }
    car_update(STOP, 1);
    return line_pos;
}
void turn_left(){
    car_update(TURN_LEFT, 3); // Force turn
    car_update(TURN_LEFT, 0);
    while(1){
      lt_state = lt_sense();
      if(lt_state & SENSOR_L) break;
      delay(10);
    }
    car_update(STOP,10);
    start_new_linetrace();
}
void turn_forward(){
    car_update(FORWARD, 10);
    start_new_linetrace();
}
void turn_right(){
    car_update(TURN_RIGHT, 3); // Force turn
    car_update(TURN_RIGHT, 0);
    while(1){
      lt_state = lt_sense();
      if(lt_state & SENSOR_R) break;
      delay(10);
    }
    car_update(STOP,10);
    start_new_linetrace();
}

float on_line = 0.0;

void start_new_linetrace(){
    // reset control variables used by linetrace_steps
    // first trying a naive control
    on_line = 0.0;
    linetrace_step();
}

void linetrace_step(){
    // Line tracing step code
    // use only forward sensor?
    lt_state = lt_sense();
    
    if(lt_state & SENSOR_F){
      #if USE_CONTROL
      car_update(FORWARD, 0);
      delay(TICK*(1));
      car_update(STOP, 0);
      #else
      car_update(FORWARD, 1);
      #endif
      return;
    }
    else if(lt_state & SENSOR_L){
      if(on_line==0)
        car_update(FORWARD, 2);
      while(true){
          car_update(TURN_LEFT, 0);
          delay(10);
          lt_state = lt_sense();
          if(lt_state & SENSOR_F) break;
      }
      car_update(STOP, 0);
    }
    else if(lt_state & SENSOR_R){
      if(on_line==0)
        car_update(FORWARD, 2);
      while(true){
          car_update(TURN_RIGHT, 0);
          delay(10);
          lt_state = lt_sense();
          if(lt_state & SENSOR_F) break;
      }
      car_update(STOP, 0);
    }
    else{
        int search_step;
        int line_pos;
        car_update(STOP, 10); //Make sure car & motor stop for accurate rotation
        for(search_step = 1;search_step<=5;search_step+=1){
            line_pos = find_line(search_step, 0b010, true);  
            if(line_pos != INF) break;  
        }
        Serial.println(line_pos);
        if(abs(line_pos)<=1){
          on_line++;
        }
        if(search_step >= 3){
          on_line = 0.0;
        }
        if(line_pos == INF){
            // try to return to line
            for(search_step = 4; search_step <= 100; search_step *= 2){
                line_pos = find_line(search_step, 0b111, true);
                if(line_pos != INF) break;
            }
        }
        if(search_step>100){
            Serial.println("LINE NOT FOUND");
            delay(FOREVER);  
        }
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
    start_new_linetrace();
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
