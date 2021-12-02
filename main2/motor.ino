void init_motor(){
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);
}

void roll_r(int val){
  // clip values to appropriate value and roll forever
  if(val<MIN_WHEEL_SPEED && val > -MIN_WHEEL_SPEED){
    val = 0;
    analogWrite(ENA, 0);
  }
  else if(val>0){
    val = min(val, MAX_WHEEL_SPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, val);
  }
  else{
    val = max(val, -MAX_WHEEL_SPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, -val);
  }
}
void roll_l(int val){
  // clip values to appropriate value and roll forever
  if(val<MIN_WHEEL_SPEED && val > -MIN_WHEEL_SPEED){
    val = 0;
    analogWrite(ENB, 0);
  }
  else if(val>0){
    val = min(val, MAX_WHEEL_SPEED);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, val);
  }
  else{
    val = max(val, -MAX_WHEEL_SPEED);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, -val);
  }
}

void roll(int direction){
  switch(direction){
        case FORWARD:
            roll_l(speed);
            roll_r(speed);
            break;
        case BACKWARD:
            roll_l(-backward_speed);
            roll_r(-backward_speed);
            break;
        case TURN_RIGHT:
            roll_l(curve_speed);
            roll_r(-curve_speed);
            break;
        case TURN_LEFT:
            roll_l(-curve_speed);
            roll_r(curve_speed);
            break;
        case STOP:
        default:
            roll_l(0);
            roll_r(0);
            break;
    }
}
