#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

time_t clock = 0;

//two arrays H and M that have the relevant LEDs for hours and minutes

//one variable for am/pm state

// one var for boiler on and off state

//two methods that add an hour to the clock and one that adds 5 minutes to the clock


#define NUM_LEDS 41
#define LED_PIN 6
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB);
#define COLOR pixels.Color(0,100,0)
#define OFFCOLOR pixels.Color(0,0,0)
#define hourPin 3
#define minutePin 2



bool ampm = false; //am is false

//bool boilerOff = false; // led off on start, at 12pm will turn the boiler off which for now will be the inbuilt light 

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(minutePin), ChangeMinute, RISING);
  setTime(clock);
  Serial.begin(9600);
  pixels.begin();
  if (isAM() == 1)
  {
    pixels.setPixelColor(14, COLOR);
  }
  else
  {
    pixels.setPixelColor(14, OFFCOLOR);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(hourFormat12());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
  CheckSeconds();
  Serial.println(isAM());
} 

void CheckSeconds()
{
  time_t thisSecond = second();
  if (thisSecond == 0)
  {
    CheckMinutes();
  }
  int ledNum = 13;
  if (thisSecond % 2 == 0)
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

void CheckMinutes()
{
  time_t thisMinute = minute();
  if (thisMinute == 0)
  {
    CheckHours();
  }
  int ledNum = thisMinute / 5 + 16;
  pixels.setPixelColor(ledNum, COLOR);
  pixels.show();
}

void CheckHours()
{
  for (int i = 16; i < 28; i++)
  {
    pixels.setPixelColor(i, OFFCOLOR);
  }

  time_t thisHour = hourFormat12();
  if (thisHour == 0)
  {
    if (isAM() == 1)
    {
      pixels.setPixelColor(14, COLOR);
    }
    else
    {
      pixels.setPixelColor(14, OFFCOLOR);
    }


    for (int i = 0; i < 12; i++)
    {
      pixels.setPixelColor(i, OFFCOLOR);
    }

  }


  pixels.setPixelColor(thisHour - 1, COLOR);
  pixels.show();
} 

void ChangeMinute()
{
  time_t newMinute = 300; //5 min
  adjustTime(newMinute);
  CheckMinutes();
}
