#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Arduino.h"
#include "Constants.h"

class TempratureControl
{
public:
  inline TempratureControl() :
    TemperatureSet(Constants::Temperature::DefaultSetTemperature),
    TemperatureDelta(Constants::Temperature::DefaultDeltaTemperature),
    _oneWire(Constants::Temperature::SensorsOneWirePinNumber),
    _sensors(&_oneWire),
    LastUpdatedIndex(0)
  {
    for (uint8_t k = 0; k < NumberOfSensors; k++)
    {
      TemperatureMeasurements[k] = 0.0f;
      IsHeating[k] = false;
    }
  };

  inline ~TempratureControl(){};

  inline void Setup()
  {
    // MOSFET control
    for (uint8_t k = 0; k < NumberOfSensors; k++)
    {
      pinMode(Constants::Temperature::HeaterTransistorPinNumbers[k], OUTPUT);
    }
    _sensors.begin();	// Start up the library

    // Check connected sensors
    Serial.print("getDeviceCount() : ");
    Serial.print(_sensors.getDeviceCount());
    Serial.print("\n");

    uint8_t tempSensorsCount = _sensors.getDS18Count();
    Serial.print("getDS18Count() : ");
    Serial.print(tempSensorsCount);
    Serial.print("\n");

    for (uint8_t k = 0; k < tempSensorsCount; k++)
    {
      uint8_t addr[8];
      bool addrOk = _sensors.getAddress(addr, k);
      Serial.print("Get address OK : ");
      Serial.print(addrOk);
      Serial.print("\nAddress : ");
      for (uint8_t q = 0; q < 8; q++)
      {
        Serial.print(addr[q]);
        Serial.print(" - ");
      }
      Serial.print("\n");
      bool setResolutionOk = _sensors.setResolution(addr, 12);
      Serial.print(" Set resolution OK :  ");
      Serial.print(setResolutionOk);
      Serial.print("\n");

      bool requestOk = _sensors.requestTemperaturesByAddress(addr);
      Serial.print(" Temperature request OK :  ");
      Serial.print(requestOk);
      Serial.print("\n");

      Serial.print(" TempC = ");
      float tempC = _sensors.getTempC(addr);
      Serial.print(tempC);
      Serial.print("\n\n");
    }
    
  }

  // Call every ~1 second. At every call we deal with one chanel. The updated chanel index is LastUpdatedIndex
  inline void Loop()
  {
    LastUpdatedIndex++;
    LastUpdatedIndex = LastUpdatedIndex >= NumberOfSensors ? 0 : LastUpdatedIndex;

    TemperatureMeasurements[LastUpdatedIndex] = _sensors.getTempCByIndex(LastUpdatedIndex);
    if (IsHeating[LastUpdatedIndex] && (TemperatureMeasurements[LastUpdatedIndex] >= TemperatureSet + TemperatureDelta))
    {
      IsHeating[LastUpdatedIndex] = false;
    }
    if (!IsHeating[LastUpdatedIndex] && (TemperatureMeasurements[LastUpdatedIndex] <= TemperatureSet - TemperatureDelta))
    {
      IsHeating[LastUpdatedIndex] = true;
    }
    if (TemperatureMeasurements[LastUpdatedIndex] < -126.0f)
    {
      // Disconnected sensors read -127.0C => do not heat
      IsHeating[LastUpdatedIndex] = false;
    }
    digitalWrite(Constants::Temperature::HeaterTransistorPinNumbers[LastUpdatedIndex], IsHeating[LastUpdatedIndex]);

    // Request next temperature
    uint8_t nextIndex = LastUpdatedIndex + 1;
    if (nextIndex >= NumberOfSensors) nextIndex = 0;
    _sensors.requestTemperaturesByIndex(nextIndex);
  }

  static constexpr uint8_t NumberOfSensors = Constants::Temperature::NumberOfSensors;
  float TemperatureSet;
  float TemperatureDelta;
  float TemperatureMeasurements[NumberOfSensors];
  bool IsHeating[NumberOfSensors];
  uint8_t LastUpdatedIndex;

private:
  OneWire _oneWire;	
  DallasTemperature _sensors;
};



