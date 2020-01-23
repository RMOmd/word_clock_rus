//////////////////////////////////////////////////////////////////
//
// таблица символов:
// с   е  й  ч  а  с  б  д  в  а  о
// ч   е  т  ы  р  е  д  в  е  о  л
// о   д  и  н  н  а  д  ц  а  т  о
// т   р  и  д  е  в  о  с  е  м  ь
// д   е  с  я  т  ь  ф  п  я  т  ь
// ш   е  с  т  ь  о  ч  а  с  о  в
// ч   а  с  а  я  д  в  а  т  р  и
// п   я  т  н  а  д  ц  а  т  ь  л
// ь   д  е  с  я  т  с  о  р  о  к
// п   я  т  ь  в  ф  м  и  н  у  т
 
// нумерация диодов:
// 100  101 102 103 104 105 106 107 108 109 110
// 99   98  97  96  95  94  93  92  91  90  89
// 78   79  80  81  82  83  84  85  86  87  88
// 77   76  75  74  73  72  71  70  69  68  67
// 56   57  58  59  60  61  62  63  64  65  66
// 55   54  53  52  51  50  49  48  47  46  45
// 34   35  36  37  38  39  40  41  42  43  44
// 33   32  31  30  29  28  27  26  25  24  23
// 12   13  14  15  16  17  18  19  20  21  22
// 11   10  9     8   7   6   5   4   3   2   1
///////////////////////////////////////////////////////////////////
 
 
 
#include <EEPROM.h>
#include <Wire.h>
#include <Button.h>
 
#include <Adafruit_NeoPixel.h>
 
#define DS1307_ADDRESS 0x68
 
#define arraySize 8
 
#define timeButtonPin 3 // изменить время
 
#define colourButtonPin 2 // настроить цвет
 
// на какой пин Arduino подключен NeoPixels?
#define PIN            4
// Сколь диодов NeoPixels подключено к Arduino?
#define NUMPIXELS     110//
 
 
// Когда мы настраиваем библиотеку NeoPixel, мы сообщаем ей, сколько пикселей и какой вывод использовать для отправки сигналов.
// Обратите внимание, что для старых полос NeoPixel вам может потребоваться изменить третий параметр - см. strandtest
// пример для получения дополнительной информации о возможных значениях.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500; // пауза полсекукнды
 
byte zero = 0x00; //исправление проблему #527  (хз чо это... скопипаситл с куском кода, думаю это нужно)
 
//Настройка пинов и конфигрурация
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
 
 
#define coloursDefined  6 //Количество цветов, определенных для последующего цикла
 
//список цветов
uint32_t colours[] = {pixels.Color(0, 0, 255),
                pixels.Color(127, 127, 0),
                pixels.Color(127, 0, 127),
                pixels.Color(0, 127, 127),
                pixels.Color(255, 0, 0),
                pixels.Color(0, 255, 0)};
 
 
 
int eepromColorAddress = 0; //место для сохранения цвета при выключении
 
 
int memoryColour = 0; //цвет по-умолчанию
 
 
//Установка цвет глобально
uint32_t colourOut;// = colours[memoryColour];
 
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Wire.begin();
 
   
  Serial.begin(9600);
 
 
 
  pinMode(timeButtonPin, INPUT);
  pinMode(colourButtonPin, INPUT);
 
  //Читаем цвет из EEPROM
 
  //EEPROM.write(eepromColorAddress,4);
  memoryColour = readColor(); //текущий цвет
 
  colourOut = colours[memoryColour];
 
 
