//////////////////////////////////////////////////////////////////
//
// TinkerElectric.com
// Word Clock
// 
// This code uses the DS1307 Real Time Clock and presents the appropriate
// time in words. Original code by John Missikos and Damien Brombal (c) 2013
//
// Libraries used include:
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson, Adafruit
///////////////////////////////////////////////////////////////////



#include <EEPROM.h>
#include <Wire.h>
#include <Button.h>

#include <Adafruit_NeoPixel.h>

#define DS1307_ADDRESS 0x68

#define arraySize 8

#define timeButtonPin 3 //to increment the time

#define colourButtonPin 2 //to adjust the colour

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            4
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     110// 196


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500; // delay for half a second

byte zero = 0x00; //workaround for issue #527

//Pin Setup + Configuration
byte o_itis = 0;     //сейчас Output
byte o_minutes = 0;  //минут Output
byte o_oclock = 0;   //часов Output
byte o_to = 0;       //час Output
byte o_past = 0;     //часа Output

byte m_five = 0;     //5 Output
byte m_ten = 0;      //10 Output
byte m_quarter = 0;  //15 Output
byte m_twenty = 0;   //20 Output
byte m_half = 0;     //30 Output
byte m_forty = 0;    //40 Output
byte m_fifty = 0;    //50 Output

byte h_one = 0;      //1 Output
byte h_two = 0;      //2 Output
byte h_three = 0;    //3 Output
byte h_four = 0;     //4 Output
byte h_five = 0;     //5 Output
byte h_six = 0;      //6 Output
byte h_seven = 0;    //7 Output
byte h_eight = 0;    //8 Output
byte h_nine = 0;     //9 Output
byte h_ten = 0;      //10 Output
byte h_eleven = 0;   //11 Output
byte h_twelve = 0;   //12 Output


int intHour = 0;
int intMinute = 0;

int secondSet =   0;    //0-59
int minuteSet =   0;    //0-59
int hourSet =     0;    //0-23
int weekDaySet =  1;    //1-7
int monthDaySet = 1;    //1-31
int monthSet =    1;    //1-12
int yearSet  =    0;    //0-99

char junk = ' ';

int pixelAddress = 0;


#define coloursDefined  6 //Number of colors defined as we use to cycle later

//list colours here
uint32_t colours[] = {pixels.Color(0, 0, 255),
                pixels.Color(127, 127, 0),
                pixels.Color(127, 0, 127),
                pixels.Color(0, 127, 127),
                pixels.Color(255, 0, 0),
                pixels.Color(0, 255, 0)};



int eepromColorAddress = 0; //location to store the color when powered off


int memoryColour = 0; //the default color


//Set the colour globally
uint32_t colourOut;// = colours[memoryColour];


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Wire.begin();
  
    
  Serial.begin(9600);
  

 
  pinMode(timeButtonPin, INPUT);
  pinMode(colourButtonPin, INPUT);
  
  //Read back color if stored in EEPROM
  
  //EEPROM.write(eepromColorAddress,4);
  memoryColour = readColor(); //the colour currently stored
  
  colourOut = colours[memoryColour];
  
  
pixels.begin(); // This initializes the NeoPixel library.
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  
  intHour = getHour();
  intMinute = getMinute();

  
  Serial.print("Hour Value Off RTC:    ");
  Serial.println(intHour);
  Serial.print("Minute Value Off RTC:  ");
  Serial.println(intMinute);
  
  serialConvert(intHour,intMinute);
 
  displayTime();
  
  
  



  
  
  
  
  //temp test

  if (digitalRead(colourButtonPin) == HIGH) {
    colourButtonPressed();
    delay(500);
  }
  if (digitalRead(timeButtonPin) == HIGH) {
    timeButtonPressed();
    delay(50);
  }
  
}


