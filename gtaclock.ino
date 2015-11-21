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

int startTime = 1009;         //setting the initial time on the clock
int currentTime;              //the time now       
int alarmLevel = 0;           //just how annoying and bad the alarm is. 0 is off
int alarmTime = 1009;         //when is this going off?
int snoozeTime = 1009;        //next time for alarm to go off
int alarmGap = 1;             //minutes between alarm levels/snoozes
int buttonCounter = 0;            //remaining pushes for the button for alarm
int wantedValues[] = {1,4,10,25,50,100};
                              //number of pushes needed for each alarm level
int alarmSpeeds[] = {0,1000,600,450,300,200,100};
                              //rate of flashing and buzzinf for each alarm level
int alarmValues[] = {1,4,10,25,50,100};
                              //number of pushes needed for each alarm level
const int buttonPin = 8;      // the number of the pushbutton pin
int buttonState = 0;          
int lastButtonState = 0;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 10;    // the debounce time; increase if the output flickers


void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);
  matrix.setBrightness(1);    //scale of 0 to 15 for display brightness
  
  
  for (int i=2;i<8;i++){      //LEDs for stars on pins 2-7
    pinMode(i, OUTPUT);       
  }
  pinMode(buttonPin, INPUT);  //button on pin 8
  pinMode(9, OUTPUT);         //piezo buzzer for alarm on pin 9
}

void loop() {
  Serial.println(buttonCounter);
  Serial.println(alarmLevel);
  
  currentTime = getTimeNow(millis()*5) + startTime;  
        //getting the 4-digit elapsed time from start and adding start time
  showTime(currentTime);     //displaying elapsed + start time

  //alarm going off first time
  if (currentTime == alarmTime && alarmLevel==0){
    alarmLevel = 1;
    snoozeTime = alarmTime;
    buttonCounter = wantedValues[alarmLevel-1];
  }

  //alarm going off times 2-6
  if (currentTime == snoozeTime && buttonCounter == 0){
    buttonCounter = wantedValues[alarmLevel-1];
  }

  if (buttonCounter>0){
    flashAndBuzz(alarmSpeeds[alarmLevel],alarmLevel);
  }

  //resetting alarmLevel once series is finished


//reading the button and correcting for debounce
 int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {

    // reset the debouncing timer
    lastDebounceTime = millis();
  }

// if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      //things change if it's the last button press
      if (buttonState==1 && buttonCounter==1){
        buttonCounter--;          //sets button Counter to 0
        alarmLevel++;             //increases alarmLevel
        snoozeTime += alarmGap;   //changes snoozeTime
        analogWrite(9, 0);  
        for (int i=2;i<=7;i++){
          digitalWrite(i, LOW); 
        }
      }
      //just change buttonCounter if it's >1
      else if (buttonState==1 && buttonCounter>1){
        buttonCounter--;          //decrements button counter
      }      
    }

  //resetting evrything back to start once series is complete  
  if (alarmLevel==7) {
    alarmLevel=0;
    snoozeTime=alarmTime;
  }  
delay(10);
/*  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      //things change if it's the last button press
      if (buttonState==1 && buttonCounter==1){
        buttonCounter--;          //sets button Counter to 0
        alarmLevel++;             //increases alarmLevel
        snoozeTime += alarmGap;   //changes snoozeTime
        analogWrite(9, 0);  
        digitalWrite(2, LOW); 
      }
      //just change buttonCounter if it's >1
      else if (buttonState==1 && buttonCounter>1){
        buttonCounter--;          //decrements button counter
      }      
    }
 
  }
*/    
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

//displays the time on the LED display
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
    analogWrite(9, alarmValues[alarmLevel-1]);
  }
  else {
    analogWrite(9, 0);  
    for (int i=2;i<=stars+1;i++){
         digitalWrite(i, LOW); 
    }
  }

}

