/*
  MR2 робот на Arduino
  Created by Rostislav Varzar
*/

#include <SoftwareSerial.h>

// Определения для портов
#define CRASHSENSOR1 A2

#define US1_trigPin A4
#define US1_echoPin A5
#define US2_trigPin 10
#define US2_echoPin 11
#define US3_trigPin 8
#define US3_echoPin 9
#define US4_trigPin 2
#define US4_echoPin 3
#define US5_trigPin A0
#define US5_echoPin A1
#define US6_trigPin 13
#define US6_echoPin 12
#define minimumRange 0
#define maximumRange 200

#define PWMA 5
#define DIRA 4
#define PWMB 6
#define DIRB 7

// Параметры ПИД регулятора
#define KPID 0.75

// Параметры дистанций
#define DIST1 20
#define DIST2 45
#define DIST4 2

// Параметры моторов
#define MPWR 30
#define LLLL 10

// Программный UART для Bluetooth
SoftwareSerial BT(A3, 0);

// UART1 для Bluetooth
//#define BT Serial1

// Переменные для управления роботом
char command = 'S';
char prevCommand = 'A';
int velocity = 0;
unsigned long timer0 = 2000;
unsigned long timer1 = 0;

// Параметры заедания
#define DIST3 2

// Задержка между считываниями датчиков
#define USDELAY 15

// Задержка между столкновениями
#define CDELAY 250

// Максимальное количество заеданий
#define COLMAX 5

int flag = 0;
long left1 = 0;
long right1 = 0;
long oldleft1 = 0;
long oldright1 = 0;
long left2 = 0;
long right2 = 0;
long oldleft2 = 0;
long oldright2 = 0;
long colcounter = 0;
float u = 0;

void setup()
{
  // Инициализация последовательного порта
  Serial.begin(9600);

  // Инициализация последовательного порта для Bluetooth
  BT.begin(9600);

  // Инициализация выводов для работы с УЗ датчиком
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);
  pinMode(US3_trigPin, OUTPUT);
  pinMode(US3_echoPin, INPUT);
  pinMode(US4_trigPin, OUTPUT);
  pinMode(US4_echoPin, INPUT);
  pinMode(US5_trigPin, OUTPUT);
  pinMode(US5_echoPin, INPUT);
  pinMode(US6_trigPin, OUTPUT);
  pinMode(US6_echoPin, INPUT);

  // Инициализация выходов для управления моторами
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
}

