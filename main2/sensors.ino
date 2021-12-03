void init_light(){
    pinMode(LIGHT, INPUT);
}
bool is_dark(){
    // return true if light sensor is dark.
    return analogRead(LIGHT) > LIGHT_THRESHOLD;
}


void init_ultrasonic(){
    pinMode(TRIGGER_PIN,OUTPUT);
    pinMode(ECHO_PIN,INPUT);
}
bool is_ultrasonic(){
    // return true if cone is ahead.
    long duration, cm;
    digitalWrite(TRIGGER_PIN,HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN,LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
    cm = duration/29/2;
    Serial.println(cm);
    
    if (cm<=15) {
      return true;
    } else {
      return false;
    }
}

SoftwareSerial bt_serial(RX_PIN, TX_PIN);
void init_bluetooth(){
  bt_serial.begin(9600);
}
bool is_bluetooth(){
    // return true if signal received.
    bool ret;
    if (bt_serial.available() > 0){
        byte data = (byte) bt_serial.read();
        if (data == 'e'){
            ret = true;   
        } else {
            ret = false;   
        }
    } else {
        ret = false;   
    }
    return ret;
}
void send_bluetooth(){
    //send "EMERGENCY" to phone
    bt_serial.println("EMERGENCY");
}
void send_rfid_tagged(){
    bt_serial.println("rfid");
}

#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <SPI.h>


MFRC522 mfrc522(SS_PIN,RST_PIN);
MFRC522::MIFARE_Key key;
byte buffer[18];  //Buffer size, at least 18 bytes.
byte block, status;
byte size;

void init_rfid(){
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i<6; i++) key.keyByte[i]=0xff;
  block = 6;
  size = sizeof(buffer);
  Serial.println("Scan Card to see UID and type...");
}
int read_rfid(){
    // wait until rfid tagged then read it.
    // return 0 or 1 or 2
    int mode;
    while(1){
          Serial.println('a');
          delay(100);
          if(!mfrc522.PICC_IsNewCardPresent()){
            continue;
          }
          if(!mfrc522.PICC_ReadCardSerial()){
            continue;
          }
          status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,block,&key,&(mfrc522.uid));
          Serial.println(mfrc522.GetStatusCodeName(status));
    
          status = mfrc522.MIFARE_Read(block, buffer, &size);
          if (status != MFRC522::STATUS_OK){
            continue;
          }
          else{
            if(buffer[0] == 0x21){
              byte data = buffer[1];
              if(data == 0x00){
                mode = 0; //RFS
              }
              else if(data == 0x01){
                mode = 1; //LFS
              }
              else if(data == 0x02){
                mode = 2; //T_PARKING
              }
            }
//            mfrc522.PICC_HaltA();
            mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
            break;
          }
      }
      
      return mode;
}

void init_sensors(){
    init_light();
    init_ultrasonic();
    init_bluetooth();
    init_rfid();
}
