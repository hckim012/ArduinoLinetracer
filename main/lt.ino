#include <TimerOne.h>

void init_lt_modules(){
    pinMode(LT_MODULE_L, INPUT);
    pinMode(LT_MODULE_F, INPUT);
    pinMode(LT_MODULE_R, INPUT);
}
volatile float left, forward, right;
volatile int ltValue;

void lt_interrupt(){
    // left = left * 0.98 + analogRead(LT_MODULE_L) * 0.02;
    // forward = forward * 0.98 + analogRead(LT_MODULE_F) * 0.02;
    // right = right * 0.98 + analogRead(LT_MODULE_R) * 0.02;
    l_cnt = min(l_cnt+1,1000);
    f_cnt = min(f_cnt+1,1000);
    r_cnt = min(r_cnt+1,1000);
    if(analogRead(LT_MODULE_L) > LT_THRESHOLD_L)    l_cnt = 0;
    if(analogRead(LT_MODULE_F) > LT_THRESHOLD_F)    f_cnt = 0;
    if(analogRead(LT_MODULE_R) > LT_THRESHOLD_R)    r_cnt = 0;
}

int lt_sense(){
    // ltValue = ((left>LT_THRESHOLD_L)?SENSOR_L:0) |
    //     ((forward>LT_THRESHOLD_F)?SENSOR_F:0) |
    //     ((right>LT_THRESHOLD_R)?SENSOR_R:0);
    ltValue = (l_cnt<10)*SENSOR_L+(f_cnt<10)*SENSOR_F+(r_cnt<10)*SENSOR_R;
    return ltValue;
}

void set_lt_interrupt(){
    l_cnt = 1000;
    f_cnt = 1000;
    r_cnt = 1000;
    Timer1.initialize(1000); // every 1ms
    Timer1.attachInterrupt(lt_interrupt);
}
