// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <TimeLib.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

float h = 0.0;
float t = 0.0;
unsigned long previousMillis = 0;  // will store last time LCD was updated
unsigned long previousStateMillis = 0;  // will store last time LCD was updated
const int interval = 2000;
const int stateInterval = 6000;
int pos = 0;    // variable to store the servo position
bool state = false; //false will show humidity, true temperature;

// https://projecthub.arduino.cc/arduino_uno_guy/i2c-liquid-crystal-displays-5eb615
// initialize the liquid crystal library
// the first parameter is  the I2C address
// the second parameter is how many rows are on your screen
// the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
 
void setup()
{
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(9600);
  myservo.attach(9);
  ReadData();
}

void loop()
{
  CheckTime();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    ReadData();
  }
  if (currentMillis - previousStateMillis >= stateInterval)
  {
    previousStateMillis = currentMillis;
    state = !state;
  } 

}

void CheckTime()
{
  lcd.setCursor(4, 1);
  lcd.print(hourFormat12());
  lcd.print(F(":"));
  if (minute() >= 0 && minute() <= 9)
  {
    lcd.print(F("0"));
  }
  lcd.print(minute());
  lcd.print(F(":"));
  if (second() >= 0 && second() <= 9)
  {
    lcd.print(F("0"));
  }
  lcd.print(second());
  if (isAM() == 1)
  {
    lcd.print(F(" am"));
  }
  else
  {
    lcd.print(F(" pm"));
  }
}

void ChangeMinute()
{
  time_t newMinute = 60; //1 min
  adjustTime(newMinute);
}

void ChangeHour()
{
  time_t newHour = 3600; //1 hour
  adjustTime(newHour);
}

void RunServo()
{
  for (pos = 0; pos <= 90; pos += 1) 
  { 
    myservo.write(pos);              
    delay(15);                       
  }
  
  for (pos = 90; pos >= 0; pos -= 1) 
  { 
    myservo.write(pos);            
    delay(15);   
  }                    
  
}

void ReadData()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Err read on sens");
    Serial.println("Error:");
    Serial.println("Unable to read sensor");
    return;
  }

  HandleData();
}

bool CheckFault(float reading, int cap)
{
  if (reading > cap)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void SendData(bool err)
{
  if (err)
  {
    Serial.print(1);
  }
  else
  {
    Serial.print(0);
  }
  Serial.print(", ");
  Serial.print(h);
  Serial.print(", ");
  Serial.println(t);
}

void DisplayDataToLCD()
{
  if (!state)
  {
    // humidity
    lcd.setCursor(0, 0);
    lcd.print(F("Humi: "));
    lcd.print(h);
    lcd.print("%");
  }
  else
  {
    // temp
    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    lcd.print(t);
    lcd.print(F("C"));
  }

}

void DisplayErrorToLCD(bool humid)
{
  if (humid) //check if the error is humidity
  {
    lcd.setCursor(0, 0);
    lcd.print(F("Humidity: "));
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Too humid      "); // extra spaces are to overwrite the previous information still on the LCD screen
  }
  else //assume its temp if not
  {
    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    lcd.print(t);
    lcd.print(F("C"));
    lcd.setCursor(0, 1);
    lcd.print("Too hot        ");
  }
  SendData(true);
}

/// This method checks the conditions around the sensor and will react accordingly if so and will trigger the relay if a valid fault is true
/// it will also print the information to the LCD screen as well as send the readings over the serial connection together every two seconds; humidity first, temperature second
/// if a fault occurs, the serial information will send the reason for the fault in place of the humidity, followed by the humidity or temperature that the sensor is reading
void HandleData()
{
  if (CheckFault(h, 70))
  {
    DisplayErrorToLCD(true);
    digitalWrite(RELAY, HIGH);
    return;
  }

  else if (CheckFault(t, 30))
  {
    DisplayErrorToLCD(false);
    digitalWrite(RELAY, HIGH);
    return;
  }

  digitalWrite(RELAY, LOW);

  DisplayDataToLCD();

  SendData(false);
}
