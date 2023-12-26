#include "PepperManager.h"

#include "Arduino.h"

#include "Button.h"
#include "SunHours.h"
#include "DateTime.h"
#include "Constants.h"

PepperManager::PepperManager() :
  State(ManagerState::Display),
  Sun(Constants::Time::EarliestLightOnHour, Constants::Time::MinArtificialDayDuration),
  Energy(0.0f),
  _buttonMinus(*this, Constants::Buttons::MinusPinNumber),
  _buttonEdit(*this, Constants::Buttons::EditPinNumber),
  _buttonPlus(*this, Constants::Buttons::PlusPinNumber),
  _accumulatedEnergy(0.0f),
  _displayEnergy(false)
{}

PepperManager::~PepperManager()
{}

void PepperManager::Setup()
{
  _buttonMinus.Setup(nullptr, &PepperManager::_callbackButtonMinusPress);
  _buttonEdit.Setup(nullptr, &PepperManager::_callbackButtonEditPress);
  _buttonPlus.Setup(nullptr, &PepperManager::_callbackButtonPlusPress);

  _buttonEdit.LongLowCallback = &PepperManager::_callbackButtonEditLongPress;

  Display.Setup();

  // Light control MOSFET output
  pinMode(Constants::Light::LightControlPinNumber, OUTPUT);

  TempControl.Setup();

  _updateAfterStateChange();
}

void PepperManager::Loop(bool isNewSecond)
{
  _buttonMinus.Loop();
  _buttonEdit.Loop();
  _buttonPlus.Loop();

  if (isNewSecond)
  {
    Date.IncrementSeconds();

    if (State == ManagerState::Display)
    {

      Display.PrintDateTime(Date.Day, Date.Month, Date.Hours, Date.Minutes, State);
      Display.PrintSetTemperature(TempControl.TemperatureSet, State);
      uint8_t minDayDuration = uint8_t(round(Sun.GetMinDayDuration()));
      Display.PrintArtificialDayLength(minDayDuration, State);

      if (_displayEnergy)
      {
        _displayEnergy = false;
        delay(600); // Wait to match temp sensor read time
        Display.PrintEnergy(Energy);
      }
      else
      {
        // Temperature control & display
        TempControl.Loop();
        Display.PrintTemperature(
          TempControl.LastUpdatedIndex,
          TempControl.TemperatureMeasurements[TempControl.LastUpdatedIndex],
          TempControl.IsHeating[TempControl.LastUpdatedIndex]);
        
        if (TempControl.LastUpdatedIndex + 1 == Constants::Temperature::NumberOfSensors)
        {
          // Next time, skip temperatur control and display energy instead
          _displayEnergy = true;
        }
      }
      

      // Light control & display
      bool lightOn = Sun.IsLightOn(Date.YearDay, Date.DecimalHours);
      digitalWrite(Constants::Light::LightControlPinNumber, lightOn);
      Display.PrintLightOnOff(lightOn);

      // Accumulate energy
      constexpr float ratio = 1.0f / 3600.0f; // runs every second
      if (lightOn) _accumulatedEnergy += ratio * Constants::Light::LightPower;

      for (uint8_t k = 0; k < Constants::Temperature::NumberOfSensors; k++)
      {
        if (TempControl.IsHeating[k]) _accumulatedEnergy += ratio * Constants::Temperature::HeaterPower[k];
      }

      if (_accumulatedEnergy > min(1.0f, 0.001f * Energy))
      {
        Energy += _accumulatedEnergy;
        _accumulatedEnergy = 0.0f;
      }
    } 
  }
}


void PepperManager::_callbackButtonMinusPress()
{
  switch (State)
  {
    case ManagerState::Display :
      break;
    case ManagerState::EditDay :
      Date.DecrementDays();
      break;
    case ManagerState::EditMonth :
      Date.DecrementMonths();
      break;
    case ManagerState::EditHour :
      Date.DecrementHours();
      break;
    case ManagerState::EditMinutes :
      Date.DecrementMinutes();
      break;
    case ManagerState::EditSetTemperature :
      TempControl.TemperatureSet -= 1.0f;
      break;
    case ManagerState::EditArtificialDayLength :
    {
      float minDayDuration = round(Sun.GetMinDayDuration());
      minDayDuration -= 1.0f;
      minDayDuration = max(0.0f, min(minDayDuration, 23.0f));
      Sun.SetMinDayDuration(minDayDuration);
      break;
    } 
  }
  _updateAfterStateChange();
}

void PepperManager::_callbackButtonEditPress()
{
  switch (State)
  {
    case ManagerState::Display :
      break;
    case ManagerState::EditDay :
      State = ManagerState::EditMonth;
      break;
    case ManagerState::EditMonth :
      State = ManagerState::EditHour;
      break;
    case ManagerState::EditHour :
      State = ManagerState::EditMinutes;
      break;
    case ManagerState::EditMinutes :
      State = ManagerState::EditSetTemperature;
      break;
    case ManagerState::EditSetTemperature :
      State = ManagerState::EditArtificialDayLength;
      break;
    case ManagerState::EditArtificialDayLength :
      State = ManagerState::EditDay;
      break;
  }
  _updateAfterStateChange();
}

void PepperManager::_callbackButtonEditLongPress()
{
  if (State == ManagerState::Display)
  {
    State = ManagerState::EditMinutes;
  }
  else
  {
    State = ManagerState::Display;
  }
  _updateAfterStateChange();
}

void PepperManager::_callbackButtonPlusPress()
{
  switch (State)
  {
    case ManagerState::Display :
      break;
    case ManagerState::EditDay :
      Date.IncrementDays();
      break;
    case ManagerState::EditMonth :
      Date.IncrementMonths();
      break;
    case ManagerState::EditHour :
      Date.IncrementHours();
      break;
    case ManagerState::EditMinutes :
      Date.IncrementMinutes();
      break;
    case ManagerState::EditSetTemperature :
      TempControl.TemperatureSet += 1.0f;
      break;
    case ManagerState::EditArtificialDayLength :
    {
      float minDayDuration = round(Sun.GetMinDayDuration());
      minDayDuration += 1.0f;
      minDayDuration = max(0.0f, min(minDayDuration, 23.0f));
      Sun.SetMinDayDuration(minDayDuration);
      break;
    } 
  }
  _updateAfterStateChange();
}

void PepperManager::_updateAfterStateChange()
{
  switch (State)
  {
    case ManagerState::Display :
      Display.QuitEditMode();
      break;
    case ManagerState::EditDay :
    case ManagerState::EditMonth :
    case ManagerState::EditHour :
    case ManagerState::EditMinutes :
      Display.PrintDateTime(Date.Day, Date.Month, Date.Hours, Date.Minutes, State);
      break;
    case ManagerState::EditSetTemperature :
      Display.PrintSetTemperature(TempControl.TemperatureSet, State);
      break;
    case ManagerState::EditArtificialDayLength :
      uint8_t minDayDuration = uint8_t(round(Sun.GetMinDayDuration()));
      Display.PrintArtificialDayLength(minDayDuration, State);
      break;
  }
}

