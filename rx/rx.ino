#include <SoftwareSerial.h>

SoftwareSerial P_Serial(6,7);

char str[50];
char* nuid[5];
char* p;
void setup() {
  Serial.begin(9600);
  P_Serial.begin(9600);
}

void loop() {
  
  if(P_Serial.available()){
    
    int index = 0, i = 0;
    char*ptr = NULL;

    while(P_Serial.available()){
      str[index++]=P_Serial.read();
      // Serial.print(str[index-1]);
    }
    
    str[index] = '\0';
    
    delay(100);
    
    ptr = strtok(str,"@"); 
     
    while(ptr !=NULL){      // 카드값 출력
      nuid[i++]=ptr;
      ptr=strtok(NULL,"@"); 
    }
    
    //카드 입력시 인증, 차단
    int a = atoi(nuid[1]);
    int b = atoi(nuid[2]);
    int c = atoi(nuid[3]);
    int d = atoi(nuid[4]);
    
    if(a == 109 && b == 4 && c == 12 && d == 50){
      Serial.println("Autohrized access");    
      delay(500);
    }
    else{
      Serial.println("Access denied");
      delay(500);
    }
  } 
}