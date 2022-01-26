//$1 14 0 0 0 1 0 0 90 101 1 208;
#include <Wire.h>

int inArray[12];
int outArray[5];
int slaveAdress = 0x01;


void setup() {
  Serial.begin(9600);
  Wire.begin(); // инициализируем устройсво как master

}

void loop() {

  
}



bool readSlave(int slaveAdress,int inArray[]){
  const int lenInArray = sizeof(inArray ) / sizeof(inArray[0]);
  const byte startSymbol = (byte) '$'; 
  const byte finishSymbol = (byte) ';';
  static byte globalBuffer [lenInArray * 5];
  static int indexGlobalBuffer = 0;
  static bool startReadFlag = false;
  static int realByte = 0;
  
  Wire.requestFrom(slaveAdress, lenInArray * 5);
  while(int numBytes = Wire.available()){
    byte secondBuffer[numBytes];
    for(int i = 0; i < numBytes; i++){
      secondBuffer[i] = Wire.read();
    }

    for (int i = 0; i < numBytes; i++) {
      if (secondBuffer[i] == startSymbol) {
        indexGlobalBuffer = 0;
        startReadFlag = true;
        realByte = 0;
        continue;
      }else if (secondBuffer[i] == finishSymbol) {
           //обновляем глобальное состояние
        inArrayUpload(secondBuffer, realByte, inArray);
        realByte = 0;
        startReadFlag = false;
        indexGlobalBuffer = 0;
        return true;
      }
      
      if (startReadFlag) {
        if(indexGlobalBuffer == (lenInArray * 5)){
          realByte = 0;
          startReadFlag = false;
          indexGlobalBuffer = 0;
          return false;
      }
      
      globalBuffer[indexGlobalBuffer++] = secondBuffer[i];
      realByte++;
      }
    }
  }
}

void inArrayUpload (byte newInArray[], int realByte, int inArray[]){
  const int lenInArray = sizeof(inArray ) / sizeof(inArray[0]);
  const byte separatorSymbol = (byte) ' ';
  int bufferArray[lenInArray];
 
  for (int i = 0, acc = 0, factor = 0, indexOfBufferArray = 0; i < realByte + 1; i++) {
    if (i == realByte) {
      bufferArray[indexOfBufferArray] = acc;
      break;
    }

    if (newInArray[i] == separatorSymbol) {
        bufferArray[indexOfBufferArray] = acc;
        indexOfBufferArray++;
        
        if (indexOfBufferArray == (lenInArray)) {
          // пришедший пакет больше ожидаемого
          return;
        }
        
        acc = 0;
        factor = 0;
     } else if (((int)newInArray[i] - 48) >= 0 && ((int)newInArray[i] - 48) <= 9) {
        acc = ((acc * factor) + ((int)newInArray[i] - 48));
        factor = 10;
     } else {
         // была ошибка валидности пакета
        return ;
     }    
  }
  
  int crc = 0;
  for (int n = 0; n < lenInArray - 1; n++) {
    crc += bufferArray[n];
  }
        
  if (bufferArray[lenInArray - 1] == crc) {
    //все ок
    for(int i = 0; i < lenInArray; i++){
      inArray[i] = newInArray[i];
    }
    return ;
  } else {
    // была ошибка crc                 
    return ;
  }
} 

















