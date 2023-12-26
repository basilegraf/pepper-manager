#pragma once
class Sun
{
  public:
  Sun(float earliestOn, float minDayDuration);
  ~Sun();

  void Print();

  static float SunriseHour(int day); // Returns hours as floating point. E.g. 8.5f <=> 8h30min
  static float SunsetHour(int day);  // Returns hours as floating point. E.g. 8.5f <=> 8h30min

  void SetDay(int day);
  void SetMinDayDuration(float minDayDuration);
  float GetMinDayDuration();
  bool IsLightOn(int yearDay, float decimalHour);
  
  float HourOnMorning;    // Morning turn ON time
  float HourOffMorning;   // Evening turn OFF time
  float HourOnEvening;    // Morning turn ON time
  float HourOffEvening;   // Evening turn OFF time

  private:
    void _computeLightSwitchingTimes();
    int _day;        // Day number, 0 = 01.01.2024
    float _earliestOn; // Do not turn light on earlier than this, eg 7.0f => 7h
    float _minDayDuration; // Minimum artificial day duration in hours

};