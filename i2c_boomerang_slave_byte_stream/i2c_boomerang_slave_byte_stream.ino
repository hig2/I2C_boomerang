//$1 14 0 0 0 1 0 0 90 101 1 208;
#include <Wire.h>


int inArray[5];
int outArray[12] = {1, 14, 0, 0, 0, 1, 0, 0, 90, 101, 1, 0};



void setup() {
  Serial.begin(9600);
  init_I2C();
}

void loop() {
  delay(20);
}

void init_I2C(){
  Wire.begin(0x01); // инициализируем устройсво как slave
  Wire.onRequest(write_I2C_slave);
  Wire.onReceive(read_I2C_slave);
}


void write_I2C_slave(){
  const int lenOutArray = sizeof(outArray) / sizeof(outArray[0]);
  const int lenWriteArray = (lenOutArray * 2) + 4;
  byte writeArray[lenWriteArray];
  long acc = 0;
  

  for(int i = 0; i < lenOutArray; i++ ){
    writeArray[i * 2] = (byte)outArray[i];
    writeArray[(i * 2) + 1 ] = (byte)(outArray[i] >> 8);
    acc += outArray[i];
  }

  for(int i = 0; i < 4; i++){
    writeArray[i +(lenOutArray * 2)] = i == 0 ? (byte) acc : (byte) (acc >> (i * 8));
  }
  
  Wire.write(writeArray, lenWriteArray);
}



bool read_I2C_slave(){
  const int lenInArray = sizeof(inArray ) / sizeof(inArray[0]);
  int bufferInArray[lenInArray];
  long crc = 0;
  long acc = 0;
  
  while(int numBytes = Wire.available()){
    byte secondBuffer[numBytes];
    for(int i = 0; i < numBytes; i++){
      secondBuffer[i] = Wire.read();
    }

    for(int i = 0; i < lenInArray; i++){
        bufferInArray[i] = (((int)secondBuffer[(i * 2) + 1 ]) << 8 ) | secondBuffer[ i * 2];
        acc += bufferInArray[i];
    }
    
    for(int i = 0; i < 4; i++){
      crc = i == 0 ? secondBuffer[i + (lenInArray * 2)] : (((int)secondBuffer[i + (lenInArray * 2)]) << (8 * i)) | crc;
    } 

    if(acc == crc){
      for(int i = 0; i < lenInArray; i++){
        inArray[i] = bufferInArray[i];
        Serial.print(bufferInArray[i]);
        Serial.print(" ");
      }
      Serial.println("CRC: " + crc);
    }else{
      Serial.print("Ошибка crc: ");
      Serial.println(crc);
      
    }
    
  }   
}
