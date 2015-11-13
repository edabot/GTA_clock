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
int currentTime;              //the time now       
int alarmLevel = 0;           //just how annoying and bad the alarm is. 0 is off
int alarmTime = 1010;         //when is this going off?
int snoozeTime = 1010;        //next time for alarm to go off
int alarmGap = 1;             //minutes between alarm levels/snoozes
int buttonCounter = 0;            //remaining pushes for the button for alarm
int wantedValues[] = {1,4,10,30,50,100};
                              //number of pushes needed for each alarm level
int alarmSpeeds[] = {0,1000,600,450,300,200,100};
                              //rate of flashing and buzzinf for each alarm level

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);
  matrix.setBrightness(1);    //scale of 0 to 15 for display brightness
  
  pinMode(9, OUTPUT);         //piezo buzzer for alarm on pin 9
  for (int i=2;i<8;i++){      //LEDs for stars on pins 2-7
    pinMode(i, OUTPUT);       
  }
}

void loop() {
  currentTime = getTimeNow(millis()) + startTime;  
        //getting the 4-digit elapsed time from start and adding start time
  showTime(currentTime);     //displaying elapsed + start time
  if (currentTime == alarmTime && alarmLevel==0){
    alarmLevel = 1;
  }

   if (alarmLevel>0){
    flashAndBuzz(alarmSpeeds[alarmLevel],alarmLevel);
  }
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



void flashAndBuzz (int speed, int stars) {
  if (millis()%speed > speed/2) {
    for (int i=2;i<=stars+1;i++){
      digitalWrite(i, HIGH); 
    }
    analogWrite(9, 20);
  }
  else {
    analogWrite(9, 0);  
    digitalWrite(2, LOW); 
  }
}