//----------------------------------------------------------
//checkHourButton will look to see if button has been pressed on globally set pin 
void timeButtonPressed(){
  Serial.println("+++ Hour Button Pressed +++");
  hourSet =     getHour(); //get existing hour
  
  minuteSet =   getMinute()+1;  //0-59, the button increments the minute
  
  //When you are at 60 mins, increment the hour and reset the minutes
  if (minuteSet >= 60) {
    hourSet =     getHour()+1;      //0-23
    minuteSet = minuteSet - 60;
  }
  
  //When you are past 24 hours, start again
  if (hourSet >= 24) {
    hourSet = hourSet - 24;
  }
  
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator

  Wire.write(decToBcd(secondSet));
  Wire.write(decToBcd(minuteSet));
  Wire.write(decToBcd(hourSet));
  Wire.write(decToBcd(weekDaySet));
  Wire.write(decToBcd(monthDaySet));
  Wire.write(decToBcd(monthSet));
  Wire.write(decToBcd(yearSet));

  Wire.write(zero); //start 
  Wire.endTransmission();
}


//----------------------------------------------------------
//checkMinuteButton will look to see if button has been pressed on globally set pin 
void colourButtonPressed(){

  
  Serial.println("+++ Colour Button Pressed +++");  

  
      
      memoryColour = memoryColour + 1;
  
  if (memoryColour > (coloursDefined-1)) //ensure not out of bounds
  {
    
    memoryColour = 0;
    Serial.println("caught");
  
  }
 // Serial.println(memoryColour);  
 
// switch (memoryColour) {
//  case 0:
//    colourOut = pixels.Color(0, 0, 255);
//    break;
//  case 1:
//    colourOut = pixels.Color(0, 255,0);
//  break;
//    case 2:
//    colourOut = pixels.Color(255,0,0);
//  break;
//
//  default: 
//    memoryColour = 0;
//}
 

colourOut = colours[memoryColour];
  
  //store to eeprom
  updateColor(memoryColour);

}


//----------------------------------------------------------
//getHour pulls the hour integer from DS1307 Real Time Clock 
int getHour(){
  
  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Need to read the whole packet of information even though Hour is only needed
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> Sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());
  
  return hour;
}


//----------------------------------------------------------
//getMinute pulls the minute integer from DS1307 Real Time Clock
int getMinute(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Need to read the whole packet of information even though Minute is only needed
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> Sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());
  
  return minute;
}


//----------------------------------------------------------
byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}


//----------------------------------------------------------
byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}


//----------------------------------------------------------
//serialConvert expects...
//  - inputHour between the valueas of 0-23 (24 Hour)
//  - inputMinutes between the valueas of 0-59

//serialConvert will populate all the bytes used to display words
void serialConvert(int inputHour, int inputMinute){
  
  //Check to see if inputHour and inputMinute are within suitable range
  if (inputHour < 0 || inputHour > 23) {
   Serial.print("   inputHour is not suitable: ");
   Serial.println(inputHour);
   Serial.println("   Select a value between 0 and 23");
   Serial.println();
   return;
  }
  if (inputMinute < 0 || inputMinute > 59) {
   Serial.print("   inputMinute is not suitable: ");
   Serial.println(inputMinute);
   Serial.println("   Select a value between 0 and 59");
   Serial.println();
   return;
  }
  
  clearSerialArray();
  
  //IT IS is always on
  o_itis = 1;
  
  //Minute Serial Conversion
  if (inputMinute < 5) {            //??:00 --> ??:04
    o_oclock = 1;
  } 
  else if (inputMinute >= 30) {      //??:30 --> ??:34
    o_past = 1;
    if (inputMinute >= 35){         //??:35 --> ??:39
      m_five = 1;
    }
    else if (inputMinute >= 15 && inputMinute <20 ) { //??:15 --> ??:19
      m_quarter = 1;
    }
    else {
      o_minutes = 1;
      if (inputMinute >= 20){       //??:20 --> ??:29
        m_twenty = 1;
        if (inputMinute >= 25) {    //??:25 --> ??:29
          m_five = 1;
        }
      }
      else if (inputMinute >= 10){  //??:10 --> ??:14
        m_ten = 1;
      }
      else {                        //??:05 --> ??:09
        m_five = 1;
      }
      if (inputMinute >= 40){       //??:40 --> ??:49
        m_forty = 1;
        if (inputMinute >= 45) {    //??:45 --> ??:49
          m_five = 1;
        }
      }
      if (inputMinute >= 50){       //??:50 --> ??:59
        m_fifty = 1;
        if (inputMinute >= 55) {    //??:55 --> ??:59
          m_five = 1;
        }
      }
    }  
  }

  //Hour Serial Conversion
  if (inputHour >= 12) {
    inputHour = inputHour - 12;
    if (inputHour == 12) {
      inputHour = inputHour - 12;
    }
  }
  
  switch(inputHour){
    case 0:
      h_twelve = 1;
      break;
      
    case 1:
      h_one = 1;
      break;
    
    case 2:
      h_two = 1;
      break;
    
    case 3:
      h_three = 1;
      break;
 
    case 4:
      h_four = 1;
      break;

    case 5:
      h_five = 1;
      break;
  
    case 6:
      h_six = 1;
      break;
  
    case 7:
      h_seven = 1;
      break;

    case 8:
      h_eight = 1;
      break;
      
    case 9:
      h_nine = 1;
      break;
      
    case 10:
      h_ten = 1;
      break;
      
    case 11:
      h_eleven = 1;
      break;
  }
}


