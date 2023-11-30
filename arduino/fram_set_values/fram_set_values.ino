#include "Adafruit_FRAM_I2C.h"
   
Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

void setup(void) {
  Serial.begin(9600);
  
  if (fram.begin()) { 
    Serial.println("Found I2C FRAM");
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?");
    while (1);
  }
  
  // set the scale values

  int iw = 0;
  int iw2 = 0;
  uint16_t num = fram.writeObject(0x00, iw);
  Serial.print("Wrote an int with ");
  Serial.print(num);
  Serial.println(" bytes");
  num = fram.writeObject(0x00 + num, iw);
  Serial.print("Wrote an int with ");
  Serial.print(num);
  Serial.println(" bytes");

  int ir;
  int ir2;
  fram.readObject(0x00, ir);
  Serial.print("Read back int value: ");
  Serial.println(ir);
  fram.readObject(0x00, ir2);
  Serial.print("Read back int value: ");
  Serial.println(ir2);
}

void loop(void) {

}