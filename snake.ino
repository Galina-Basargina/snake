#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

// ножки микроконтроллера
#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 12
// не подключенная ножка
#define RANDOM_PIN A2
// управление дисплея
#define __CS 10
#define __RST 9
#define __DC 8

// определение цветов
// .... .... .... ....
//              1 1111 : blue  0..31
//       111 111       : green 0..63
// 1111 1              : red   0..31
// .... .... .... ....
// смотри также http://greekgeeks.net/#maker-tools_convertColor
#define RGB(r,g,b) ((0xF800 & (r << 11)) | (0x07E0 & (g << 5)) | (0x001F & b))
#define WHITE      RGB(31, 63, 31)
#define	BLACK      RGB(0, 0, 0)
#define RED        RGB(31, 0, 0)
#define BLUE       RGB(0, 0, 31)
#define GREEN      RGB(0, 63, 0)
#define YELLOW     RGB(31, 63, 0)
#define CYAN       RGB(0, 63, 31)
#define MAGENTA    RGB(31, 0, 31)
#define ORANGE     0xFA80 // яркий оранжевый

TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, __RST);

enum Direction
{
  DIR_UP, 
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LEFT,
  DIR_LOW = DIR_UP,
  DIR_HIGH = DIR_LEFT
};

// начало игры
bool game_started;

// ЗМЕЙКА 
Direction snake_direction;
// координаты головы Змейки
int16_t head_x;
int16_t head_y;
//координаты хвостика Змейки (хвост не больше чем размер экрана)
#define TAIL_MAX_LENGTH (16*16-1)
int16_t tail_x[TAIL_MAX_LENGTH];
int16_t tail_y[TAIL_MAX_LENGTH];
uint8_t tail_length;

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

void gameRestart(int16_t x, int16_t y, Direction d)
{
  // сброс генератора случайных чисел
  randomSeed(analogRead(RANDOM_PIN));

  // закрасила поле черным цветом
  tft.fillRect(0, 0, 128, 128, BLACK);

  // Змейка (голова)
  head_x = x;
  head_y = y;
  snake_direction = d;
  drawSnake(head_x, head_y);
  // хвостик нулевой длинны
  tail_length = 0;
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

  // в самом начале игры игра не началась
  game_started = 0;
  
  // подготовка игры
  // координаты центр поля, направление вверх
  gameRestart(64, 64, DIR_UP);
}

void loop() 
{
  if (game_started == 1)
  {
  }
  else
  {
    // нажатие на джостик
    bool key_pressed = digitalRead(SW_PIN);
    if (key_pressed == 0)
    {
      game_started = 1;
      gameRestart(64, 64, random(DIR_LOW, DIR_HIGH+1));
    }
    else 
    {
      return;
    }
  }

  /**Serial.print("head = "); Serial.print(head_x, DEC);
  Serial.print(","); Serial.print(head_y, DEC);
  Serial.print(", tail = "); Serial.print(tail_x, DEC);
  Serial.print(","); Serial.print(tail_y, DEC);/**/

  // пауза в игре
  Direction previous_direction = snake_direction;
  for (int i = 0; i < 750; i = i + 10)
  {
    delay(10);

    int x, y;
    x = analogRead(VRX_PIN); // от 0 до 1023
    y = analogRead(VRY_PIN); // от 0 до 1023
    int new_x, new_y;
    new_x = 512 - x; // от -512 до 511, где 0 х=517
    new_y = 512 - y; // от -512 до 511, где 0 у=522

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
        if (tail_length >= 0 && previous_direction != DIR_DOWN)
        {
          snake_direction = DIR_UP;
        }
      }
    }
  }

  //рассчитать новые координаты (голова)
  int16_t newhead_x = head_x, newhead_y = head_y;
  if (snake_direction == DIR_RIGHT)
    newhead_x += 8;
  else if (snake_direction == DIR_LEFT)
    newhead_x -= 8;
  else if (snake_direction == DIR_DOWN)
    newhead_y += 8;
  else if (snake_direction == DIR_UP)
    newhead_y -= 8;
  
  /**Serial.print(", snake_direction = "); Serial.print(snake_direction, DEC);
  Serial.print(", head = "); Serial.print(newhead_x, DEC);
  Serial.print(","); Serial.print(newhead_y, DEC);
  Serial.print(", tail = "); Serial.print(newtail_x, DEC);
  Serial.print(","); Serial.println(newtail_y, DEC);/**/

  // условие с пересечением линий
  if (newhead_x == -8 || newhead_x == 128 || newhead_y == -8 || newhead_y == 128)
  {
    game_started = 0;
    return;
  }
  // кусачка
  for (int i = 0; i < tail_length ; ++i)
  {
    if (tail_x[i] == newhead_x && tail_y[i] == newhead_y)
    {
      game_started = 0;
      return;
    }
  }

  //----------------
  // для отладки
  int16_t tmp_x = tail_length ? tail_x[tail_length-1] : head_x;
  int16_t tmp_y = tail_length ? tail_y[tail_length-1] : head_y;
  //----------------

  // стрираю по старым координатам (голова)
  clearSnake(head_x, head_y);
  // стираю по старым координатам (хвостик)
  for (int i = 0; i < tail_length; ++i)
  {
    clearSnake(tail_x[i], tail_y[i]);    
  }
  // рисую по новым координатам (голова)
  drawSnake(newhead_x, newhead_y);

  // применяю новые координаты (хвостик)
  for (int i = tail_length - 1; i > 0 ; --i)
  {
    tail_x[i] = tail_x[i - 1];
    tail_y[i] = tail_y[i - 1];
  }
  tail_x[0] = head_x;
  tail_y[0] = head_y;
  //----------------
  // для отладки
  static int kuk = 0;
  kuk = (kuk + 1) % 10;
  if (kuk == 0)
  {
    tail_x[tail_length] = tmp_x;
    tail_y[tail_length] = tmp_y;
    tail_length++;
  }
  //----------------
  // рисую по новым координатам (хвостик)
  for (int i = 0; i < tail_length; ++i)
  {
    drawSnake(tail_x[i], tail_y[i]);
  }

  // применяю новые координаты (голова)
  head_x = newhead_x;
  head_y = newhead_y;
}