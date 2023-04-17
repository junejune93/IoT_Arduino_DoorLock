#include <Servo.h>

#include <SoftwareSerial.h>

SoftwareSerial P_Serial(6,7);
Servo servo;

char str[50];
char* nuid[5];
char* nuid[4];

void setup() {
  Serial.begin(9600);
  P_Serial.begin(9600);
  servo.attach(9);
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
     
    char*temp = NULL;

    while(1){
      str[index++]=P_Serial.read();
      if(str[index++]== NULL)
        break;
    }
    
    str[index] = '\0';
     
    ptr = strtok(str," ");
    temp = ptr;

    while(ptr !=NULL){      // 카드값 출력
      nuid[i++]=temp;
      temp=strtok(ptr,"@"); 
    }
    
    //카드 입력시 인증, 차단
    int a = atoi(nuid[1]);
    int b = atoi(nuid[2]);
    int c = atoi(nuid[3]);
    int d = atoi(nuid[4]);
    
    if(a == 109 && b == 4 && c == 12 && d == 50){
      Serial.println("Autohrized access");
      servo.write(180);    
      delay(3000);
      servo.write(0);
    }
    else{
      Serial.println("Access denied");
      servo.write(0);
      delay(500);
    }
  } 
}

    Serial.print(nuid[0]);
    // i=0;

    // ptr = nuid[0];
    
    // while(ptr !=NULL){      // 카드값 출력
    //   id[i++]=ptr;
    //   ptr=strtok(NULL,"@"); 
    // }


    // while(ptr !=NULL){      // 카드값 출력
    //   nuid[i++]=ptr;
    //   ptr=strtok(NULL,"@"); 
    // }
    
    //   delay(100);
    
    
//     //카드 입력시 인증, 차단
//     int a = atoi(nuid[1]);
//     int b = atoi(nuid[2]);
//     int c = atoi(nuid[3]);
//     int d = atoi(nuid[4]);
    
//     if(a == 109 && b == 4 && c == 12 && d == 50){
//       Serial.println("Autohrized access");
//       servo.write(180);    
//       delay(3000);
//       servo.write(0);
//     }
//     else{
//       Serial.println("Access denied");
//       servo.write(0);
//       delay(500);
//     }
//   } 
  }
}