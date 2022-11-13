#include <Crypto.h>
#include <AES.h>
#include <string.h>

int packetCount = 0;


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


//
//key[16] cotain 16 byte key(128 bit) for encryption
byte key[32]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
//cypher[16] stores the encrypted text
byte cypher[16];
//decryptedtext[16] stores decrypted text after decryption
byte decryptedtext1[16];
//creating an object of AES128 class
AESSmall256 aes128;

String cipherText = "";
byte plainText[16];
byte cipherTextArray[16];

String encrypt(byte *plaintext)
{
     aes128.encryptBlock(cypher,plaintext);//cypher->output block and plaintext->input block
     String str ="";
    for (uint8_t i = 0; i < 16; i++)
    {
        str += (char)cypher[i];
    }
    return str;
}

unsigned long start = 0;
unsigned long end = 0;
unsigned long execution_time = 0;

String decrypt(byte *ciphertext)
{
  aes128.decryptBlock(decryptedtext1,cypher);
  String str ="";
  for (uint8_t i = 0; i < 16; i++)
  {
      str += (char)decryptedtext1[i];
  }
  return str;
}

void setup() {
  Serial.begin(9600);
  aes128.setKey(key,16);// Setting Key for AES
}

void loop() {
  // put your main code here, to run repeatedly:

   start = millis();
  // Getting sensor readings
  int altitude = getAltitude();
  int temperature = getTemperature();
  int voltage = getVoltage();

  //Generating datapacket
  String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
  String(voltage) + ",23:52:01,37.2314434,80.42644343,422.6434,7,0";
  byte plain[dataPacket.length()];
//  Serial.print("************************************");
//  Serial.print(packetCount);
//  Serial.println("************************************");
//  Serial.println(dataPacket);


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
  dataPacket.getBytes(plain, dataPacket.length());

  cipherText = "";
  
  // Converting the string into blocks of 16 bytes or 128 bits
  int len = (sizeof(plain)/sizeof(plain[0]));
//  Serial.print("Data Packet Length : ");
//  Serial.println(len);
  int numberOfBlocks = len/16;
  for(int i = 0; i < numberOfBlocks; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      plainText[j] = plain[(16*i) + j];
//      Serial.print(plainText[j]);
//      Serial.print(" , ");
//      Serial.println(dataPacket[(16*i) + j]);
    }
    String str ="";
    for (uint8_t i = 0; i < 16; i++)
    {
        str += (char)plainText[i];
    }
//    Serial.print("Block selected : ");
//    Serial.println(str);
//    Serial.print("CipherText : ");
//    Serial.println(encrypt(plainText));
//    Serial.print("Decrypted Text : ");
    String s = encrypt(plainText);
//    Serial.println();
//    Serial.print("Cipher Text : ");
//    Serial.println(s);
    byte dec[16];
    s.getBytes(dec,16);
//    Serial.println(decrypt(dec));
    cipherText += s;
//    Serial.print("Cipher Text Currently : ");
//    Serial.println(cipherText);
//    Serial.println("**************************************");
  }

  Serial.print("Cipher Text : ");
  Serial.print(cipherText);
  end = millis();
  execution_time = end - start;

  Serial.print("Encryption Execution Time : ");
  Serial.print(execution_time);
  Serial.println(" milliseconds");

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
    decryptedText = decryptedText + decrypt(cipherBytesBlock);
    Serial.println("---------------------------------");
  }
  

  Serial.print("Decryted Text : ");
  Serial.print(decryptedText);
  Serial.print(" , Length of Cipher Text");
  Serial.println(decryptedText.length());
  
  
  packetCount++;
  delay(10000);
}
