#include <Crypto.h>
#include <CryptoLW.h>
#include <Speck.h>
#include <SpeckSmall.h>
#include <SpeckTiny.h>
#include <string.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


int packetCount = 0;

byte key[] = {0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
                    0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};

Speck speck;

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


void setup() {
  Serial.begin(9600);

  // Calibrating Sensors

  // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
  
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
   // Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
   // while (1);
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
}

byte buffer[16];

String encrypt(BlockCipher *cipher, byte *plainText, size_t keySize)
{ 
  crypto_feed_watchdog();

  cipher->setKey(key, keySize);
  cipher->encryptBlock(buffer, plainText);

  Serial.println("--------------Ciphers-----------------");
//  
  String str ="";
  for (uint8_t i = 0; i < 16; i++)
  {
      Serial.print(buffer[i]);
      Serial.print(" , ");
      Serial.println((char)buffer[i]);
      str += (char)buffer[i];
  }
    
    Serial.println(str);

    return str;
}

String decrypt(BlockCipher *cipher, byte *cipherText, size_t keySize)
{ 
   cipher->decryptBlock(buffer, cipherText);

   Serial.println("--------------DeCiphers-----------------");

   String str = "";
    for (uint8_t i = 0; i < 16; i++)
    {
        Serial.print(buffer[i]);
        Serial.print(" , ");
        Serial.println((char)buffer[i]);
        str += (char)buffer[i];
    }
    
    Serial.println(str);

    return str;
}

unsigned long start = 0;

unsigned long end = 0;

unsigned execution_time = 0;

void loop() {
//  String dataPacket = "22.6434,7,000000";
//  byte plain[dataPacket.length()];
//  dataPacket.getBytes(plain, dataPacket.length());
//  String cipher = encrypt(&speck, plain ,16);
//  Serial.print("Encrypted text : ");
//  Serial.println(cipher);
//  byte cipherBytes[cipher.length()];
//  cipher.getBytes(cipherBytes, cipher.length());
//  Serial.print("Decrypted text : ");
//  Serial.println(decrypt(&speck, cipherBytes ,16));
//  Serial.println("**************");
//  delay(10000);
   start = millis();

  // Getting sensor readings
  int altitude = random(200,210);;
  int temperature = random(30,35);
  int voltage = getVoltage();
  String Imu = getIMU();

  //Generating datapacket
//  String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
//  String(voltage) + ",23:52:01,37.2314434,80.42644343,422.6434,7,0";

    String dataPacket = "1007,23:52:01," + String(packetCount) + ",C,F,R," + String(altitude) + "," + String(temperature) + "," + 
  String(voltage) + Imu;
  
  Serial.print("************************************");
  Serial.print(packetCount);
  Serial.println("************************************");
  Serial.println(dataPacket);


  // *********************************************************************************************
  // Encryption Process
  Serial.print(dataPacket.length());
  Serial.print(" , DataPacketLength % 16 : ");
  Serial.println(dataPacket.length() % 16);
  if(dataPacket.length() % 16 != 0)
  {
    for(int i = 0; i < (dataPacket.length() % 16) - 1; i++)
    {
      dataPacket = dataPacket + "0";
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
  Serial.print("Number of blocks : ");
  Serial.println(numberOfBlocks);
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
  end = millis();

  Serial.print(" , Length of Cipher Text");
  Serial.println(cipherText.length());

  execution_time = end - start;

  Serial.print("Encryption Execution Time : ");
  Serial.print(execution_time);
  Serial.println(" milliseconds");

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
