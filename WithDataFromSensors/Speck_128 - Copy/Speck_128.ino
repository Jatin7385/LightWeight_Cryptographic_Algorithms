#include <Crypto.h>
#include <CryptoLW.h>
#include <Speck.h>
#include <SpeckSmall.h>
#include <SpeckTiny.h>
#include <string.h>


int packetCount = 0;

byte key[] = {0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
                    0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};

Speck speck;

struct TestVector
{
    const char *name;
    byte key[32];
    byte plaintext[16];
    byte ciphertext[16];
};


void setup() {
  Serial.begin(9600);
}

byte buffer[16];

String encrypt(BlockCipher *cipher, byte *plainText, size_t keySize)
{
  crypto_feed_watchdog();

  cipher->setKey(key, keySize);
  cipher->encryptBlock(buffer, plainText);

  String str ="";
  for (uint8_t i = 0; i < 16; i++)
  {
      str += (char)buffer[i];
  }
    
    Serial.println(str);

    return str;
}

String decrypt(BlockCipher *cipher, byte *cipherText, size_t keySize)
{ 
   cipher->decryptBlock(buffer, cipherText);

   String str = "";
    for (uint8_t i = 0; i < 16; i++)
    {
        str += (char)buffer[i];
    }
    
    Serial.println(str);

    return str;
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


  // *********************************************************************************************
  // Encryption Process

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
    cipherText = cipherText + encrypt(&speck, plainText ,16);
    Serial.println("---------------------------------");
  }

  Serial.print("Cipher Text : ");
  Serial.print(cipherText);

  Serial.print(" , Length of Cipher Text");
  Serial.println(cipherText.length());

  //*********************************************************************************************************
  // Decryption Process
  
  //Converting the ciphertext string to byte array for decryption process
  byte cipherBytes[cipherText.length()];
  cipherText.getBytes(cipherBytes, cipherText.length());
  byte cipherBytesBlock[16];

  String decryptedText = "";
  for(int i = 0; i < numberOfBlocks; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      cipherBytesBlock[j] = cipherBytes[(16*i) + j];
      Serial.print(cipherBytesBlock[j]);
      Serial.print(" , ");
      Serial.println(cipherText[(16*i) + j]);
    }
    decryptedText = decryptedText + decrypt(&speck, cipherBytesBlock ,16);
    Serial.println("---------------------------------");
  }

  Serial.print("Decryted Text : ");
  Serial.print(decryptedText);
  Serial.print(" , Length of Cipher Text");
  Serial.println(decryptedText.length());
  
  
  packetCount++;
  delay(10000);
}
