#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#include "Button.h"
#include "SunHours.h"
#include "DateTime.h"
#include "TemperatureControl.h"
#include "Display.h"
#include "Constants.h"
#include "ManagerState.h"


class PepperManager
{
public:

  typedef Button<PepperManager> ButtonType;
  friend ButtonType;

  PepperManager();
  ~PepperManager();

  void Setup();

  void Loop(bool isNewSecond);

  
  ManagerState State;
  Sun Sun;
  DateTime Date;
  Display Display;
  TempratureControl TempControl;

private:

  void _callbackButtonMinusPress();
  void _callbackButtonEditPress();
  void _callbackButtonEditLongPress();
  void _callbackButtonPlusPress();
  void _updateAfterStateChange();

  

  ButtonType _buttonMinus;
  ButtonType _buttonEdit;
  ButtonType _buttonPlus;
};