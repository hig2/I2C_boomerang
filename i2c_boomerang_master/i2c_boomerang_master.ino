//$1 14 0 0 0 1 0 0 90 101 1 208;
#include <Wire.h>

int inArray[12];
int outArray[5] = {1, 2, 3, 4, 0};
int slaveAdress = 0x01;

const byte separatorSymbol = (byte) ' ';
const byte startSymbol = (byte) '$';
const byte finishSymbol = (byte) ';';

void setup() {
  Serial.begin(9600);
  Wire.begin(); // инициализируем устройсво как master

}

void loop() {
  masterTask(1000);
}

void masterTask(int timer){
  static unsigned long t = 0;
  if((millis() - t) > timer){
    t = millis();
    read_I2C_master(slaveAdress);
    write_I2C_master(slaveAdress);
  }
}


void write_I2C_master(int slaveAdress){
  const int lenOutArray = sizeof(outArray) / sizeof(outArray[0]);
  String acc = (String)(char)startSymbol;
  int crc = 0;
  
  for (int n = 0; n < lenOutArray - 1; n++) {
    crc += outArray[n];
  }
  outArray[lenOutArray - 1] = crc;
  
  for(int i = 0; i < lenOutArray; i++){
    if(i == (lenOutArray - 1)){
      acc += (String)outArray[i] + (char)finishSymbol;
     break;
    }
    acc += (String)outArray[i] + (char)separatorSymbol;
  }

  
  byte result[acc.length() + 1];
  acc.getBytes(result, acc.length() + 1);
  Wire.beginTransmission(slaveAdress); 
  Wire.write(result, acc.length());
  Wire.endTransmission();     
}



bool read_I2C_master(int slaveAdress){
  const int lenInArray = sizeof(inArray ) / sizeof(inArray[0]);
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
         
         
        inArrayUpload(globalBuffer, realByte, inArray, lenInArray);
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

void inArrayUpload (byte newInArray[], int realByte, int inArray[], int lenInArray){
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
      inArray[i] = bufferArray[i];
     Serial.print(inArray[i]);
     Serial.print(" ");
    }
      Serial.println("");
    return ;
    
  } else {
    // была ошибка crc                 
    return ;
  }
} 
