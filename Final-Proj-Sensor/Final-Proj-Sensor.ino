// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

#include <DHT.h>
#include <Wire.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11
#define RELAY 7

DHT dht(DHTPIN, DHTTYPE);

float h = 0.0;
float t = 0.0;
 
void setup()
{
  pinMode(RELAY, OUTPUT);
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
    Serial.println("Error:");
    Serial.println("Unable to read sensor");
    return;
  }

  HandleData();
}

void HandleData()
{
  if (CheckFault(h, 70))
  {
    SendData(true);
    digitalWrite(RELAY, HIGH);
    return;
  }

  else if (CheckFault(t, 30))
  {
    SendData(true);
    digitalWrite(RELAY, HIGH);
    return;
  }

  digitalWrite(RELAY, LOW);

  SendData(false);
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
