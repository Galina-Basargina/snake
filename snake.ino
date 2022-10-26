#include <Arduino.h>

#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 12
#define LED_PIN 13

void setup() 
{
  Serial.begin(9600);
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

enum Direction
{
  DIR_UP, 
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LEFT
};

Direction snake_direction = DIR_UP;  // по умолчанию Змейка ползет наверх

void loop() 
{
  bool sw_pin = digitalRead(SW_PIN);
  if (sw_pin != 1)
  {
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  int x, y;
  x = analogRead(VRX_PIN); // от 0 до 1023
  y = analogRead(VRY_PIN); // от 0 до 1023
  int new_x, new_y;
  new_x = x - 512; // от -512 до 511, где 0 х=517
  new_y = y - 512; // от -512 до 511, где 0 у=522

  if ((abs(new_x) > 300) && (abs(new_y) > 300))
  {
    // положение джостика по диагоналям не обрабатываем
  }
  else
  {
      if (new_x < -300)
      {
        snake_direction = DIR_RIGHT;
      }
      else if (new_x > 300)
      {
        snake_direction = DIR_LEFT;
      }
      else if (new_y < -300)
      {
        snake_direction = DIR_DOWN;
      }
      else if (new_y > 300)
      {
        snake_direction = DIR_UP;
      }
  }
  
  Serial.print("snake_direction = "); Serial.print(snake_direction, DEC);
  Serial.print(", x = "); Serial.print(x, DEC);
  Serial.print(", y = "); Serial.print(y, DEC);
  Serial.print(", new_x = "); Serial.print(new_x, DEC);
  Serial.print(", new_y = "); Serial.println(new_y, DEC);
}
