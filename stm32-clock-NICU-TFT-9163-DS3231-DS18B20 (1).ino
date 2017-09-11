// original sketch from http://educ8s.tv/arduino-real-time-clock/
// Nicu FLORICA (niq_ro) from http://www.arduinotehniq.com 
// made some changes for animate display with blinking signs
// eliminate library for clock, use just subroutines
// use 3 buttons for manual adjust the data & clock
// add alarm (now usee eeprom memory for store hour alarm)
// adapted by Vlad Gheorghe for Arduino Mega and 3.95 display with ILI9844
// adapted by Nicu FLORICA for Due and same display

// icon made by Cristi Vasilache
static const PROGMEM uint8_t clopotel[32 * 24 / 8] = {
0X00,0X03,0XC0,0X00,0X00,0X07,0XE0,0X00,0X00,0X0F,0XF0,0X00,0X00,0X0F,0XF0,0X00,
0X00,0X0F,0XF0,0X00,0X20,0X38,0X1C,0X04,0X24,0X41,0X02,0X24,0X44,0X81,0X01,0X22,
0X48,0X81,0X01,0X12,0X49,0X01,0X00,0X92,0X49,0X00,0X80,0X92,0X89,0X00,0X80,0X91,
0X91,0X00,0X80,0X49,0X92,0X1F,0X80,0X49,0X91,0X00,0X00,0X89,0X49,0X00,0X00,0X92,
0X49,0X00,0X00,0X92,0X48,0X80,0X01,0X12,0X48,0X80,0X01,0X12,0X24,0X40,0X02,0X24,
0X20,0X30,0X0C,0X04,0X00,0X7E,0X3E,0X00,0X00,0X79,0XCE,0X00,0X00,0X70,0X0E,0X00};
// alarmOff
static const PROGMEM uint8_t faraclopotel[32 * 24 / 8] = {
0X00,0X03,0XC0,0X00,0X00,0X07,0XE0,0X00,0X80,0X0F,0XF0,0X01,0X60,0X0F,0XF0,0X06,
0X10,0X0F,0XF0,0X08,0X0C,0X38,0X1C,0X30,0X02,0X40,0X82,0X40,0X01,0XC0,0X81,0X80,
0X00,0XC0,0X83,0X00,0X01,0X30,0X8C,0X80,0X01,0X08,0X90,0X80,0X01,0X06,0XE0,0X80,
0X01,0X1F,0X80,0X40,0X01,0X03,0XC0,0X40,0X01,0X04,0X20,0X80,0X01,0X18,0X18,0X80,
0X01,0X20,0X04,0X80,0X00,0XC0,0X03,0X00,0X01,0X40,0X01,0X80,0X06,0X40,0X02,0X60,
0X08,0X30,0X0C,0X10,0X30,0X7E,0X7E,0X0C,0X40,0X79,0X8E,0X02,0X80,0X70,0X0E,0X01,
};

#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 22 on the Arduino Due
#define ONE_WIRE_BUS PB1
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


//#include <Adafruit_ST7735.h>
#include <avr/dtostrf.h> // for work "dtostrf" with Due https://forum.arduino.cc/index.php?topic=140695.0
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
//#include "EEPROM.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
//#include <Adafruit_ILI9341.h>
#include <TFT_ILI9163C.h>

//#define LCD_CS A3 // Chip Select goes to Analog 3
//#define LCD_CD A2 // Command/Data goes to Analog 2
//#define LCD_WR A1 // LCD Write goes to Analog 1
//#define LCD_RD A0 // LCD Read goes to Analog 0
//#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
//#include <MCUFRIEND_kbv.h>
//MCUFRIEND_kbv tft;
#define TFT_CS     PB13
#define TFT_RST    PB14                      
#define TFT_DC     PB12
//SCK   PA5
//SDI/MOSI    PA7
//SDA PB7 
//SCL PB6 
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS,  TFT_DC, TFT_RST);
TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS,  TFT_DC, TFT_RST);

//#define TFT_CS     9
//#define TFT_RST    7                      
//#define TFT_DC     8
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
//#define TFT_SCLK 13   // set these to be whatever pins you like!
//#define TFT_MOSI 11   // set these to be whatever pins you like!

float maxTemperature=0;
float minTemperature=200;
char charMinTemperature[10];
char charMaxTemperature[10];
char timeChar[100];
char dateChar[50];
char temperatureChar[10];

float temperature = 0;
float previousTemperature = 0;

String dateString;
int minuteNow=0;
int minutePrevious=0;


