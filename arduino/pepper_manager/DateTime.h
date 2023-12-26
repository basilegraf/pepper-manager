#pragma once
#include <LiquidCrystal.h>

class DateTime
{
  public:
  DateTime();
  ~DateTime();
 
  void SerialPrint();
  
  void IncrementSeconds();
  void IncrementMinutes();
  void DecrementMinutes();
  void IncrementHours();
  void DecrementHours();
  void IncrementDays();
  void DecrementDays();
  void IncrementMonths();
  void DecrementMonths();

  int Seconds;  // 0-59
  int Minutes;  // 0-59
  int Hours;    // 0-23
  int Day;      // 1-30/31
  int Month;    // 1-12
  int YearDay;  // 0-364
  float DecimalHours; // 0.0f <= x < 24.0f

  private:
  void _update();
};