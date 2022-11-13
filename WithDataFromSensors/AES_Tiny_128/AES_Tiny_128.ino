#include <Crypto.h>
#include <AES.h>
#include <string.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

int packetCount = 0;


Adafruit_MPU6050 mpu;


Adafruit_BMP280 bmp; // I2C Interface

int status;

/* Get new sensor events with the readings */
sensors_event_t a, g, t;
float getAltitude()
{
  float altitude = bmp.readAltitude(1019.66);
  return altitude;
}

float getTemperature()
{
  float temp = bmp.readTemperature();
  return temp;
}

int getVoltage()
{
  return random(6,10);
}

String getIMU()
{
  mpu.getEvent(&a, &g, &t);
  String data = "";

  data = String(a.acceleration.x) + " , " + String(a.acceleration.y) + String(a.acceleration.z) + " , " + 
         String((g.gyro.x)* (57296 / 1000)) + " , " +  String((g.gyro.y)* (57296 / 1000)) + " , " + String((g.gyro.z)* (57296 / 1000));        
  return data;
}


//
//key[16] cotain 16 byte key(128 bit) for encryption
byte key[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
//cypher[16] stores the encrypted text
byte cypher[16];
//decryptedtext[16] stores decrypted text after decryption
byte decryptedtext1[16];
//creating an object of AES128 class
AESTiny128 aes128;

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

  // Calibrating Sensors

  // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
  
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
  
  aes128.setKey(key,16);// Setting Key for AES
}

void loop() {
  // put your main code here, to run repeatedly:

   start = millis();
  // Getting sensor readings
  int altitude = getAltitude();
  int temperature = getTemperature();
  int voltage = getVoltage();

  String Imu = getIMU();

  //Generating datapacket
//  String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
//  String(voltage) + ",23:52:01,37.2314434,80.42644343,422.6434,7,0";

    String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
  String(voltage) + Imu;
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
