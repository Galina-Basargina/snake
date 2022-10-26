#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

// ножки микроконтроллера
#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 12
// управление дисплея
#define __CS 10
#define __RST 9
#define __DC 8

// определение цветов
// .... .... .... ....
//              1 1111 : blue  0..31
//       111 111       : green 0..63
// 1111 1              : red   0..31
#define RGB(r,g,b) ((0xF800 & (r << 11)) | (0x07E0 & (g << 5)) | (0x001F & b))
#define WHITE      RGB(31, 63, 31)
#define	BLACK      RGB(0, 0, 0)
#define RED        RGB(31, 0, 0)
#define BLUE       RGB(0, 0, 31)
#define GREEN      RGB(0, 63, 0)
#define YELLOW     RGB(31, 63, 0)
#define CYAN       RGB(0, 63, 31)
#define MAGENTA    RGB(31, 0, 31)
#define ORANGE     GREEN //TODO: исправить цвет

TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, __RST);

enum Direction
{
  DIR_UP, 
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LEFT
};

Direction snake_direction = DIR_UP;  // по умолчанию Змейка ползет наверх


void drawSnake(int16_t x, int16_t y)
{
  int16_t vert = x + 3;
  int16_t horiz = y + 3;
  tft.fillRect(x, y, 7, 7, ORANGE);
  tft.drawLine(vert, y, vert, y+6, BLACK);
  tft.drawLine(x, horiz, x+6, horiz, BLACK);  
}

void clearSnake(int16_t x, int16_t y)
{
  tft.fillRect(x, y, 7, 7, BLACK);
}

void setup()
{
  Serial.begin(9600);
  // настроить работу с дисплеем
  tft.begin();
  // настроить работу с джостиком
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT);

  // закрасила поле черным цветом
  tft.fillRect(0, 0, 128, 128, BLACK);

  // временная Змейка
  drawSnake(8, 8);
  drawSnake(16, 8);
  drawSnake(16, 16);
  drawSnake(16, 24);
  drawSnake(24, 24);
}

void loop() 
{
  // нажатие на джостик
  //bool sw_pin = digitalRead(SW_PIN);

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