//----------------------------------------------------------
//clearSerialArray will zero all values in outputSerial[] 
void clearSerialArray(){
  
  //Pin Setup + Configuration
o_itis = 0;     //сейчас Output
o_minutes = 0;  //минут Output
o_oclock = 0;   //часов Output
o_to = 0;       //час Output
o_past = 0;     //часа Output

m_five = 0;     //5 Output
m_ten = 0;      //10 Output
m_quarter = 0;  //15 Output
m_twenty = 0;   //20 Output
m_half = 0;     //30 Output
m_forty = 0;    //40 Output
m_fifty = 0;    //50 Output

h_one = 0;      //1 Output
h_two = 0;      //2 Output
h_three = 0;    //3 Output
h_four = 0;     //4 Output
h_five = 0;     //5 Output
h_six = 0;      //6 Output
h_seven = 0;    //7 Output
h_eight = 0;    //8 Output
h_nine = 0;     //9 Output
h_ten = 0;      //10 Output
h_eleven = 0;   //11 Output
h_twelve = 0;   //12 Output
}


void displayTime(){
  //clearall
  clearAll();
  
  //list the time, then in it, list the pixels that are with i 
     if (o_itis == 1){     //сейчас Output
    
      pixels.setPixelColor(110, colourOut);
      pixels.setPixelColor(109, colourOut);
      pixels.setPixelColor(108, colourOut);
      pixels.setPixelColor(107, colourOut);
      pixels.setPixelColor(106, colourOut);
      pixels.setPixelColor(105, colourOut);
    
  }

  if (o_minutes == 1){  //минут Output
      if (NUMPIXELS != 196){ 
      //nothing, no minutes on small version
    }
    else{
    pixels.setPixelColor(5, colourOut);
    pixels.setPixelColor(4, colourOut);
    pixels.setPixelColor(3, colourOut);
    pixels.setPixelColor(2, colourOut);
    pixels.setPixelColor(1, colourOut);
    }
  }

  if (o_oclock == 1){   //часов Output
        pixels.setPixelColor(51, colourOut);
        pixels.setPixelColor(52, colourOut);
        pixels.setPixelColor(53, colourOut);
        pixels.setPixelColor(54, colourOut);
        pixels.setPixelColor(55, colourOut);
    }
  }

  if (o_to == 1){       //час Output
        pixels.setPixelColor(51, colourOut);
        pixels.setPixelColor(52, colourOut);
        pixels.setPixelColor(53, colourOut);
    }
  }

  if (o_past == 1){     //часа Output
        pixels.setPixelColor(34, colourOut);
        pixels.setPixelColor(35, colourOut);
        pixels.setPixelColor(36, colourOut);
        pixels.setPixelColor(37, colourOut);
    }
  }


  if (m_five == 1){     //5 Output
    pixels.setPixelColor(11, colourOut);
    pixels.setPixelColor(10, colourOut);
    pixels.setPixelColor(9, colourOut);
    pixels.setPixelColor(8, colourOut);
    }
  }

  if (m_ten == 1){      //10 Output
        pixels.setPixelColor(13, colourOut);
        pixels.setPixelColor(14, colourOut);
        pixels.setPixelColor(15, colourOut);
        pixels.setPixelColor(10, colourOut);
        pixels.setPixelColor(9, colourOut);
        pixels.setPixelColor(8, colourOut); 
    }
  }

  if (m_quarter == 1){ //15 Output
        pixels.setPixelColor(33, colourOut);
        pixels.setPixelColor(32, colourOut);
        pixels.setPixelColor(31, colourOut);
        pixels.setPixelColor(30, colourOut);
        pixels.setPixelColor(29, colourOut);
        pixels.setPixelColor(28, colourOut);
        pixels.setPixelColor(27, colourOut);
        pixels.setPixelColor(26, colourOut);
        pixels.setPixelColor(25, colourOut);
        pixels.setPixelColor(24, colourOut);
    }
  }  

  if (m_twenty == 1){   //20 Output
        pixels.setPixelColor(39, colourOut);
        pixels.setPixelColor(40, colourOut);
        pixels.setPixelColor(41, colourOut);
        pixels.setPixelColor(28, colourOut);
        pixels.setPixelColor(27, colourOut);
        pixels.setPixelColor(26, colourOut);
        pixels.setPixelColor(25, colourOut);
        pixels.setPixelColor(24, colourOut);
    }
  }

  if (m_half == 1){     //30 Output
        pixels.setPixelColor(42, colourOut);
        pixels.setPixelColor(43, colourOut);
        pixels.setPixelColor(44, colourOut);
        pixels.setPixelColor(28, colourOut);
        pixels.setPixelColor(27, colourOut);
        pixels.setPixelColor(26, colourOut);
        pixels.setPixelColor(25, colourOut);
        pixels.setPixelColor(24, colourOut);
    }
  }

  if (m_forty == 1)     //40 Output        !!!!!!!!!!!!!!!! если это закоментить, то все ок
        pixels.setPixelColor(42, colourOut);
        pixels.setPixelColor(43, colourOut);
        pixels.setPixelColor(44, colourOut);
        pixels.setPixelColor(28, colourOut);
        pixels.setPixelColor(27, colourOut);
        pixels.setPixelColor(26, colourOut);
        pixels.setPixelColor(25, colourOut);
        pixels.setPixelColor(24, colourOut); 
    }
  }
  if (m_half == 1){     //50 Output        !!!!!!!!!!!!!!!! если это закоментить, то все ок
        pixels.setPixelColor(33, colourOut);
        pixels.setPixelColor(32, colourOut);
        pixels.setPixelColor(31, colourOut);
        pixels.setPixelColor(12, colourOut);
        pixels.setPixelColor(13, colourOut);
        pixels.setPixelColor(14, colourOut);
        pixels.setPixelColor(15, colourOut);
        pixels.setPixelColor(16, colourOut);
        pixels.setPixelColor(17, colourOut);
    }
  }


  if (h_one == 1){      //1 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(78, colourOut);
        pixels.setPixelColor(79, colourOut);
        pixels.setPixelColor(80, colourOut);
        pixels.setPixelColor(81, colourOut);
    }
  }  

  if (h_two == 1){      //2 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(103, colourOut);
        pixels.setPixelColor(102, colourOut);
        pixels.setPixelColor(101, colourOut);
    }
  }  

  if (h_three == 1){    //3 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(67, colourOut);
        pixels.setPixelColor(68, colourOut);
        pixels.setPixelColor(69, colourOut);
    }
  }  

  if (h_four == 1){     //4 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(89, colourOut);
        pixels.setPixelColor(90, colourOut);
        pixels.setPixelColor(91, colourOut);
        pixels.setPixelColor(92, colourOut);
        pixels.setPixelColor(93, colourOut);
        pixels.setPixelColor(94, colourOut); 
    }
  }  

  if (h_five == 1){     //5 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(59, colourOut);
        pixels.setPixelColor(58, colourOut);
        pixels.setPixelColor(57, colourOut);
        pixels.setPixelColor(56, colourOut);
    }
  }  

  if (h_six == 1){     //6 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(45, colourOut);
        pixels.setPixelColor(46, colourOut);
        pixels.setPixelColor(47, colourOut);
        pixels.setPixelColor(48, colourOut);
        pixels.setPixelColor(49, colourOut); 
    }
  }  

  if (h_seven == 1){    //7 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(74, colourOut);
        pixels.setPixelColor(75, colourOut);
        pixels.setPixelColor(76, colourOut);
        pixels.setPixelColor(77, colourOut);
    }
  }  

  if (h_eight == 1){    //8 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(72, colourOut);
        pixels.setPixelColor(73, colourOut);
        pixels.setPixelColor(74, colourOut);
        pixels.setPixelColor(75, colourOut);
        pixels.setPixelColor(76, colourOut);
        pixels.setPixelColor(77, colourOut);
    }
  }  

  if (h_nine == 1){    //9 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(70, colourOut);
        pixels.setPixelColor(71, colourOut);
        pixels.setPixelColor(72, colourOut);
        pixels.setPixelColor(63, colourOut);
        pixels.setPixelColor(62, colourOut);
        pixels.setPixelColor(61, colourOut); 
    }
  }  

  if (h_ten == 1){      //10 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(70, colourOut);
        pixels.setPixelColor(71, colourOut);
        pixels.setPixelColor(64, colourOut);
        pixels.setPixelColor(63, colourOut);
        pixels.setPixelColor(62, colourOut);
        pixels.setPixelColor(61, colourOut); 
    }
  }  

  if (h_eleven == 1){   //11 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(88, colourOut);
        pixels.setPixelColor(87, colourOut);
        pixels.setPixelColor(86, colourOut);
        pixels.setPixelColor(85, colourOut);
        pixels.setPixelColor(84, colourOut);
        pixels.setPixelColor(83, colourOut);
        pixels.setPixelColor(82, colourOut);
        pixels.setPixelColor(81, colourOut);
        pixels.setPixelColor(80, colourOut);
        pixels.setPixelColor(79, colourOut);
        pixels.setPixelColor(78, colourOut);
    }
  }  

  if (h_twelve == 1){   //12 Output
    if (NUMPIXELS != 196){ 
        pixels.setPixelColor(95, colourOut);
        pixels.setPixelColor(96, colourOut);
        pixels.setPixelColor(97, colourOut);
        pixels.setPixelColor(84, colourOut);
        pixels.setPixelColor(83, colourOut);
        pixels.setPixelColor(82, colourOut);
        pixels.setPixelColor(81, colourOut);
        pixels.setPixelColor(80, colourOut);
        pixels.setPixelColor(79, colourOut);
        pixels.setPixelColor(78, colourOut);
    }
  } 
  
  
  

  
  
  //show them
   pixels.show(); // This sends the updated pixel color to the hardware.       




}


void clearAll(){
  
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, 0); // Moderately bright green color.
   // pixels.show(); // This sends the updated pixel color to the hardware.
    //delay(delayval); // Delay for a period of time (in milliseconds).
  }


}


int readColor(){
  int tempColor;
  tempColor = EEPROM.read(eepromColorAddress);
  //check if valid
  //if not, default 0
    if (tempColor > (coloursDefined-1) || tempColor < 0) //ensure not out of bounds
  {
    
    tempColor = 0;
    Serial.println("Fixed error in memory read");
  
  }
  
  
  
  
  return tempColor;
}

void updateColor(int colorInput){
  int tempColor;
  tempColor = EEPROM.read(eepromColorAddress);
  if (tempColor != colorInput){
    EEPROM.write(eepromColorAddress, colorInput);
  }
}