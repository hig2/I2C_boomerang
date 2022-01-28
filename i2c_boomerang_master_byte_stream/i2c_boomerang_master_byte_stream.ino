//$1 14 0 0 0 1 0 0 90 101 1 208;
#include <Wire.h>

int inArray[12];
int outArray[5] = {1, 2, 3, 4, 0};
int slaveAdress = 0x01;

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
  byte writeArray[(lenOutArray * 2) + 4];

  for(int i = 0; i < lenOutArray; i++ ){
    writeArray[i * 2] = (byte)outArray[i];
    writeArray[(i * 2) + 1 ] = (byte)(outArray[i] >> 8);
  }
  
  Wire.beginTransmission(slaveAdress); 
  Wire.write(writeArray, (lenOutArray * 2) + 4);
  Wire.endTransmission();     
}



bool read_I2C_master(int slaveAdress){
  const int lenInArray = sizeof(inArray ) / sizeof(inArray[0]);
  int bufferInArray[lenInArray];
  long crc = 0;
  
  Wire.requestFrom(slaveAdress, (lenInArray * 2) + 4);
  while(int numBytes = Wire.available()){
    byte secondBuffer[numBytes];
    for(int i = 0; i < numBytes; i++){
      secondBuffer[i] = Wire.read();
    }

    for(int i = 0; i < lenInArray; i++){
        bufferInArray[i] = (((int)secondBuffer[(i * 2) + 1 ]) << 8 ) | secondBuffer[ i * 2];
        Serial.print(bufferInArray[i]);
        Serial.print(" ");
    }
      Serial.println("");
  }   
}
