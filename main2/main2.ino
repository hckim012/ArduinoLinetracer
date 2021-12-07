#include "include.h"
long counts[6];
long curve = 0;
int lt_state;
int cone_value;

int mission_state;
int strategy;

void set_counts(int direction, long value){
    counts[FORWARD] = 0;
    counts[BACKWARD] = 0;
    counts[TURN_RIGHT] = 0;
    counts[TURN_LEFT] = 0;
    counts[STOP] = 0;
    counts[direction] = value;
}

void car_update(int direction, int duration){
    
    roll(direction);
    delay(duration);

    // update counts
    set_counts(direction, counts[direction]+duration);
    if(direction==TURN_LEFT || direction==TURN_RIGHT){
        curve+=1;
        if(curve%100==99){
            Serial.println("force forward");
            Serial.println(curve);
            roll(FORWARD);
            delay(50);
            roll(STOP);
        }
    }
    
    if(counts[FORWARD]>200){
        #if DEBUG
        Serial.println("forward stop");
        #endif
        if(strategy==LEFT_FIRST)
          car_update(TURN_LEFT, 50);
        else
          car_update(TURN_RIGHT, 50);
        curve = 0;
    }
    else if(counts[TURN_RIGHT]>100||counts[TURN_LEFT]>100){
        #if DEBUG
        Serial.println(direction==TURN_RIGHT?"turn right stop":"turn left stop");
        #endif
        car_update(STOP, 10);
        curve = 0;
    }
    else if(counts[BACKWARD]>100){
        #if DEBUG
        Serial.println("backward stop");
        #endif
        car_update(STOP, 30);
    }
}
void move_until(int direction, int target_sensor, int target_value, int max_time){
    if(max_time<10)  return;
    int i;
    for(i=0;i<max_time/10;i++){
      car_update(direction, 10);
      lt_state = lt_sense_now();
      if((lt_state & target_sensor) == target_value) break;
    }
    #if DEBUG
    if(i==max_time/10 && max_time >= 100){
        Serial.println("target not found");
    }
    #endif
    car_update(STOP, 10);
}


long last_check_time = 0;
void update_mission(){
    long current_time = millis();
    if(current_time - last_check_time > 500){
        #if DEBUG
        Serial.println("Checking for mission");
        #endif
        unset_lt_interrupt();
        last_check_time = current_time; 

        if(mission_state != CONE_WAIT && mission_state !=DARK && is_ultrasonic()){
            mission_state = CONE;
        }
        
        if(is_bluetooth()){
            Serial.print("emergency");
            mission_state = EMERGENCY;
        }
        set_lt_interrupt();   
    }
}
int do_mission(int mission){
    // return 0 if success
    switch(mission){
        case DARK:{
            unset_lt_interrupt();
            car_update(STOP, 1);
            while(is_dark()){
                delay(500);
            }
            change_strategy(LEFT_FIRST);
            mission_state = MISSION_DONE;
            break;
        }
        case T_PARKING:{
            set_lt_interrupt();
            do_t_parking();
            strategy = RIGHT_FIRST;
            mission_state = MISSION_DONE;
            break;
        }
        case EMERGENCY:{
            unset_lt_interrupt();
            car_update(STOP, 1);
            send_bluetooth(); // send "EMERGENCY" to bluetooth
            delay(3000);
            mission_state = MISSION_DONE;
            break;
        }
        case CONE:{
            unset_lt_interrupt();
            car_update(STOP, 1);
            Serial.print("rfid read : ");
            cone_value = read_rfid();
            send_rfid_tagged();
            Serial.println(cone_value);
            mission_state = CONE_WAIT;
            break;
        }
        case CONE_WAIT:{
            while(is_ultrasonic()){
                delay(100);
            }
            switch(cone_value){
                case 0:{
                    change_strategy(RIGHT_FIRST);
                    mission_state = MISSION_DONE;
                    break;
                }
                case 1:{
                    mission_state = DARK;
                    break;
                }
                case 2:{
                    mission_state = T_PARKING;
                    break;
                }
            }
            break;
        }
        case MISSION_DONE:{
            set_lt_interrupt();
            mission_state = NOTHING;
            break;
        }
    }
    return 0;
}
int do_t_parking(){
    move_until(BACKWARD, SENSOR_F, SENSOR_F, FOREVER);

    Serial.println("fake line");
    car_update(STOP, 1000);
    car_update(BACKWARD, 50);
    move_until(BACKWARD, SENSOR_F, 0, 500);
    move_until(BACKWARD, SENSOR_F, SENSOR_F, FOREVER);
    Serial.println("real line");
    car_update(STOP, 1000);
    car_update(BACKWARD, 50);
    move_until(BACKWARD, SENSOR_F, 0, 1000);
    
    int i;
    //important parameters to tune!
    int min_steps = 6;
    int max_steps = 30;
    for(i=0;i<max_steps;i++){
        Serial.println(i);
        Serial.println(lt_sense_now(),BIN);
        if(i>min_steps && (lt_sense_now() & SENSOR_R))  break;
        move_until(TURN_RIGHT, SENSOR_F, SENSOR_F, 100);
        move_until(BACKWARD, SENSOR_F, 0, 100);    
    }
    if(i==30){
        //parking failed
    }
    else{
        move_until(BACKWARD, 0b011, 0, 5000);
    }
    move_until(TURN_RIGHT, SENSOR_F, SENSOR_F, 500);
    delay(1000);
    move_until(TURN_LEFT, SENSOR_L, SENSOR_L, 500);
    move_until(FORWARD, SENSOR_L, SENSOR_L, 500);
    return 0;
}
void change_strategy(int new_strategy){
    if(strategy == new_strategy)  return;
    set_lt_interrupt();
    if(strategy == LEFT_FIRST && new_strategy == RIGHT_FIRST){
        move_until(TURN_RIGHT, SENSOR_L, SENSOR_L, 300);
    }
    else if(strategy == RIGHT_FIRST && new_strategy == LEFT_FIRST){
        move_until(TURN_LEFT, SENSOR_R, SENSOR_R, 300);
    }
    strategy = new_strategy;
    return;
}