void loop()
{

  /*
    // Тестирование датчиков
    while (true)
    {
      Serial.print(readUS1_distance());
      Serial.print("\t");
      delay(25);
      Serial.print(readUS2_distance());
      Serial.print("\t");
      delay(25);
      Serial.print(readUS3_distance());
      Serial.print("\t");
      delay(25);
      Serial.print(readUS4_distance());
      Serial.print("\t");
      delay(25);
      Serial.print(readUS5_distance());
      Serial.print("\t");
      delay(25);
      Serial.print(readUS6_distance());
      Serial.print("\t");
      delay(25);
      Serial.println("");
    }
  */

  /*
    motorA_setpower(35, false);
    motorB_setpower(35, true);
    delay(500);
    motorA_setpower(0, false);
    motorB_setpower(0, true);
    delay(500);
    while (true)
    {

    }
  */

  // Работа по Bluetooth
  while (digitalRead(CRASHSENSOR1))
  {
    // Bluetooth
    if (BT.available() > 0) {
      timer1 = millis();
      prevCommand = command;
      command = BT.read();
      if (command != prevCommand) {
        Serial.println(command);
        switch (command) {
          case 'F':
            motorA_setpower(velocity + LLLL, false);
            motorB_setpower(velocity, true);
            break;
          case 'B':
            motorA_setpower(-velocity - LLLL, false);
            motorB_setpower(-velocity, true);
            break;
          case 'L':
            motorA_setpower(-velocity - LLLL, false);
            motorB_setpower(velocity, true);
            break;
          case 'R':
            motorA_setpower(velocity + LLLL, false);
            motorB_setpower(-velocity, true);
            break;
          case 'S':
            motorA_setpower(0, false);
            motorB_setpower(0, true);
            break;
          case 'I':  //FR
            //yellowCar.ForwardRight_4W(velocity);
            motorA_setpower(velocity + LLLL, false);
            motorB_setpower(velocity / 2, true);
            break;
          case 'J':  //BR
            motorA_setpower(-velocity - LLLL, false);
            motorB_setpower(-velocity / 2, true);
            break;
          case 'G':  //FL
            motorA_setpower((velocity  + LLLL)/ 2, false);
            motorB_setpower(velocity, true);
            break;
          case 'H':  //BL
            motorA_setpower((-velocity - LLLL) / 2, false);
            motorB_setpower(-velocity, true);
            break;
          case 'W':
            break;
          case 'w':
            break;
          case 'U':
            break;
          case 'u':
            break;
          case 'V':
            break;
          case 'v':
            break;
          case 'X':
            break;
          case 'x':
            break;
          case 'D':  //Everything OFF
            motorA_setpower(0, false);
            motorB_setpower(0, true);
            break;
          default:  //Get velocity
            if (command == 'q') {
              velocity = 50;  //Full velocity
            }
            else {
              //Chars '0' - '9' have an integer equivalence of 48 - 57, accordingly.
              if ((command >= 48) && (command <= 57)) {
                //Subtracting 48 changes the range from 48-57 to 0-9.
                //Multiplying by 25 changes the range from 0-9 to 0-225.
                velocity = (command - 48) * 10;
                Serial.println(velocity);
              }
            }
        }
      }
    }
    else {
      timer0 = millis();  //Get the current time (millis since execution started).
      //Check if it has been 500ms since we received last command.
      if ((timer0 - timer1) > 500) {
        //More tan 500ms have passed since last command received, car is out of range.
        //Therefore stop the car and turn lights off.
        motorA_setpower(0, false);
        motorB_setpower(0, true);
      }
    }
  }
  delay(5000);

  // Ездилка по складу
  while (true)
  {
    // Вперед
    delay(CDELAY);
    colcounter = 0;
    motorA_setpower(MPWR, false);
    motorB_setpower(MPWR, true);
    while (flag == 0)
    {
      Serial.print(flag);
      Serial.print("\t");
      left1 = readUS2_distance(); delay(USDELAY);
      right1 = readUS1_distance(); delay(USDELAY);
      left2 = readUS6_distance(); delay(USDELAY);
      right2 = readUS5_distance(); delay(USDELAY);
      // Основное движение
      if ((left1 != (-1)) && (right1 != (-1)))
      {
        if ((left1 < DIST2) || (right1 < DIST2))
        {
          u = KPID * float(left1 - right1);
          motorA_setpower(MPWR + LLLL + u, false);
          motorB_setpower(MPWR - u, true);
        }
        else
        {
          motorA_setpower(MPWR + LLLL, false);
          motorB_setpower(MPWR, true);
        }
      }
      else
      {
        motorA_setpower(MPWR + LLLL, false);
        motorB_setpower(MPWR, true);
      }
      // Проверка на столкновение
      if ((left1 < DIST1) && (left1 != (-1)))
      {
        flag = 1;
        Serial.print("E1\t");
      }
      if ((right1 < DIST1) && (right1 != (-1)))
      {
        flag = 1;
        Serial.print("E2\t");
      }
      // Прверка на заедание
      if ((abs(oldleft1 - left1) < DIST3) && (abs(oldright1 - right1) < DIST3) &&
          (abs(oldleft2 - left2) < DIST3) && (abs(oldright2 - right2) < DIST3))
      {
        colcounter = colcounter + 1;
        Serial.print("E3\t");
      }
      if (colcounter > COLMAX)
      {
        flag = 1;
        Serial.print("E4\t");
      }
      oldleft1 = left1;
      oldright1 = right1;
      oldleft2 = left2;
      oldright2 = right2;
      Serial.print(left1);
      Serial.print("\t");
      Serial.print(right1);
      Serial.print("\t");
      Serial.print(left2);
      Serial.print("\t");
      Serial.print(right2);
      Serial.print("\t");
      Serial.print(u);
      Serial.print("\t");
      Serial.println("");
    }
    // Назад
    delay(CDELAY);
    colcounter = 0;
    motorA_setpower(MPWR, true);
    motorB_setpower(MPWR, false);
    while (flag == 1)
    {
      Serial.print(flag);
      Serial.print("\t");
      left1 = readUS4_distance(); delay(USDELAY);
      right1 = readUS3_distance(); delay(USDELAY);
      left2 = readUS5_distance(); delay(USDELAY);
      right2 = readUS6_distance(); delay(USDELAY);
      // Основное движение
      if ((left1 != (-1)) && (right1 != (-1)))
      {
        if ((left1 < DIST2) || (right1 < DIST2))
        {
          u = KPID * float(right1 - left1);
          motorA_setpower(MPWR + LLLL + u, true);
          motorB_setpower(MPWR - u, false);
        }
        else
        {
          motorA_setpower(MPWR + LLLL, true);
          motorB_setpower(MPWR, false);
        }
      }
      else
      {
        motorA_setpower(MPWR + LLLL, true);
        motorB_setpower(MPWR, false);
      }
      // Проверка на столкновение
      if ((left1 < DIST1) && (left1 != (-1)))
      {
        flag = 0;
        Serial.print("E1\t");
      }
      if ((right1 < DIST1) && (right1 != (-1)))
      {
        flag = 0;
        Serial.print("E2\t");
      }
      // Прверка на заедание
      if ((abs(oldleft1 - left1) < DIST3) && (abs(oldright1 - right1) < DIST3) &&
          (abs(oldleft2 - left2) < DIST3) && (abs(oldright2 - right2) < DIST3))
      {
        colcounter = colcounter + 1;
        Serial.print("E3\t");
      }
      if (colcounter > COLMAX)
      {
        flag = 0;
        Serial.print("E4\t");
      }
      oldleft1 = left1;
      oldright1 = right1;
      oldleft2 = left2;
      oldright2 = right2;
      Serial.print(left1);
      Serial.print("\t");
      Serial.print(right1);
      Serial.print("\t");
      Serial.print(left2);
      Serial.print("\t");
      Serial.print(right2);
      Serial.print("\t");
      Serial.print(u);
      Serial.print("\t");
      Serial.println("");
    }
  }
}