/*

void serialTest(int timer){
  static unsigned long t = 0;
  if((millis() - t) > timer){
    t = millis();
    Serial.println("$1 85 86;");
  }
}




void serialMaster(){
  static int lengthBuferComand = sizeof(buferComand) / sizeof(buferComand[0]);
  String acc = "$";
  unsigned long crc = 0;
  int lengthGlobalStateBufer = 12;
  int globalStateBufer[lengthGlobalStateBufer];
   if(parsePacket((int*)globalStateBufer)){
    for(byte i = 0; i < lengthGlobalStateBufer - 1; i++){ // расчет CRC
      crc += globalStateBufer[i];
    }

    if(globalStateBufer[lengthGlobalStateBufer - 1] == crc){ //проверка контрольной суммы пройдена
      //записываем результаты в необходимые перменные
    
     //сразу же формируем ответ на отправку
     crc = 0; //очищаем преведущую CRC
     for(byte i = 0; i < lengthBuferComand; i++){ // расчет CRC
        crc += buferComand[i];
     }
     for(byte i = 0; i < lengthBuferComand; i++){ // формируем посылку 
      if(lengthBuferComand - 1 == i){
          acc+= crc;
          acc+= ";";
        }else{
          acc+= buferComand[i];
          acc+= " ";
        }  
      }
      Serial.println(acc); // отпровляем посылку
    }else{
      //обработка ошибки
    } 
  }
}


boolean parsePacket(int *intArray) {
    if (Serial.available()) {
        uint32_t timeoutTime = millis();
        int value = 0;
        byte index = 0;
        boolean parseStart = 0;

        while (millis() - timeoutTime < 100) {
            if (Serial.available()) {
                timeoutTime = millis();
                if (Serial.peek() == '$') {
                    parseStart = true;
                    Serial.read();
                    continue;
                }
                if (parseStart) {
                    if (Serial.peek() == ' ') {
                        intArray[index] = value / 10;
                        value = 0;
                        index++;
                        Serial.read();
                        continue;
                    }
                    if (Serial.peek() == ';') {
                        intArray[index] = value / 10;
                        Serial.read();
                        return true;
                    }
                    value += Serial.read() - '0';
                    value *= 10;
                }
                else
                {
                    Serial.read(); //возможно не будет работать нужна очистка 
                }
            }
        }
    }
    return false;
}



bool readSerial(int inArray[]){
    const int lenInArray = sizeof(inArray) / sizeof(inArray[0]);
    const byte startSymbol = (byte) '$';
    const byte finishSymbol = (byte) ';';
    static byte globalBuffer [lenInArray * 5];
    static int indexGlobalBuffer = 0;
    static bool startReadFlag = false;
    static int realByte = 0;
  delay(20);
  while (Serial.available()) {
      int lenReadBufer = Serial.available();
      byte readBuffer[lenReadBufer];
      int numRead = Serial.readBytes(readBuffer, lenReadBufer);
      
      
      for (int i = 0; i < numRead; i++) {
        if (readBuffer[i] == startSymbol) {
            indexGlobalBuffer = 0;
            startReadFlag = true;
            realByte = 0;
            continue;
        }else if (readBuffer[i] == finishSymbol) {
           //обновляем глобальное состояние
          inArrayUpload(globalBuffer, realByte);
          realByte = 0;
          startReadFlag = false;
          indexGlobalBuffer = 0;
          return true;

        }

        if (startReadFlag) {
          if(indexGlobalBuffer == (lenInArray * 5)){
            realByte = 0;
            startReadFlag = false;
            indexGlobalBuffer = 0;
            return false;
          }
          globalBuffer[indexGlobalBuffer++] = readBuffer[i];
          realByte++;
        }
    }
  }
  return false;  
}

void inArrayUpload (byte newInArray[], int realByte){
  int lenInArray = sizeof(inArray) / sizeof(inArray[0]);
  byte separatorSymbol = (byte) ' ';
  int bufferArray[lenInArray];
 
  for (int i = 0, acc = 0, factor = 0, indexOfBufferArray = 0; i < realByte + 1; i++) {
    if (i == realByte) {
       bufferArray[indexOfBufferArray] = acc;
       break;
    }

    if (newInArray[i] == separatorSymbol) {
        bufferArray[indexOfBufferArray] = acc;
        indexOfBufferArray++;
        
        if (indexOfBufferArray == (lenInArray)) {
          // пришедший пакет больше ожидаемого
          return;
        }
        acc = 0;
        factor = 0;
     } else if (((int)newInArray[i] - 48) >= 0 && ((int)newInArray[i] - 48) <= 9) {
         acc = ((acc * factor) + ((int)newInArray[i] - 48));
         factor = 10;
     } else {
         // была ошибка валидности пакета
         return ;
     }    
   }
  
        int crc = 0;
        for (int n = 0; n < lenInArray - 1; n++) {
            crc += bufferArray[n];
        }
        
        if (bufferArray[lenInArray - 1] == crc) {
            //все ок
            Serial.println("Yes");
            return ;
        } else {
            // была ошибка crc
                    
            return ;
        }
} 

*/
