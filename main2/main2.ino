#include "include.h"
long counts[6];
long curve = 0;
int lt_state;

//SoftwareSerial bt_serial(2,3);

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
    if(digitalRead(SW)==LOW){
      set_counts(STOP, counts[STOP]+10);
      roll(STOP);
      delay(10);
      return;
    }
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
//        if(curve%1000==999){
//            speed += 10;
//            Serial.print("Adjust speed to");
//            Serial.println(speed);
//        }
    }
    
    if(counts[FORWARD]>100){
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
    else if(counts[BACKWARD]>30){
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
int check_mission(){
    //TODO
    long current_time = millis();
    if(current_time - last_check_time > 1000){
        #if DEBUG
        Serial.println("Checking for mission");
        #endif
        unset_lt_interrupt();
        last_check_time = current_time; 
        
        if(mission_state < DARK && is_dark()){
            Serial.println("Mission Dark");
            mission_state = DARK;
            return DARK;
        }
        set_lt_interrupt();   
    }
    return NOTHING;
}
int do_mission(int mission){
    //TODO
    // return 0 if success
    switch(mission){
        case DARK:{
            car_update(STOP, 0);
            while(1){
                if(!is_dark())  break;
                delay(500);
            }
            change_strategy(RIGHT_FIRST);
            break;
        }
        case T_PARKING:{
            move_until(BACKWARD, SENSOR_F, SENSOR_F, FOREVER);
            Serial.println(lt_sense_now(),BIN);
            if((lt_sense_now() & 0b011)==0b00){
                
            }
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
            int min_steps = 10;
            int max_steps = 30;
            for(i=0;i<max_steps;i++){
                Serial.println(i);
                if(i>min_steps && (lt_sense_now() & SENSOR_R))  break;
                move_until(TURN_RIGHT, SENSOR_F, SENSOR_F, 300);
                car_update(BACKWARD, 50);
                move_until(BACKWARD, SENSOR_F, 0, 300);    
            }
            if(i==30){
                //parking failed
            }
            else{
                move_until(BACKWARD, 0b011, 0, 5000);
            }
            car_update(BACKWARD, 300);
            strategy = RIGHT_FIRST;
            break;
        }
    }
    return 0;
}

void change_strategy(int new_strategy){
    if(strategy == new_strategy)  return;
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
//    bt_serial.begin(9600);
    init_lt_modules();
    init_motor();
    init_light();
    
    pinMode(SW, OUTPUT);
    //blink
    for(int i=0;i<3;i++){
        digitalWrite(SW, HIGH);
        delay(150);
        digitalWrite(SW,LOW);
        delay(150);
    }
    pinMode(SW, INPUT);

    mission_state = NOTHING;
    strategy = LEFT_FIRST;
    set_lt_interrupt();
    while(digitalRead(SW)==LOW) delay(100);
    Serial.println("Run start!");
    do_mission(T_PARKING);
    delay(FOREVER);
    Serial.println("parking end");
}
void loop(){
    // First check for any mission
    while(1){
        if(check_mission()!=NOTHING){
            do_mission(mission_state);
        }
        else{
            break;
        }
        delay(100);
    }
    travel();
}