void travel(){
    // high level code for moving around
    lt_state = lt_sense();
    
    #if DEBUG
    if(strategy==LEFT_FIRST){
        if((lt_state & 0b11) != 0b01)
        Serial.println(lt_state & 0b11,BIN);  
    }
    else if(strategy==RIGHT_FIRST){
        if((lt_state & 0b11) != 0b10)
        Serial.println(lt_state & 0b11,BIN);  
    }
    
    #endif
    
    if(strategy==LEFT_FIRST){
        switch(lt_state & 0b11){
            case 0b01:{
                car_update(FORWARD, 10);
                break;
            }
            case 0b11:{
                move_until(TURN_LEFT, SENSOR_L, 0, 1000);
                break;
            }
            case 0b00:{
                move_until(TURN_RIGHT, SENSOR_R, SENSOR_R, 300);
                move_until(TURN_LEFT, SENSOR_R, SENSOR_R, 30);
                break;
            }
            case 0b10:{
                move_until(TURN_LEFT, SENSOR_R, SENSOR_R, 300);
                move_until(TURN_RIGHT, SENSOR_R, SENSOR_R, 30);
                break;
            }
        }
    }
    else if(strategy==RIGHT_FIRST){
        switch(lt_state & 0b11){
            case 0b10:{
                car_update(FORWARD, 10);
                break;
            }
            case 0b11:{
                move_until(TURN_RIGHT, SENSOR_R, 0, 1000);
                break;
            }
            case 0b00:{
                move_until(TURN_LEFT, SENSOR_L, SENSOR_L, 300);
                move_until(TURN_RIGHT, SENSOR_L, SENSOR_L, 30);
                break;
            }
            case 0b01:{
                move_until(TURN_RIGHT, SENSOR_L, SENSOR_L, 300);
                move_until(TURN_LEFT, SENSOR_L, SENSOR_L, 30);
                break;
            }
        }
    }
    if(SLOW)  car_update(STOP,20);
}

void setup(){
    Serial.begin(9600);
    init_lt_modules();
    init_motor();
    init_sensors();

    mission_state = NOTHING;
    strategy = RIGHT_FIRST;
    set_lt_interrupt();
    Serial.println("Run start!");
}
void loop(){
    // First check for any mission
    while(1){
        update_mission();
        if(mission_state != NOTHING){
            Serial.println(mission_state);
            do_mission(mission_state);
        }
        else{
            break;
        }
        delay(100);
    }
    travel();
}