pixels.begin(); // Инициализация библиотеки NeoPixel.
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
//checkHourButton проверит, была ли нажата кнопка на пине
void timeButtonPressed(){
  Serial.println("+++ Hour Button Pressed +++");
  hourSet =     getHour(); //получаем текущий час
 
  minuteSet =   getMinute()+1;  //0-59, кнопка увеличивает минуты
 
  //Когда достигает 60, увеличивает час и сбрасывает минуты
  if (minuteSet >= 60) {
    hourSet =     getHour()+1;      //0-23
    minuteSet = minuteSet - 60;
  }
 
  //По достижении 24 часов - начинает заново
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
 
  if (memoryColour > (coloursDefined-1)) //убедимся, что не выходим за пределы
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
 
  //сохранить в eeprom
  updateColor(memoryColour);
 
}
 
 
//----------------------------------------------------------
//getHour извлекает целое число часов из часов реального времени DS1307
int getHour(){
 
  // Сброс указателя регистра
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Нужно прочитать весь пакет информации, хотя час нужен только
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 -часовой формат
  int weekDay = bcdToDec(Wire.read()); //0-6 -> Воскресенье - суббота
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());
 
  return hour;
}
 
 
//----------------------------------------------------------
//getMinute извлекает минутное целое число из часов реального времени DS1307
int getMinute(){
 
  // Сброс указателя регистра
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Нужно прочитать весь пакет информации, хотя минута нужна только
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 -часовой формат
  int weekDay = bcdToDec(Wire.read()); //0-6 -> Воскресенье - суббота
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());
 
  return minute;
}
 
 
//----------------------------------------------------------
byte decToBcd(byte val){
// Преобразование нормальных десятичных чисел в двоично-десятичное
  return ( (val/10*16) + (val%10) );
}
 
 
//----------------------------------------------------------
byte bcdToDec(byte val)  {
// Преобразование двоично-десятичного числа в нормальное десятичное число
  return ( (val/16*10) + (val%16) );
}
 
 
//----------------------------------------------------------
//serialConvert expects...
//  - inputHour between the valueas of 0-23 (24 Hour)
//  - inputMinutes between the valueas of 0-59
 
//serialConvert заполнит все байты, используемые для отображения слов
void serialConvert(int inputHour, int inputMinute){
 
  //Проверяем, находятся ли inputHour и inputMinute в подходящем диапазоне
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
 
  //СЕЙЧАС всегда вкл
  o_itis = 1;
 
  //Преобразование минут
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
 
  //Преобразование часов
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
//clearSerialArray обнулит все значения в outputSerial []
void clearSerialArray(){
 
  //Настройка пинов и конфигрурация
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
  //очистить
  clearAll();
 
  //перечислить время, затем в нем, перечислить пиксели
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
 
  if (o_to == 1){       //час Output
        pixels.setPixelColor(51, colourOut);
        pixels.setPixelColor(52, colourOut);
        pixels.setPixelColor(53, colourOut);
    }
 
  if (o_past == 1){     //часа Output
        pixels.setPixelColor(34, colourOut);
        pixels.setPixelColor(35, colourOut);
        pixels.setPixelColor(36, colourOut);
        pixels.setPixelColor(37, colourOut);
    }
 
  if (m_five == 1){     //5 Output
    pixels.setPixelColor(11, colourOut);
    pixels.setPixelColor(10, colourOut);
    pixels.setPixelColor(9, colourOut);
    pixels.setPixelColor(8, colourOut);
    }
 
  if (m_ten == 1){      //10 Output
        pixels.setPixelColor(13, colourOut);
        pixels.setPixelColor(14, colourOut);
        pixels.setPixelColor(15, colourOut);
        pixels.setPixelColor(10, colourOut);
        pixels.setPixelColor(9, colourOut);
        pixels.setPixelColor(8, colourOut);
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
 
  if (m_forty == 1){     //40 Output        
        pixels.setPixelColor(42, colourOut);
        pixels.setPixelColor(43, colourOut);
        pixels.setPixelColor(44, colourOut);
        pixels.setPixelColor(28, colourOut);
        pixels.setPixelColor(27, colourOut);
        pixels.setPixelColor(26, colourOut);
        pixels.setPixelColor(25, colourOut);
        pixels.setPixelColor(24, colourOut);
  }
  if (m_half == 1){     //50 Output        
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
 
 
 
 
 
 
  //показать
   pixels.show(); // Это отправляет обновленный цвет пикселя на железо.      
 
 
 
 
}
 
 
void clearAll(){
 
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color принимает значения RGB, от 0,0,0 до 255,255,255
    pixels.setPixelColor(i, 0); // Умеренно ярко-зеленый цвет.
   // pixels.show(); // This sends the updated pixel color to the hardware.
    //delay(delayval); // Delay for a period of time (in milliseconds).
  }
 
 
}
 
 
int readColor(){
  int tempColor;
  tempColor = EEPROM.read(eepromColorAddress);
  //проверяет верно ли
  //если нет, то default 0
    if (tempColor > (coloursDefined-1) || tempColor < 0) //не выходить за пределы
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