#define DS3231_I2C_ADDRESS 104
byte tMSB, tLSB;
float temp3231;
float temperatura, temperatura0;
byte hh, mm, ss;
byte yy, ll, dd, zz;
int meniu= PA0;
int minus=PA1;
int plus=PA4;
int alarma=PB0;

int v_meniu=4000;
int v_minus=4000;
int v_plus=4000;
int v_alarma=4000;

//#define meniu 23 // A8 
//#define minus 24 // A9
//#define plus  25 // A10
//#define alarm 26 // 22
#define buzzer PC13 // 23

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define DARKGREEN       0x03E0

int nivel = 0;   // if is 0 - clock
                 // if is 1 - hour adjust
                 // if is 2 - minute adjust

int hh1, mm1, zz1, dd1, ll1, yy1;   
int maxday;
boolean initial = 1;
char chartemp[3];
int hha, mma;   // for alarm
byte al;     // for alarm 
byte xa = 95;
byte ya = 90;

#define disk1 0x57    //Address of 24LC256 eeprom chip


void setup () 
{
   Serial.begin(9600);
   
     tft.begin();
    tft.setRotation(1);
    tft.fillScreen();
//uint16_t g_identifier;  
// uint32_t when = millis();
    
  //  if (!Serial) delay(5000);           //allow some time for Leonardo
  //  Serial.println("Serial took " + String((millis() - when)) + "ms to start");
  //  static uint16_t identifier;
   
  //  g_identifier = tft.readID(); //
  //  Serial.print("ID = 0x");
  //  Serial.println(g_identifier, HEX);
  //  if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) g_identifier = 0x9481; // write-only shield
  //  if (g_identifier == 0xFFFF) g_identifier = 0x9341; // serial

 //   tft.begin(0x9488); 
 
 //g_identifier = 0x9481;
// tft.begin(g_identifier); 
  //tft.initR(INITR_BLACKTAB);
   tft.setRotation(1);
    tft.fillScreen(BLACK);
   
    Wire.begin();
pinMode(PA0, INPUT_ANALOG); 
pinMode(PA1, INPUT_ANALOG); 
pinMode(PA4, INPUT_ANALOG);
pinMode(PB0, INPUT_ANALOG);

sensors.requestTemperatures(); // Send the command to get temperatures
temperature = sensors.getTempCByIndex(0); 
 Serial.println(sensors.getTempCByIndex(0));   

    
/*pinMode(meniu, INPUT); 
pinMode(plus, INPUT); 
pinMode(minus, INPUT); 
pinMode(alarm, INPUT); 
pinMode(buzzer, OUTPUT);
digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
digitalWrite(plus, HIGH);
digitalWrite(minus, HIGH);
digitalWrite(alarm, HIGH);
digitalWrite(buzzer, LOW);
*/

//pinMode(PA0, INPUT_ANALOG); 
//pinMode(PA1, INPUT_ANALOG); 
//pinMode(PA4, INPUT_ANALOG); 

    printText("Temperatura", GREEN,0,55,1);  // Temperature Static Text
    printText("Alarma setata", MAGENTA,80,55,1);
   // printText("Ceas cu alarma si termometru", DARKGREEN,0,115,1);
    //printText("termometru cu STM32", DARKGREEN,200,215,1);
     printText("@ Vlad Gheorghe", RED,70,119,1);
    printText("MAX", BLUE,10,90,1);
    printText("MIN", RED,55,90,1);
// setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
//setDS3231time(0, 51, 19, 2, 14, 02, 2017);

//  hha = 7;
//  mma = 00;
//  al = 1;  // 0 = alarm is off (must put in 1 foar active)

/*
// for Mega
hha = EEPROM.read(100);
mma = EEPROM.read(101);
al = EEPROM.read(102);
*/
// for Due 
hha = readEEPROM(disk1,100), DEC;
mma = readEEPROM(disk1,101), DEC;
al = readEEPROM(disk1,102), DEC;



if ((hha < 0) || (hha > 23)) hha = 7;
if ((mma < 0) || (mma > 59)) mma = 0;
if ((al < 0) || (al > 1)) al = 0;
}


