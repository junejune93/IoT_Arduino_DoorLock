#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#define SS_PIN 10 // spi 통신을 위한 SS(chip select)핀 설정
#define RST_PIN 4 // 리셋 핀 설정

int emergencyMode_PIN = 2; // 비상 모드 키 입력
int resetMasterKey_PIN = 3; // 마스터키 리셋 핀
MFRC522 rfid(SS_PIN, RST_PIN); // 'rfid' 이름으로 클래스 객체 선언
MFRC522::MIFARE_Key key; 
SoftwareSerial P_Serial(6, 7); // 6번 : tx, 7번 : rx
byte masterKey[4] = {109, 4, 12, 50}; // 마스터 키 번호
LiquidCrystal_I2C lcd(0x3F, 16, 2); // lcd 출력 핀
char strR[5]; // 수신단에서 보낸 값을 저장
char* nuidR[2];

byte nuidPICC[4];   // 카드 ID들을 저장(비교)하기 위한 변수 선언

unsigned long pre_time = 0; // 채터링 방지
unsigned long cur_time = 0;

void setup() { 
  Serial.begin(9600); // 화면 출력 0, 1
  P_Serial.begin(9600); // 시리얼 통신 시작
  SPI.begin(); // SPI 통신 시작
  pinMode(emergencyMode_PIN, INPUT_PULLUP); // 비상 모드 핀
  pinMode(resetMasterKey_PIN, INPUT_PULLUP); // 마스터키 리셋 핀
  attachInterrupt(digitalPinToInterrupt(emergencyMode_PIN), emergencyMode, FALLING); // 비상 모드 함수
  attachInterrupt(digitalPinToInterrupt(resetMasterKey_PIN), resetMasterKey, FALLING); // 마스터 키 변경 모드 함수
  rfid.PCD_Init(); // RFID(MFRC522) 초기화 
  for (byte i = 0; i < 6; i++) { // ID값 초기화  
    key.keyByte[i] = 0xFF;
  }
  lcd.init(); // lcd 초기화
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Contact card");
}
 
void loop() {
  if ( ! rfid.PICC_IsNewCardPresent()) // 카드 접촉이 있을 때만 다음 단계로 넘어감
    return;

  if ( ! rfid.PICC_ReadCardSerial()) // 카드 읽힘이 제대로 되면 다음으로 넘어감
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && // MIFARE 방식의 카드인지 확인 루틴
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wrong Card Type");
    return;
  }

  // 이전 인식된 카드와 다른, 혹은 새카드가 인식되면 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) {
    for (byte i = 0; i < 4; i++) { // 고유아이디(UID) 값을 저장한다.
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

 // 수신단에 RFID값 전송
  String sensing0 = (String)"8"; // 데이터 타입 확인 (8 : 일반 송신, 9 : 마스터키 변경)
  String sensing1 = (String)nuidPICC[0];
  String sensing2 = (String)nuidPICC[1];
  String sensing3 = (String)nuidPICC[2];
  String sensing4 = (String)nuidPICC[3]; // 통신할때는 ASC로 바뀌기 때문에 바꿈
  String sensingsum = sensing0 + "@" + sensing1 + "@" + sensing2 + "@" + sensing3 + "@" + sensing4;
  P_Serial.print(sensingsum);
  P_Serial.println();
  
 // 수신단에서 결과값을 다시 보내줌 
  String str1R; // 수신단 입력값
  if(P_Serial.available()){
    int i = 0;
    char*ptr = NULL;
    while (P_Serial.available()) {
      str1R = P_Serial.readStringUntil('\n');
    }
    str1R.toCharArray(strR, str1R.length());
    ptr = strtok(strR, "@"); 
    while (ptr != NULL) {
      nuidR[i++] = ptr;
      ptr = strtok(NULL, "@");
    }
    if(atoi(nuidR[0])==1){ // 결과값으로 lcd에 문 개폐여부 출력 (1 : 열림 / 2 : 닫힘)
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Door is Open");
    } else if(atoi(nuidR[0])==2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Door is Close");
    }
  }
}

// 비상 모드 함수 (버튼을 누르면 마스터키 초기화, 이후 마스터키 저장번호와 대조후 판별)
void emergencyMode() {
  cur_time = millis();
  if (cur_time - pre_time >= 300) {
    interrupts();
    pre_time = cur_time;
    for (int i = 0; i < 4; i++) {
      nuidPICC[i] = 0;
    }
    while (1) {  // 마스터키 재입력
      if (masterKey[0] == nuidPICC[0] && masterKey[1] == nuidPICC[1] && masterKey[2] == nuidPICC[2] && masterKey[3] == nuidPICC[3]) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Emergency Mode");
        lcd.setCursor(0,1);
        lcd.print("Clear");
        delay(1000);
        return;
      } else if (rfid.PICC_IsNewCardPresent()) {
        rfid.PICC_ReadCardSerial();
        for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
        }
      } else {  // 마스터키 입력이 안될경우 반복
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Need Master key");
        delay(100);
      }
    }
  }
}

// 마스터키 초기화 함수
void resetMasterKey() {
  cur_time = millis();
  if (cur_time - pre_time >= 300) {
    interrupts();
    pre_time = cur_time;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Reset Master Key");
    int print = 0;
    for (int i = 0; i < 4; i++) {
      nuidPICC[i] = 0;
    }
    while (1) {
      delayMicroseconds(100);
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
        }
        for (int i = 0; i < 4; i++) { // 새로운 마스터키 입력 받음
          masterKey[i] = nuidPICC[i];
        }
        print = 1;
        break;
      }
    }
    if (print) {
      String ChangeSensing0 = (String) "9";  // 데이터 타입 확인
      String ChangeSensing1 = (String)nuidPICC[0];
      String ChangeSensing2 = (String)nuidPICC[1];
      String ChangeSensing3 = (String)nuidPICC[2];
      String ChangeSensing4 = (String)nuidPICC[3];
      String ChangeSensingsum = ChangeSensing0 + "@" + ChangeSensing1 + "@" + ChangeSensing2 + "@" + ChangeSensing3 + "@" + ChangeSensing4;
      P_Serial.print(ChangeSensingsum); // 변경된 마스터키 송신단에 전송
      P_Serial.println();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Reset Master Key");
      lcd.setCursor(0,1);
      lcd.print("Done");
      delay(1000);
    }
  }
}