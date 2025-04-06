// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11
#define RELAY 7

DHT dht(DHTPIN, DHTTYPE);

float h = 0.0;
float t = 0.0;

// https://projecthub.arduino.cc/arduino_uno_guy/i2c-liquid-crystal-displays-5eb615
// initialize the liquid crystal library
// the first parameter is  the I2C address
// the second parameter is how many rows are on your screen
// the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  pinMode(RELAY, OUTPUT);
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(9600);
}

void loop()
{
  // Wait a few seconds between measurements.
  delay(2000);
  ReadData();
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
  // humidity
  lcd.setCursor(0, 0);
  lcd.print(F("Humidity: "));
  lcd.print(h);
  lcd.print("%");
  
  // temp
  lcd.setCursor(0, 1);
  lcd.print(F("Temp: "));
  lcd.print(t);
  lcd.print(F("C"));
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
