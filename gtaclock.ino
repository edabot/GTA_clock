/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Enable one of these two #includes and comment out the other.
// Conditional #include doesn't work due to Arduino IDE shenanigans.
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
//#include <TinyWireM.h> // Enable this line if using Adafruit Trinket, Gemma, etc.

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment(); //initializing display

int startTime = 1008;         //setting the initial time on the clock
int elapsedTime;              //variable for elapsed time since start of sketch       

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);
  matrix.setBrightness(1);    //scale of 0 to 15
  pinMode(9, OUTPUT);         //piezo buzzer for alarm
  for (int i=2;i<8;i++){
    pinMode(i, OUTPUT);       //LEDs for stars
  }
}

void loop() {
  elapsedTime = getTimeNow(millis());  //getting the 4-digit elapsed time from start
  showTime(elapsedTime+startTime);     //displaying elapsed + start time
  //beep(200); 
  
if (millis()%1000 > 500) {
  digitalWrite(2, HIGH); 
}
else {
  digitalWrite(2, LOW); 
}
  //delay(1000);
}

//converting the change in millis time to change in 4-digit time
int getTimeNow(unsigned long timeRaw){
  int tempTime = floor(timeRaw / 1000);   //dropping it to seconds
  int tempHours = floor(tempTime/3600);   //getting the hours
  tempHours = tempHours % 12;             //modulo 12 on the hours
  int tempMinutes = floor((tempTime%3600)/60);  //getting the minutes
  int showTime = tempHours * 100 + tempMinutes; //combining hours and minutes
  return showTime;
}
void showTime(int fourDigitTime){
  //checking to see if minutes are over 60. If so, add 40 to turn it into an extra hour
  if (fourDigitTime%100 >= 60){
    fourDigitTime += 40;
  }
  //checking to see if hours are over 12 and correcting
  if (floor(fourDigitTime/100)>12) {
    fourDigitTime-=1200;
  }
  //printing the numbers
  matrix.print(fourDigitTime);
  //writing the colon in the middle
  matrix.writeDigitRaw(2, 0x02);
  matrix.writeDisplay();
}

void beep(unsigned char delayms){
  analogWrite(9, 20);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(9, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  
