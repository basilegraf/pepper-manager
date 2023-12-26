#include <Arduino.h>
#include "SunHours.h"

// Hours every 40 days, starting 1st of January (Neuchâtel)
float sunrisesHoursLUT[] = {9.3, 8.75, 7.533333333333333, 6.3, 5.616666666666667, 5.95, 6.783333333333333, 7.65, 8.616666666666667, 9.283333333333333, 8.883333333333333};
float sunsetsHourLUT[] = {17.866666666666667, 18.783333333333335, 19.766666666666666, 20.683333333333334, 21.416666666666668, 21.316666666666666, 20.3, 18.983333333333334, 17.916666666666668, 17.8, 18.65};

// Day of time changes
constexpr int idx1 = 90;
constexpr int idx2 =  299;



Sun::Sun(float earliestOn, float minDayDuration)
{
  _earliestOn = earliestOn >= 0.0f ? earliestOn : 0.01f;
  _earliestOn = _earliestOn < 12.0f ? _earliestOn : 11.99f;
  _minDayDuration = minDayDuration > 0.0f ? minDayDuration : 0.01f;
  _minDayDuration = _minDayDuration < 24.0f ? _minDayDuration : 23.99f;
  _computeLightSwitchingTimes();
}

Sun::~Sun(){};

void Sun::Print()
{
  Serial.print("_earliestOn : ");
  Serial.print(_earliestOn);
  Serial.print(" _minDayDuration : ");
  Serial.print(_minDayDuration);
  Serial.print("\n");
}

float evalLUT(int day, float* lut)
{
  day = day % 365;
  byte i1 = day / 40;
  byte i2 = i1 + 1;
  float l1 = lut[i1];
  float l2 = lut[i2];
  float val = 0.025f * ((40.0f * i2 - day) * l1 + (day - 40.0f * i1) * l2);

  if ((day < idx1) || (idx2 < day))
  {
    val -= 1.0f;
  }

  return val;
}

float Sun::SunriseHour(int day)
{
  return evalLUT(day, sunrisesHoursLUT);
}


float Sun::SunsetHour(int day)
{
  return evalLUT(day, sunsetsHourLUT);
}

void Sun::SetDay(int day)
{
  _day = day % 365;
  _computeLightSwitchingTimes();
}

void Sun::SetMinDayDuration(float minDayDuration)
{
  _minDayDuration = minDayDuration >= 0.0f ? minDayDuration : 0.01f;
  _minDayDuration = _minDayDuration < 24.0f ? _minDayDuration : 23.99f;
  _computeLightSwitchingTimes();
}

float Sun::GetMinDayDuration()
{
  return _minDayDuration;
}

bool Sun::IsLightOn(int yearDay, float decimalHour)
{
  if (yearDay != _day)
  {
    SetDay(yearDay);
  }
  bool lightOn = false;
  if ((HourOnMorning <= decimalHour) && (decimalHour < HourOffMorning))
  {
    lightOn = true;
  }
  if ((HourOnEvening <= decimalHour) && (decimalHour < HourOffEvening))
  {
    lightOn = true;
  }
  return lightOn;
}

void Sun::_computeLightSwitchingTimes()
{
  float sunriseHour = SunriseHour(_day);
  float sunsetHour = SunsetHour(_day);

  float naturalDayDuration = sunsetHour - sunriseHour;

  if (naturalDayDuration >= _minDayDuration)
  {
    HourOnMorning = 11.0f;
    HourOffMorning = 11.0f;
    HourOnEvening = 13.0f;
    HourOffEvening = 13.0f;
    return;
  }

  float missignDayTime = _minDayDuration - naturalDayDuration;
  HourOnMorning = sunriseHour - 0.5 * missignDayTime;

  HourOnMorning = HourOnMorning < _earliestOn ? _earliestOn : HourOnMorning;

  HourOffMorning = sunriseHour >= HourOnMorning ? sunriseHour : HourOnMorning;

  float duration = naturalDayDuration + HourOffMorning - HourOnMorning;
  HourOnEvening = sunsetHour;
  if (duration < _minDayDuration)
  {
    HourOffEvening = sunsetHour + (_minDayDuration - duration);
  }
  else
  {
    HourOffEvening = HourOnEvening;
  }

}
