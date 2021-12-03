#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte buffer[30];
byte size;
byte block, status;

int strategy = 0;

#define LT_MODULE_L A2
#define LT_MODULE_F A1
#define LT_MODULE_R A0

#define CAR_DIR_FW 1
#define CAR_DIR_BW 2
#define CAR_DIR_TR 3
#define CAR_DIR_TL 4
#define CAR_DIR_ST 5

#define ENA 6
#define EN1 7
#define EN2 3
#define ENB 5
#define EN3 4
#define EN4 2

int g_carDirection = 0;
int speed = 120;
int tspeed = 250;


void setup() {
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(EN3,OUTPUT);
  pinMode(EN4,OUTPUT);
  pinMode(EN1,OUTPUT);
  pinMode(EN2,OUTPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i<6; i++) key.keyByte[i]=0xff;
  block = 6;
  size = sizeof(buffer);
  Serial.println("Scan Card to see UID and type...");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,block,&key,&(mfrc522.uid));
  Serial.println(mfrc522.GetStatusCodeName(status));

  status = mfrc522.MIFARE_Read(block, buffer, &size);
  
    if(buffer[0] == 0x21){
    byte mode = buffer[1];
    if(mode == 0x00){
      strategy = 0;
      Serial.println(strategy);
      digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
    delay(1000);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    }
    else if(mode == 0x01){
      strategy = 1;
      Serial.println(strategy);
      digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, LOW);
  digitalWrite(EN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  delay(1000);
  analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    }
    else if(mode == 0x02){
      strategy = 2;
      Serial.println(strategy);
    }
  
  else{
    Serial.println("nope");
    }
  }
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  delay(1000);
}
