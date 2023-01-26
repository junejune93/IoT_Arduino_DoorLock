#include <SoftwareSerial.h>

SoftwareSerial P_Serial(6,7);

char str[50];
char* nuid[4];
char** pnuid = NULL;
int j = 0;
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
    }
    str[index] = '\0';
    delay(100);
    
    ptr = strtok(str,"@"); 
    while(ptr !=NULL){      // 카드값 출력
      nuid[i++]=ptr;
      Serial.print(ptr);
      ptr=strtok(NULL,"@"); 
    }
  }
  
  for(i = 0; i < 4; i++)
  {
    if(pnuid[i] == nuid){
    
    }
    
    else{
      for(i = 0; i < 4; i++){ // 새카드값 입력
        if(pnuid[i]=NULL){
        pnuid[i] = nuid;
        break;
        }
      }
    }  
  }
}  


