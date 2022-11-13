# LightWeight_Cryptographic_Algorithms
Lightweight Cryptographic Algorithm implementation on Arduino and ESP32 with BMP280, MPU6050 for real time telemetry in satellite communication

## Sensors Used for replicating a flight computer's telemetry system : 
- BMP280 : Pressure Sensor
- MPU6050 : IMU

# Arduino Uno 
## Microcontroller: ATmega328P , Clock Speed: 16 MHz , Flash Memory: 32 KB (ATmega328P) of which 0.5 KB used by bootloader , SRAM: 2 KB (ATmega328P) EEPROM: 1 KB (ATmega328P) : 
### Encryption Time taken : 
- AES128 -> 22ms
- AES192 -> 22ms
- AES256 -> 22ms
- AESSmall 128 -> 22ms
- AESSmall 256 -> 22ms
- AESTiny 128 -> 22ms
- AESTiny 256 -> 22ms
- Speck128 -> 37ms
- Speck192 -> 37ms
- Speck 256 -> 37ms
- SpeckSmall 128 -> 39ms
- SpeckSmall 192 -> 40ms
- SpeckSmall 256 -> 40ms
- SpeckTiny 128 -> 37ms
- SpeckTiny 192 -> 37ms
- SpeckTiny 256 -> 37ms

# ESP32S
## CPU: Xtensa dual-core (or single-core) 32-bit LX6 microprocessor, operating at 160 or 240 MHz and performing at up to 600 DMIPS , Memory: 320 KiB RAM, 448 KiB ROM .  : 
### Encryption Time taken : 
- AES128 -> <=1ms
- AES192 -> <=1ms
- AES256 -> <=1ms
- AESSmall 128 -> <=1ms
- AESSmall 256 -> <=1ms
- AESTiny 128 -> <=1ms
- AESTiny 256 -> <=1ms
- Speck128 -> <=1ms
- Speck192 -> <=1ms
- Speck 256 -> <=1ms
- SpeckSmall 128 -> <=1ms
- SpeckSmall 192 -> <=1ms
- SpeckSmall 256 -> <=1ms
- SpeckTiny 128 -> <=1ms
- SpeckTiny 192 -> <=1ms
- SpeckTiny 256 -> <=1ms