void loop () 
{
//tft.drawBitmap(275, 70, alarm, 32, 24, 0xFCA0);
if (nivel == 0)
{
v_meniu=analogRead(meniu);
//Serial.println(v_meniu);
if ((v_meniu) < 100)
  {
  //nivel = nivel+1;
  nivel =99;
  hh1=hh;
  mm1=mm;
  zz1=zz;
  yy1=yy;
  ll1=ll;
  dd1=dd;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }



  
  readDS3231time(&ss, &mm, &hh, &zz, &dd, &ll,&yy);

if ((ss == 15) || (ss == 45 || initial))
{
//  float temperature = get3231Temp();  // internal temepraturere
  sensors.requestTemperatures(); // Send the command to get temperatures
 temperature = sensors.getTempCByIndex(0); // DS18B20 temperature
Serial.println(temperature);
}


    String halarma = "";
    if(hha<10)
    {
         halarma = halarma+" "+String(hha);
    }else  
    halarma = halarma+ String(hha);
    if(mma<10)
    {
        halarma = halarma+":0"+String(mma);
    }else
    {
        halarma = halarma+":"+String(mma);
    }
    halarma.toCharArray(timeChar,200);

//if (digitalRead(alarm) == LOW)
v_alarma=analogRead(alarma);
if ((v_alarma) < 100)
  {
  al = al + 1;
 // EEPROM.write(102, al%2);  // Mega
  writeEEPROM(disk1, 102, al%2);  // Due
  delay(500);
  tft.drawBitmap(xa-20, ya-15, faraclopotel, 32, 24, BLACK);  
  tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, BLACK);  
  }
if (mma == mm & hha == hh & al%2)
{
digitalWrite(buzzer, HIGH);
tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, GREEN);
tft.setFont(&FreeSans9pt7b);
   printText(timeChar, GREEN,xa+17,ya+5,1);tft.setFont();
}
else
{
if (al%2 == 0)
{
tft.drawBitmap(xa-20, ya-15, faraclopotel, 32, 24, RED);
    digitalWrite(buzzer, LOW);tft.setFont(&FreeSans9pt7b);
    printText(timeChar, RED,xa+17,ya+5,1);tft.setFont();
 
}
if (al%2 == 1)
{
//tft.drawBitmap(xa-20, ya-15, faraclopotel, 32, 24, BLACK);    
tft.drawBitmap(xa-20, ya-15, clopotel, 32, 24, WHITE);  
    digitalWrite(buzzer, LOW);tft.setFont(&FreeSans9pt7b);
    printText(timeChar, WHITE,xa+17,ya+5,1);tft.setFont();
}
}
 
  minuteNow = mm;
  if(minuteNow!=minutePrevious || initial)
  {
    initial = 0;
    dateString = getDayOfWeek(zz)+", ";
    dateString = dateString+String(dd)+"-"+month(ll);
    dateString= dateString+"-20"+ String(yy); 
    minutePrevious = minuteNow;
    String hours = "";
    if(hh<10)
    {
         hours = hours+"0"+String(hh);
    }else  
    hours = hours+ String(hh);
    if(mm<10)
    {
        hours = hours+" 0"+String(mm);
    }else
    {
        hours = hours+" "+String(mm);
    }
    
    hours.toCharArray(timeChar,100);
    tft.fillRect(0,0,160,53,BLACK);
  //  printText(timeChar, WHITE,20,25,3);lcd.setFreeFont(FSB24);
   tft.setFont(&FreeSansBold24pt7b);printText(timeChar, ORANGE,5,50,1);tft.setFont();
    dateString.toCharArray(dateChar,50);tft.setFont(&FreeSerif9pt7b);
    printText(dateChar, CYAN,0,12,1);tft.setFont();
  }
  
  if(temperature != previousTemperature)
  {
    previousTemperature = temperature;
    String temperatureString = String(temperature,1);
    temperatureString.toCharArray(temperatureChar,10);
    tft.fillRect(0,65,52,20,BLACK);
    printText(temperatureChar, GREEN,2,69,2);
    printText("o", WHITE,52,68,1);
    printText("C", WHITE,62,68,2);

    if(temperature>maxTemperature)
    {
      maxTemperature = temperature;
    }
      dtostrf(maxTemperature,5, 1, charMaxTemperature); 
      tft.fillRect(3,100,30,15,BLACK);
      printText(charMaxTemperature, WHITE,1,105,1);
      printText("o", WHITE,33,103,1);
      printText("C", WHITE,40,105,1);
    //}
    if(temperature<minTemperature)
    {
      minTemperature = temperature;
    }
      dtostrf(minTemperature,5, 1, charMinTemperature); 
      tft.fillRect(50,100,28,15,BLACK);
      printText(charMinTemperature, WHITE,48,105,1);
      printText("o", WHITE,79,103,1);
      printText("C", WHITE,86,105,1);
    //}
  }

delay(500);
//     printText("o", WHITE,62,28,1);
//     printText("o", WHITE,62,35,1);
   //  printText("o", ORANGE,225,75,4);
     tft.fillCircle(65,25,5,ORANGE);
     tft.fillCircle(65,45,5,ORANGE);
   //  printText("o", ORANGE,225,105,4);

