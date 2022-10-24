#include <Arduino.h>

#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 12
//#define LED_PIN 13

void setup() 
{
  Serial.begin(9600);
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
  //pinMode(LED_PIN, OUTPUT);
}

void loop() 
{
  bool sw_pin = digitalRead(SW_PIN);
  /*if (sw_pin != 1)
  {
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }*/

  int x, y;
  x = analogRead(VRX_PIN);
  y = analogRead(VRY_PIN);
  int new_x, new_y;
  new_x = x - 512; // 0=517
  new_y = y - 512; // 0=522

  if ((new_x > -50) && (new_x < 50))
  {
    new_x = 0;
  }
  if ((new_y > -50) && (new_y < 50))
  {
    new_y = 0;
  }
  Serial.print("x = "); Serial.print(x, DEC);
  Serial.print(", y = "); Serial.print(y, DEC);
  Serial.print(", new_x = "); Serial.print(new_x, DEC);
  Serial.print(", new_y = "); Serial.println(new_y, DEC);
}
