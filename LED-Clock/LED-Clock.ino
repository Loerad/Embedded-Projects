#include <TimeLib.h>

time_t clock = 0;

//two arrays H and M that have the relevant LEDs for hours and minutes
//one variable for am/pm state
// one var for boiler on and off state
//two methods that add an hour to the clock and one that adds 5 minutes to the clock




void setup() {
  // put your setup code here, to run once:
  setTime(clock);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(hour());
  Serial.print(minute());
  Serial.print(second());
  Serial.print(day());
  Serial.println(year());
} 