delay(500);
    // printText("o", BLACK,225,75,4);
    // printText("o", BLACK,225,105,4);
    tft.fillCircle(65,25,4,BLACK);
     tft.fillCircle(65,45,4,BLACK);
} // end usual case (clock)

if (nivel == 99)   // clear display
{
  tft.fillScreen(BLACK);
//  tft.fillRect(0,0,128,160,BLACK);
nivel = 1;
  }    

if (nivel == 1)   // change hours
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
   delay(500);
  tft.fillScreen();
  tft.fillRect(0,0,160,240,BLACK);
  }    

// Temperature Static Text
printText("HOUR:", GREEN,10,36,2);  // Temperature Static Text
dtostrf(hh1,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(50,50,70,38,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
      
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    hh1 = hh1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    hh1 = hh1-1;
    delay(150);
    }    
if (hh1 > 23) hh1 = 0;
if (hh1 < 0) hh1 = 23;       
     
}  // end loop nivel = 1 (change the hours)


if (nivel == 2)   // change minutes
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MINUTE:", GREEN,10,35,2);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mm1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    mm1 = mm1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    mm1 = mm1-1;
    delay(150);
    }    
if (mm1 > 59) mm1 = 0;
if (mm1 < 0) mm1 = 59;       
     
}  // end loop nivel = 2 (change the minutes)

if (nivel == 3)   // change day in week
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(0,35,128,18,BLACK);
printText("Day in Week:", GREEN,10,40,1);  // Temperature Static Text
//Serial.println(mm1);
dtostrf(zz1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
tft.fillRect(10,80,100,18,BLACK);      
if (zz1 == 1) printText("1 - Luni", RED,10,80,1);  // Temperature Static Text
if (zz1 == 2) printText("2 - Marti", RED,10,80,1);  // Temperature Static Text
if (zz1 == 3) printText("3 - Miercuri", RED,10,80,1);  // Temperature Static Text
if (zz1 == 4) printText("4 - Joi", RED,10,80,1);  // Temperature Static Text
if (zz1 == 5) printText("5 - Vineri", RED,10,80,1);  // Temperature Static Text
if (zz1 == 6) printText("6 - Simbata", RED,10,80,1);  // Temperature Static Text
if (zz1 == 0) printText("0 - Duminica", RED,10,80,1);  // Temperature Static Text  
    delay(50);
     
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    zz1 = zz1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    zz1 = zz1-1;
    delay(150);
    }    
if (zz1 > 6) zz1 = 0;
if (zz1 < 0) zz1 = 6;       
     
}  // end loop nivel = 3 (change the day of the week)

if (nivel == 4)   // change year
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  tft.fillRect(10,80,100,18,BLACK);    
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("YEAR:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(yy1,3, 0, chartemp); 
      tft.fillRect(40,50,80,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText("20", WHITE,45,50,2);
      printText(chartemp, WHITE,60,50,2);
      delay(50);

     
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    yy1 = yy1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    yy1 = yy1-1;
    delay(150);
    }    
if (yy1 > 49) yy1 = 49;
if (yy1 < 16) yy1 = 16;       
     
}  // end loop stare = 4 (change the year)


if (nivel == 5)   // change mounth
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
   tft.fillRect(10,80,100,18,BLACK);    
  }    
printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MONTH:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(ll1,3, 0, chartemp); 
      tft.fillRect(40,50,80,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    ll1 = ll1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    ll1 = ll1-1;
    delay(150);
    }    
   
if (ll1 > 12) ll1 = 1;
if (ll1 < 1) ll1 = 12;       
     
}  // end loop stare = 5 (change the day as data)

if (nivel == 6)   // change day as data
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
 tft.fillRect(10,80,100,18,BLACK);      
  }    