// УЗ датчик 1
long readUS1_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US1_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_trigPin, LOW);
  duration = pulseIn(US1_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 2
long readUS2_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US2_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US2_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_trigPin, LOW);
  duration = pulseIn(US2_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 3
long readUS3_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US3_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US3_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US3_trigPin, LOW);
  duration = pulseIn(US3_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 4
long readUS4_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US4_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US4_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US4_trigPin, LOW);
  duration = pulseIn(US4_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 5
long readUS5_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US5_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US5_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US5_trigPin, LOW);
  duration = pulseIn(US5_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// УЗ датчик 6
long readUS6_distance()
{
  long duration = 0;
  long distance = 0;
  digitalWrite(US6_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(US6_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US6_trigPin, LOW);
  duration = pulseIn(US6_echoPin, HIGH, 50000);
  distance = duration / 58.2;
  if (distance >= maximumRange || distance <= minimumRange) {
    distance = -1;
  }
  return distance;
}

// Мощность мотора "A" от -100% до +100% (от знака зависит направление вращения)
void motorA_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRA, LOW);
  }
  else
  {
    digitalWrite(DIRA, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMA, pwmvalue);
}

// Мощность мотора "B" от -100% до +100% (от знака зависит направление вращения)
void motorB_setpower(int pwr, bool invert)
{
  // Проверка, инвертирован ли мотор
  if (invert)
  {
    pwr = -pwr;
  }
  // Проверка диапазонов
  if (pwr < -100)
  {
    pwr = -100;
  }
  if (pwr > 100)
  {
    pwr = 100;
  }
  // Установка направления
  if (pwr < 0)
  {
    digitalWrite(DIRB, LOW);
  }
  else
  {
    digitalWrite(DIRB, HIGH);
  }
  // Установка мощности
  int pwmvalue = abs(pwr) * 2.55;
  analogWrite(PWMB, pwmvalue);
}

