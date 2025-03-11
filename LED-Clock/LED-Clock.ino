#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define hourPin 3
#define minutePin 2

#define NUM_LEDS 41 //led strip needs to be at least 32 LEDS long or the same as the MAXMINUTELED value
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB);
#define COLOR pixels.Color(0,255,0)
#define OFFCOLOR pixels.Color(0,0,0)
#define AMCOLOR pixels.Color(255, 242, 0)
#define PMCOLOR pixels.Color(55, 0, 255)

#define SECONDLED 13
#define MINMINUTELED 20
#define MAXMINUTELED 32
#define MINSUBMINLED 15
#define MAXSUBMINLED 20
#define MINHOURLED 0
#define MAXHOURLED 12

//bool boilerOff = false; // led off on start, at 12pm will turn the boiler off which for now will be the inbuilt light 

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(minutePin), ChangeMinute, FALLING);
  attachInterrupt(digitalPinToInterrupt(hourPin), ChangeHour, FALLING);
  time_t clock = 0; //the TimeLib uses time_t values for each method but doesn't have any way to tell if a int is a time_t so this is needed
  setTime(clock);
  Serial.begin(9600); //for debug
  pixels.begin();
  CheckAMPM();  //the clock always starts on AM, this just makes sure its on for the first loop of 12 hours
  pixels.setBrightness(50);
}

void loop() {
  CheckSecond();
} 

void CheckSecond()
{
  time_t thisSecond = second();
  if (thisSecond == 0) //has the time gone from 59 to 60?
  {
    CheckSubMinute();
  }
  int ledNum = SECONDLED;
  if (thisSecond % 2 == 0) //blinks the second led for one second on then off
  {
    pixels.setPixelColor(ledNum, COLOR);
    pixels.show();
  }
  else
  {
    pixels.setPixelColor(ledNum, OFFCOLOR);
    pixels.show();
  }
}

void CheckMinute()
{
  time_t thisMinute = minute();
  if (thisMinute == 0)
  {
    CheckHour();
    Reset(MINMINUTELED, MAXMINUTELED);
  }
  //the way these leds work is by dividing the values by the amount of minutes/hours.
  //in this case, I divide 5 by the minute value then add the minimum starting LED so it appears in the strip where I want it.
  //for example: time is 45, divide that by 5 equals 9, then plus that by the min start LED which is 20 we end up turning on led 29
  int ledNum = thisMinute / 5 + MINMINUTELED;
  if (ledNum == MINMINUTELED) { return; } //stops the first led from being on all the time not showing any relevant information
  pixels.setPixelColor(ledNum, COLOR);
  pixels.show();
}

void CheckSubMinute()
{
  time_t thisSubMin = minute();
  if (thisSubMin % 5 == 0) //has the time gone from the 4th minute to the 5th minute of any set of 5 minutes?
  {
    CheckMinute();
    Reset(MINSUBMINLED, MAXSUBMINLED);
  }
  int ledNum = thisSubMin % 5 + MINSUBMINLED;
  if (ledNum == MINSUBMINLED) {return;}
  pixels.setPixelColor(ledNum, COLOR);
  pixels.show();
}

void CheckHour()
{
  time_t thisHour = hourFormat12();
  int ledNum = thisHour;
  if (thisHour == MAXHOURLED) //this is using 12 hour time 
  {
    Reset(MINHOURLED, MAXHOURLED); //call to reset hour leds
    ledNum = MINHOURLED;

    CheckAMPM();
  }  
  
  pixels.setPixelColor(ledNum, COLOR);
  pixels.show();
} 

void CheckAMPM()
{
  if (isAM() == 1) //for some reason isAM returns an int instead of a boolean 
  {
    pixels.setPixelColor(14, AMCOLOR);
  }
  else
  {
    pixels.setPixelColor(14, PMCOLOR);
  }
}

void Reset(int min, int max)
{
  for (int i = min; i < max; i++)
  {
    pixels.setPixelColor(i, OFFCOLOR);
  }
}

void ChangeMinute()
{
  time_t newMinute = 60; //1 min
  adjustTime(newMinute);
  CheckSubMinute();
}

void ChangeHour()
{
  time_t newHour = 3600; //1 hour
  adjustTime(newHour);
  CheckHour();
}
