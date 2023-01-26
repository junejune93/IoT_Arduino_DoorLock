#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10    // spi 통신을 위한 SS(chip select)핀 설정
#define RST_PIN 9    // 리셋 핀 설정
 
MFRC522 rfid(SS_PIN, RST_PIN); // 'rfid' 이름으로 클래스 객체 선언
MFRC522::MIFARE_Key key; 
SoftwareSerial P_Serial(6, 7); // 6번 : tx, 7번 : rx

byte nuidPICC[4];   // 카드 ID들을 저장(비교)하기 위한 배열(변수)선언

void setup() { 
  Serial.begin(9600); // 화면 출력 0, 1
  SPI.begin(); // SPI 통신 시작
  rfid.PCD_Init(); // RFID(MFRC522) 초기화 
  // ID값 초기화  
  P_Serial.begin(9600);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
 
void loop() {
  // 새카드 접촉이 있을 때만 다음 단계로 넘어감
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // 카드 읽힘이 제대로 되면 다음으로 넘어감
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  // 현재 접촉 되는 카드 타입을 읽어와 모니터에 표시함
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // MIFARE 방식의 카드인지 확인 루틴
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  // 이전 인식된 카드와 다른, 혹은 새카드가 인식되면 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

 // 고유아이디(UID) 값을 저장한다.
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
  }
 // 연속으로 동일한 카드를 접촉하면 다른 처리 없이 
 // '이미 인식된 카드'라는 메세지를 출력한다.
  else Serial.println(F("Card read previously."));
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
 // 수신단에 RFID값 전송
  String sensing1 = (String)nuidPICC[0];
  String sensing2 = (String)nuidPICC[1];
  String sensing3 = (String)nuidPICC[2];
  String sensing4 = (String)nuidPICC[3]; // 통신할때는 ASC로 바뀌기 때문에 바꿈
  String sensingsum = (String)sensing1 + "@" + (String)sensing2 + "@" + (String)sensing3 + "@" + (String)sensing4;
  Serial.println(sensingsum);
  P_Serial.println(sensingsum);
}