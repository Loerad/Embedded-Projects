#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <TimeLib.h>

//pins
#define DHTPIN 4 // Digital pin connected to the DHT sensor
#define MINBUTTON 10
#define HOURBUTTON 11
#define DHTTYPE DHT11 // DHT 11

//timers
#define interval 2000 //standard interval that reads data
#define stateInterval 6000 //changes the state of the screen to show 
#define buttonTimeout 500 //
unsigned long previousMillis = 0;      
unsigned long previousStateMillis = 0; 

//data and position
float h = 0.0;
float t = 0.0;
int pos = 30;  

//states 
bool state = false; // false will show humidity, true temperature;
int minButtonState = 0;
int hourButtonState = 0;
int prevMinButtonState = HIGH;
int prevHourButtonState = HIGH;

// https://projecthub.arduino.cc/arduino_uno_guy/i2c-liquid-crystal-displays-5eb615
// initialize the liquid crystal library
// the first parameter is  the I2C address
// the second parameter is how many rows are on your screen
// the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  lcd.init();
  lcd.backlight();

  dht.begin();
  servo.attach(9);

  pinMode(MINBUTTON, INPUT_PULLUP);
  pinMode(HOURBUTTON, INPUT_PULLUP);
  Serial.begin(9600);

  ReadData(); //does a pre-read to make sure the LCD isn't blank on start
}

void loop()
{
  CheckTime();
  CheckButtons();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) //DHT needs 2 seconds between reads
  {
    previousMillis = currentMillis;
    ReadData();
  }
  if (currentMillis - previousStateMillis >= stateInterval) //switches between showing temp and humi every 6 seconds
  {
    previousStateMillis = currentMillis;
    state = !state;
  }
}

/// @brief Reads the state of the buttons and calls the needed method based on button pressed
void CheckButtons()
{
  minButtonState = digitalRead(MINBUTTON);

  if (minButtonState != prevMinButtonState)
  {
    if (minButtonState == LOW)
    {
      ChangeMinute();
    }
  }
  hourButtonState = digitalRead(HOURBUTTON);
  if (hourButtonState != prevHourButtonState)
  {
    if (hourButtonState == LOW)
    {
      ChangeHour();
    }
  }
  prevMinButtonState = minButtonState;
  prevHourButtonState = hourButtonState;
}

/// @brief Checks the time and formats it for the LCD panel.
void CheckTime()
{
  lcd.setCursor(3, 1);
  if (hourFormat12() >= 0 && hourFormat12() <= 9)
  {
    lcd.print(F("0"));
  }
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

  if ((hour() >= 21) || (hour() < 12))
  {
    ServoToOn();
  }
  else
  {
    ServoToOff();
  }
}

/// @brief Adjusts the time by 1 minute forward
void ChangeMinute()
{
  time_t newMinute = 60; // 1 min
  adjustTime(newMinute);
}

/// @brief Adjusts the time by 1 hour forward
void ChangeHour()
{
  time_t newHour = 3600; // 1 hour
  adjustTime(newHour);
}

/// @brief Moves the servo forward to turn the switch on
void ServoToOn()
{
  for (pos; pos <= 60; pos += 1)
  {
    servo.write(pos);
  }
}

/// @brief Moves the servo backwards to turn the switch off
void ServoToOff()
{
  for (pos; pos >= 30; pos -= 1)
  {
    servo.write(pos);
  }
}

/// @brief Reads the data in from the sensor and catches if the sensor is incorrectly connected
void ReadData()
{
  lcd.clear();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    lcd.setCursor(0, 0);
    lcd.print("Err read on sens");
    Serial.println("Error:");
    Serial.println("Unable to read sensor");
    return;
  }

  HandleData();
}

/// @brief Checks if the incoming reading is over the supplied cap
/// @param reading the value given from the sensor
/// @param cap the limit you want to cap the reading's accepted values
/// @return true if the value is greater signaling a fault.
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

/// @brief Sends the data over the serial connection to be picked up by an external listener
/// @param err whether or not to send a fault is occuring
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

/// @brief Displays the received data to the LCD panel. Flips back and forth based on the state bool
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

/// @brief Displays the received fault (error) to the LCD panel
/// @param humid true if the fault is humidity, false for temperature
void DisplayErrorToLCD(bool humid)
{
  if (humid) // check if the error is humidity
  {
    lcd.setCursor(0, 0);
    lcd.print(F("ERR H: "));
    lcd.print(h);
    lcd.print("%");
  }
  else // assume its temp if not
  {
    lcd.setCursor(0, 0);
    lcd.print(F("ERR T: "));
    lcd.print(t);
    lcd.print(F("C"));
  }
  SendData(true);
}

/// @brief Handles which data to be displayed
void HandleData()
{
  if (CheckFault(h, 70))
  {
    DisplayErrorToLCD(true);
    return;
  }

  else if (CheckFault(t, 30))
  {
    DisplayErrorToLCD(false);
    return;
  }

  DisplayDataToLCD();

  SendData(false);
}
