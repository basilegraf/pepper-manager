#include <Arduino.h>
#include <LiquidCrystal.h>
#include "DateTime.h"

DateTime::DateTime() :
  Seconds(0),  // 0-59
  Hours(0),    // 0-23
  Day(1),      // 1-30/31
  Month(1),    // 1-12
  YearDay(0),  // 0-364
  DecimalHours(0.0f) // 0.0f <= x < 24.0f
{

}

void DateTime::SerialPrint()
{
  Serial.print("(");
  Serial.print(YearDay);
  Serial.print(") ");
  Serial.print(Day);
  Serial.print("-");
  Serial.print(Month);
  Serial.print(" ");
  Serial.print(Hours);
  Serial.print(":");
  Serial.print(Minutes);
  Serial.print(":");
  Serial.print(Seconds);
}

// void DateTime::LcdPrint(LiquidCrystal lcd, byte colStart, byte lineStart)
// {
//   lcd.setCursor(colStart, lineStart);
//   lcd.print("00-00 00:00");

//   lcd.setCursor(colStart + (Day > 9 ? 0 : 1), lineStart);
//   lcd.print(Day);

//   lcd.setCursor(colStart + (Month > 9 ? 3 : 4), lineStart);
//   lcd.print(Month);

//   lcd.setCursor(colStart + (Hours > 9 ? 6 : 7), lineStart);
//   lcd.print(Hours);

//   lcd.setCursor(colStart + (Minutes > 9 ? 9 : 10), lineStart);
//   lcd.print(Minutes);
// }

DateTime::~DateTime(){};

void DateTime::IncrementSeconds()
{
  Seconds++;
  _update();
}

void DateTime::IncrementMinutes()
{
  Minutes++;
  _update();
}

void DateTime::DecrementMinutes()
{
  Minutes--;
  _update();
}

void DateTime::IncrementHours()
{
  Hours++;
  _update();
}

void DateTime::DecrementHours()
{
  Hours--;
  _update();
}

void DateTime::IncrementDays()
{
  Day++;
  _update();
}

void DateTime::DecrementDays()
{
  Day--;
  _update();
}

void DateTime::IncrementMonths()
{
  Month++;
  _update();
}

void DateTime::DecrementMonths()
{
  Month--;
  _update();
}

void div_mod(int a, int b, int& d, int&m)
{ 
    d = 0;
    m = a;
    while (m < 0)
    {
        d--;
        m += b;
    }
    while (m >= b)
    {
        d++;
        m -= b;
    }
}


void DateTime::_update()
{
  int carry = 0;

  div_mod(Seconds, 60, carry, Seconds);
  
  Minutes += carry;
  div_mod(Minutes, 60, carry, Minutes);

  Hours += carry;
  div_mod(Hours, 24, carry, Hours);

  YearDay += carry;
  Day  += carry;
  Day--;
  div_mod(Day, 31, carry, Day);
  Day++;

  Month += carry;
  Month--;
  div_mod(Month, 12, carry, Month);
  Month++;

  if (Day == 31)
  {
    if ((Month == 2) || (Month == 6) || (Month == 4) || (Month == 9) || (Month == 11))
    {
      Day = 1;
      Month++;
    }
  }
  else if ((Day == 29) && (Month == 4))
  {
    Day = 1;
    Month++;
  }

  DecimalHours = Hours + (1.0f / 60.0f) * Minutes + (1.0f / 3600.0f) * Seconds;

}