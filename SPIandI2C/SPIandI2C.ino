#include <SPI.h>
#include "Wire.h"

#define ADS7828   0b1001000
#define COMMAND 0b10001100

char buf[100];
volatile byte pos;

volatile byte SPIcount = 0;

// Arduino version compatibility Pre-Compiler Directives
#if defined(ARDUINO) && ARDUINO >= 100   // Arduino v1.0 and newer
  #define I2C_WRITE Wire.write 
  #define I2C_READ Wire.read
#else                                   // Arduino Prior to v1.0 
  #define I2C_WRITE Wire.send 
  #define I2C_READ Wire.receive
#endif

void setup(){
  Wire.begin();                 //initialize comm over i2c
  pinMode(MISO,OUTPUT);
  SPCR |= _BV(SPE);
  //initialize buffer
  pos = 0;
  
  SPI.attachInterrupt();
  
  Serial.begin(57600);
  Serial.println("SeedPod Comm Test");
}


volatile byte data[] = {0x3, 0x0, 0x0, 0x0};
volatile uint8_t outCount = 0;

bool semaphore = true;
bool transmit = false;

byte one;
byte two;

void loop(){
  if(semaphore){
    semaphore=false;
    //launch conversion
    Wire.beginTransmission(ADS7828);
    I2C_WRITE(COMMAND);
    Wire.endTransmission();
   //read conversion
    Wire.requestFrom(ADS7828,2);
    data[1] = I2C_READ();
    data[2] = I2C_READ();
    Wire.endTransmission();
    uint16_t output = data[1]<<8 | data[2];
    
    data[3]=data[1]+data[2];
    semaphore=true;
  }
  if(outCount>4){
    outCount=0;
    semaphore=true;
    transmit=false;
  }
}


ISR(SPI_STC_vect)
{
  byte c = SPDR;
  if(transmit){
    SPDR = data[outCount++];
    semaphore=false;
  }
  else if(c==3){
    semaphore=false;
    transmit=true;
  }
  else
    SPDR = 5;
  /*
  if(c==0x3 && semaphore){
    Serial.println("send");
    semaphore  =  false;  //take semaphore
    transmit  =  true;    //but allow self to run
    SPDR = data[outCount++];
  }
  else if(transmit){
    SPDR = data[outCount++];  //transmit data
    if(outCount==4){         //if we've done all data let it go
      outCount=0;
      transmit  =false;
      semaphore = true;
    }
  }
  else
    SPDR = 1;
  */
  if(pos<sizeof buf)
  {
    buf[pos++]=c;  
  }
  
  
}
