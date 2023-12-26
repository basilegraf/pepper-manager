#pragma once

#include <LiquidCrystal.h>
#include "Constants.h"
#include "ManagerState.h"

namespace Characters
{
  static constexpr byte Up = 0;
  static constexpr byte UpDef[8] = 
  {
    B00000,
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00100,
    B00000,
  };

  static constexpr byte Down = 1;
  static constexpr byte DownDef[8] = 
  {
    B00000,
    B00100,
    B00100,
    B00100,
    B10101,
    B01110,
    B00100,
    B00000,
  };

  static constexpr byte DegC = 2;
  static constexpr byte DegCDef[8] = 
  {
    B11000,
    B11000,
    B00111,
    B01000,
    B01000,
    B01000,
    B00111,
    B00000,
  };
}


class Display
{
public:
  
  inline Display() : 
    Lcd(Constants::Display::RS, Constants::Display::EN, Constants::Display::D4, Constants::Display::D5, Constants::Display::D6, Constants::Display::D7)
  {};

  inline ~Display(){};

  inline Setup()
  {
    Lcd.begin(Constants::Display::NumberOfColumns, Constants::Display::NumberOfLines);
    for (uint8_t kLine = 0; kLine < Constants::Display::NumberOfLines; kLine++)
    {
      Lcd.setCursor(0, kLine);
      for (uint8_t kCol = 0; kCol < Constants::Display::NumberOfColumns; kCol++)
      {
        Lcd.print(" ");
      }
    }

    Lcd.createChar(Characters::Up, Characters::UpDef);
    Lcd.createChar(Characters::Down, Characters::DownDef);
    Lcd.createChar(Characters::DegC, Characters::DegCDef);
    
  }


  void PrintDateTime(uint8_t day, uint8_t month, uint8_t hours, uint8_t minutes, ManagerState state)
  {
    constexpr uint8_t colStart = 0;
    constexpr uint8_t lineStart = 0;

    Lcd.setCursor(colStart, lineStart);
    Lcd.print("00-00 00:00");

    Lcd.setCursor(colStart + (day > 9 ? 0 : 1), lineStart);
    Lcd.print(day);

    Lcd.setCursor(colStart + (month > 9 ? 3 : 4), lineStart);
    Lcd.print(month);

    Lcd.setCursor(colStart + (hours > 9 ? 6 : 7), lineStart);
    Lcd.print(hours);

    Lcd.setCursor(colStart + (minutes > 9 ? 9 : 10), lineStart);
    Lcd.print(minutes);

    switch (state)
    {
      case ManagerState::EditDay :
        Lcd.setCursor(colStart + 1, lineStart);
        Lcd.blink();
        break;
      case ManagerState::EditMonth :
        Lcd.setCursor(colStart + 4, lineStart);
        Lcd.blink();
        break;
      case ManagerState::EditHour :
        Lcd.setCursor(colStart + 7, lineStart);
        Lcd.blink();
        break;
      case ManagerState::EditMinutes :
        Lcd.setCursor(colStart + 10, lineStart);
        Lcd.blink();
        break;
      case ManagerState::Display :
        Lcd.noBlink();
        break;
      case ManagerState::EditSetTemperature :
      case ManagerState::EditArtificialDayLength :
      default:
        break;
    }
  }

  void PrintTemperature(uint8_t chanelIndex, float temperature, bool isHeating)
  {
    constexpr uint8_t colStart = 0;
    constexpr uint8_t lineStart = 1;

    Lcd.setCursor(colStart, lineStart);
    Lcd.print(chanelIndex + 1);
    Lcd.print(":");
    Lcd.print(temperature);
    Lcd.write(isHeating ? Characters::Up : Characters::Down);
  }

  void PrintSetTemperature(uint8_t setTemperature, ManagerState state)
  {
    constexpr uint8_t colStart = 8;
    constexpr uint8_t lineStart = 1;

    Lcd.setCursor(colStart, lineStart);
    Lcd.print(" ");
    Lcd.print(setTemperature);
    Lcd.write(Characters::DegC);
    if (state == ManagerState::EditSetTemperature)
    {
      Lcd.setCursor(colStart + 2, lineStart);
      Lcd.blink();
    }
  }

  void PrintArtificialDayLength(uint8_t dayLengthHours, ManagerState state)
  {
    constexpr uint8_t colStart = 13;
    constexpr uint8_t lineStart = 1;

    Lcd.setCursor(colStart, lineStart);
    Lcd.print("00h");
    Lcd.setCursor(dayLengthHours > 9 ? colStart : colStart + 1, lineStart);
    Lcd.print(dayLengthHours);
    if (state == ManagerState::EditArtificialDayLength)
    {
      Lcd.setCursor(colStart + 1, lineStart);
      Lcd.blink();
    }
  }

  void QuitEditMode()
  {
    Lcd.noBlink();
  }

  LiquidCrystal Lcd;
};