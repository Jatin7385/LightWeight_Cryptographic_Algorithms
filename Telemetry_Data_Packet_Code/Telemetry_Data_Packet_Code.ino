#include <Crypto.h>
#include <CryptoLW.h>
#include <Speck.h>
#include <SpeckSmall.h>
#include <SpeckTiny.h>
#include <string.h>


int packetCount = 0;

struct TestVector
{
    const char *name;
    byte key[32];
    byte plaintext[16];
    byte ciphertext[16];
};


void setup() {
  Serial.begin(9600);

//  String prefix = "1007,23:52:01,16,C,F,R,";
//  String suffix = ",25.436,8.843,23:52:01,37.2314434,80.42644343,422.6434,7,0";
//  byte plain[message.length()];
//  message.getBytes(plain, message.length());
//  for(int i = 0; i<message.length(); i++)
//  {
//    Serial.println(plain[i]);
//  }
//
//  Serial.println("******");

}

int getAltitude()
{
  int randNumber = random(0,1000);
  return randNumber;
}

int getTemperature()
{
  int randNumber = random(25,40);
  return randNumber;
}

int getVoltage()
{
  return random(6,10);
}



void loop() {
  // put your main code here, to run repeatedly:

  // Getting sensor readings
  int altitude = getAltitude();
  int temperature = getTemperature();
  int voltage = getVoltage();

  //Generating datapacket
  String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
  String(voltage) + ",23:52:01,37.2314434,80.42644343,422.6434,7,0";

  Serial.print("************************************");
  Serial.print(packetCount);
  Serial.println("************************************");
  Serial.println(dataPacket);

  if(dataPacket.length() % 16 != 0)
  {
    for(int i = 0; i < dataPacket.length() % 16; i++)
    {
      dataPacket = dataPacket + "/";
    }
  }

  // Converting string to bytes for encryption
  byte plain[dataPacket.length()];
  dataPacket.getBytes(plain, dataPacket.length());

  // Converting the string into blocks of 16 bytes or 128 bits
  int len = (sizeof(plain)/sizeof(plain[0]));
  Serial.print("Data Packet Length : ");
  Serial.println(len);
  int numberOfBlocks = len/16;
  String cipherText = "";
  byte plainText[16];
  byte cipherTextArray[16];
  for(int i = 0; i < numberOfBlocks; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      plainText[j] = plain[(16*i) + j];
      Serial.print(plainText[j]);
      Serial.print(" , ");
      Serial.println(dataPacket[(16*i) + j]);
    }
    Serial.println("---------------------------------");
  }
  
  packetCount++;
  delay(10000);
}
