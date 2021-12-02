#include <TimerOne.h>

void init_lt_modules(){
    pinMode(LT_MODULE_L, INPUT);
    pinMode(LT_MODULE_F, INPUT);
    pinMode(LT_MODULE_R, INPUT);
}

void lt_interrupt(){
    l_cnt = min(l_cnt+2,1000);
    f_cnt = min(f_cnt+2,1000);
    r_cnt = min(r_cnt+2,1000);
    if(analogRead(LT_MODULE_L) > LT_THRESHOLD_L)    l_cnt = 0;
    if(analogRead(LT_MODULE_F) > LT_THRESHOLD_F)    f_cnt = 0;
    if(analogRead(LT_MODULE_R) > LT_THRESHOLD_R)    r_cnt = 0;
}

int lt_sense(){
    return (l_cnt<=10)*SENSOR_L+(f_cnt<=10)*SENSOR_F+(r_cnt<=10)*SENSOR_R;
}
int lt_sense_now(){
    return (l_cnt<=2)*SENSOR_L+(f_cnt<=2)*SENSOR_F+(r_cnt<=2)*SENSOR_R;
}
void reset_lt(){
    l_cnt = 1000;
    f_cnt = 1000;
    r_cnt = 1000;
}
void set_lt_interrupt(){
    reset_lt();
    Timer1.initialize(2000); // every 2ms
    Timer1.attachInterrupt(lt_interrupt);
    delay(14); // wait before lt_sense can work properly.
}
void unset_lt_interrupt(){
    Timer1.detachInterrupt();
}
