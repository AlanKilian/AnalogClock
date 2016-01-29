
/* 
 DESCRIPTION
 ====================
 Simple example of the Bounce library that switches the debug LED when 
 either of 2 buttons are pressed.
 */
 
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>
#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
char decToBcd(char val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
char bcdToDec(char val)
{
  return( (val/16*10) + (val%16) );
}

#define BUTTON_HOUR 2
#define BUTTON_MINUTE 4
#define BUTTON_SECOND 3
#define BUTTON_UP 5
#define BUTTON_DOWN 6

#define HOURMETER 9
#define MINUTEMETER 10
#define SECONDMETER 11

// Calibration data. Min and Max analogWrite() values for each meter.
#define HMIN 8.0
#define MMIN 4.0
#define SMIN 4.0
#define HMAX 246.0
#define MMAX 240.0
#define SMAX 255.0

// Instantiate a Bounce object
Bounce Hour = Bounce(); 
Bounce Minute = Bounce();
Bounce Second = Bounce();
Bounce Up = Bounce();
Bounce Down = Bounce();

unsigned long nextmillis;

void setDS3231time(unsigned char second, unsigned char minute, unsigned char hour, byte dayOfWeek, byte
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
void readDS3231time(unsigned char *second,unsigned char *minute,unsigned char *hour)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
  // request three bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
}

void displayTime()
{
  unsigned char hour, minute, second;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour);
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.println(second, DEC);

}
void setup() {
  Serial.begin(9600);

  // Setup the first button with an internal pull-up :
  pinMode(BUTTON_HOUR,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  Hour.attach(BUTTON_HOUR);
  Hour.interval(5); // interval in ms
  
   // Setup the second button with an internal pull-up :
  pinMode(BUTTON_MINUTE,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  Minute.attach(BUTTON_MINUTE);
  Minute.interval(5); // interval in ms
    
  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_SECOND,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  Second.attach(BUTTON_SECOND);
  Second.interval(5); // interval in ms
    
  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_UP,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  Up.attach(BUTTON_UP);
  Up.interval(5); // interval in ms
    
  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_DOWN,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  Down.attach(BUTTON_DOWN);
  Down.interval(5); // interval in ms
  
  nextmillis = millis()+1000;
  Wire.begin();
  /* setDS3231time(0,0,18, 0,0,0,0); */
}

//unsigned char hh;
void loop() {
  unsigned char hh;
  unsigned char ss;
  unsigned char mm;
  float avalue;
  
  // Update the button debouncers
  Hour.update();
  Minute.update();
  Second.update();
  Up.update();
  Down.update();

  if(millis() > nextmillis) {
    //displayTime();
    readDS3231time(&ss, &mm, &hh);
    
    hh %= 12;
    avalue = (SMAX-SMIN)/60.0;
    avalue *= (float)ss;
    avalue += SMIN;
    analogWrite(SECONDMETER, avalue);
    
    avalue = (MMAX-MMIN)/60.0;
    avalue *= (float)mm;
    avalue += MMIN;
    analogWrite(MINUTEMETER, avalue);
    
    avalue = (HMAX-HMIN)/12.0;
    avalue *= (float)hh + (float)mm/60;
    avalue += HMIN;
    analogWrite(HOURMETER, avalue);

    // Get the updated button values
    int h = Hour.read();
    int m = Minute.read();
    int s = Second.read();
    int u = Up.read();
    int d = Down.read();
    nextmillis = millis()+100;
    
    if(0 == h) {
      if(0 == u) {
         readDS3231time(&ss, &mm, &hh);
         hh++;
         if(hh > 255) hh = 255;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
      if(0 == d) {
         readDS3231time(&ss, &mm, &hh);
         hh--;
         if(hh < 0) hh = 0;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
    }
    //analogWrite(HOURMETER, hh);
    //analogWrite(MINUTEMETER, hh);
    //analogWrite(SECONDMETER, hh);
    //Serial.println(hh,DEC);
    //return;
    
    if(0 == m) {
      if(0 == u) {
         readDS3231time(&ss, &mm, &hh);
         mm++;
         if(mm > 59) mm = 59;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
      if(0 == d) {
        readDS3231time(&ss, &mm, &hh);
         mm--;
         if(mm < 0) mm = 0;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
    }
    
    if(0 == s) {
      if(0 == u) {
         readDS3231time(&ss, &mm, &hh);
         ss++;
         if(ss > 59) ss = 59;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
      if(0 == d) {
        readDS3231time(&ss, &mm, &hh);
         ss--;
         if(ss < 0) ss = 0;
         setDS3231time(ss, mm, hh, 0,0,0,0);
      }
    }
  }
}


