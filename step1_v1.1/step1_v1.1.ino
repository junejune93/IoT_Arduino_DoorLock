#include<Servo.h> // 서보모터 구동 라이브러리
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);

Servo servo; // servo 클래스로 객체 생성
#define trig 7 // trig 7번핀으로 정의
#define echo 6 // echo 6번핀으로 정의
int angle = 0; // 초기 각도 0으로 설정

void setup() {
  servo.attach(2);
  Serial.begin(9600);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
}

void loop() {
  long duration, distance;
  
  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = ((float)(340*duration)/10000)/2;

  Serial.print("거리 : ");
  Serial.print(distance);
  Serial.println("cm");

  if(distance < 10){
    angle = 70;
		lcd.init();
	  lcd.backlight();
    lcd.setCursor(0,0);
	  lcd.print("Success!");
		lcd.setCursor(0,1);
	  lcd.print("DOOR OPEN");
		Serial.println("문이 열렸습니다.");
    delay(500);
  } else{
    angle = 180;
		lcd.init();
	  lcd.backlight();
		lcd.setCursor(0,1);
	  lcd.print("DOOR CLOSE");
    delay(500);
  }
  servo.write(angle);
}