printText("SETTING", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("DAY:", GREEN,10,35,2);  // Temperature Static Text
dtostrf(dd1,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(10,80,100,18,BLACK);  // erase last explication...
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
 
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    dd1 = dd1+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    dd1 = dd1-1;
    delay(150);
    
    }    
    
 // Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
 if (ll == 4 || ll == 5 || ll == 9 || ll == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (ll ==2 && yy % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (ll ==2 && ll % 4 !=0) {
    maxday = 28;
  }

if (dd1 > maxday) dd1 = 1;
if (dd1 < 1) dd1 = maxday;       
     
}  // end loop nivel = 6 (change the day as data)

if (nivel == 7)   // change hours alarm
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    
tft.fillRect(10,15,100,38,BLACK);
printText("ALARM", GREEN,10,18,2);  // Temperature Static Text
printText("HOUR:", GREEN,10,36,2);  // Temperature Static Text
dtostrf(hha,3, 0, chartemp); 
      //tft.fillRect(50,50,70,18,BLACK);
      tft.fillRect(50,50,70,38,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
      
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    hha = hha+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    hha = hha-1;
    delay(150);
    }    
if (hha > 23) hha = 0;
if (hha < 0) hha = 23;       
     
}  // end loop nivel = 7 (change the hours)

if (nivel == 8)   // change minutes for alarm
{
v_meniu=analogRead(meniu);
if ((v_meniu) < 100)
  {
  nivel = nivel+1;
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,128,160,BLACK);
  }    
tft.fillRect(10,15,70,18,BLACK);
printText("ALARM", GREEN,10,18,2);  // Temperature Static Text
tft.fillRect(10,35,70,18,BLACK);
printText("MINUTE:", GREEN,10,35,2);  // Temperature Static Text
Serial.println(mm1);
dtostrf(mma,3, 0, chartemp); 
      tft.fillRect(50,50,70,18,BLACK);
      delay(50);
      printText(chartemp, WHITE,60,50,2);
      delay(50);
     
//if (digitalRead(plus) == LOW)
v_plus=analogRead(plus);
if ((v_plus) < 100)
    {
    mma = mma+1;
    delay(150);
    }
//if (digitalRead(minus) == LOW)
v_minus=analogRead(minus);
if ((v_minus) < 100)
    {
    mma = mma-1;
    delay(150);
    }    
if (mma > 59) mma = 0;
if (mma < 0) mma = 59;       
  tft.fillScreen();   
}  // end loop nivel = 8 (change the minutes)


 if (nivel >=9)  // readfy to return to main loop
 {
  setDS3231time(0, mm1, hh1, zz1, dd1, ll1, yy1);
/*
// for Mega
  EEPROM.write(100, hha);
  EEPROM.write(101, mma);
  EEPROM.write(102, al%2);
*/  
// for Due
writeEEPROM(disk1, 100, hha);
writeEEPROM(disk1, 101, mma);
writeEEPROM(disk1, 102, al%2);

  nivel = 0;  
  previousTemperature= 0.0;
  initial = 1;
    printText("Temperatura", GREEN,0,55,1);  // Temperature Static Text
    printText("Alarma setata", MAGENTA,80,55,1);
  //  printText("Ceas cu alarma si termometru cu Arduino", DARKGREEN,225,295,1);
  //  printText("Due, DS18B20, DS3231 si afisaj de 10cm.", DARKGREEN,215,305,1);
    printText("@ Vlad Gheorghe", RED,70,119,1);
    printText("MAX", BLUE,10,90,1);
    printText("MIN", RED,55,90,1);
    //  float temperature = get3231Temp();  // internal temepraturere
  sensors.requestTemperatures(); // Send the command to get temperatures
  float temperature = sensors.getTempCByIndex(0); // DS18B20 temperature
Serial.println(temperature);

 }
}  // end main loop


void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

String getDayOfWeek(int i)
{
  switch(i)
  {
    case 0: return "Duminica";break;
    case 1: return "Luni";break;
    case 2: return "Marti";break;
    case 3: return "Miercuri";break;
    case 4: return "Joi";break;
    case 5: return "Vineri";break;
    case 6: return "Simbata";break;
    default: return "Monday";break;
  }
}

float get3231Temp() // from http://www.goodliffe.org.uk/arduino/rtc_ds3231.php
{
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
 
  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion
   
    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }
   
  return temp3231;
}

// http://forum.arduino.cc/index.php?topic=398891.0
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
    // sets time and date data to DS3231
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set next input to start at the seconds register
    Wire.write(decToBcd(second)); // set seconds
    Wire.write(decToBcd(minute)); // set minutes
    Wire.write(decToBcd(hour)); // set hours
    Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
    Wire.write(decToBcd(month)); // set month
    Wire.write(decToBcd(year)); // set year (0 to 99)
    Wire.endTransmission();
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
    return ( (val / 16 * 10) + (val % 16) );
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
    return ( (val / 10 * 16) + (val % 10) );
}

void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}
String month(int i)
{
  switch(i)
  {
    case 1: return "IAN";break;
    case 2: return "FEB";break;
    case 3: return "MAR";break;
    case 4: return "APR";break;
    case 5: return "MAI";break;
    case 6: return "IUN";break;
    case 7: return "IUL";break;
    case 8: return "AUG";break;
    case 9: return "SEP";break;
    case 10: return "OCT";break;
    case 11: return "NOV";break;
    case 12: return "DEC";break;
    default: return "****";break;
  }
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}

