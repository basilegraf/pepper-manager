#pragma once

#include "Arduino.h"

namespace Constants
{
  namespace Time
  {
    static constexpr float EarliestLightOnHour = 7.0f;
    static constexpr float MinArtificialDayDuration = 12.0f;
  }

// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//LiquidCrystal lcd(12, 10, 5,4,3,2);
  namespace Display
  {
    static constexpr uint8_t RS = 12;
    static constexpr uint8_t EN = 10;
    static constexpr uint8_t D4 = 5;
    static constexpr uint8_t D5 = 4;
    static constexpr uint8_t D6 = 3;
    static constexpr uint8_t D7 = 2;

    static constexpr uint8_t NumberOfColumns = 20;
    static constexpr uint8_t NumberOfLines = 4;
  }

  namespace Buttons
  {
    static constexpr uint8_t MinusPinNumber = 11;
    static constexpr uint8_t EditPinNumber = 9;
    static constexpr uint8_t PlusPinNumber = 7;
  }

  namespace Temperature
  {
    static constexpr uint8_t NumberOfSensors = 4;
    static constexpr uint8_t SensorsOneWirePinNumber = 6;
    static constexpr uint8_t HeaterTransistorPinNumbers[4] = {A0, A1, A2, A3};
    static constexpr float HeaterPower[4] = {12.0f * 12.0f / 90.0f, 12.0f * 12.0f / 90.0f, 12.0f * 12.0f / 90.0f, 12.0f * 12.0f / 90.0f}; // 90Ohm at 12V
    static constexpr float DefaultSetTemperature = 27.0f;
    static constexpr float DefaultDeltaTemperature = 0.25f;
  }

  namespace Light
  {
    static constexpr uint8_t LightControlPinNumber = A4; // LED_BUILTIN;
    static float LightPower = 5.0f;
    static float DayTimeOverlapHours = 0.25f; 
  }
  
}