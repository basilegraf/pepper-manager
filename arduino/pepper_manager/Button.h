#pragma once
#include <Arduino.h>

template<class TParent>
class Button
{
  public:

  // Define type ParentMethod for a pointer to a parent class method
  typedef void (TParent::*ParentMethod)(void);

  inline Button(TParent& parent, uint8_t pinNumber) :
    ParentPtr(&parent),
    LowToHighCallback(nullptr),
    HighToLowCallback(nullptr),
    LongLowCallback(nullptr),
    LongHighCallback(nullptr),
    _pinNumber(pinNumber),
    _count(0),
    _state(false),
    _lastValue(false),
    _isInit(true)
  {};

  inline ~Button(){};

  void Setup(ParentMethod lowToHigh, ParentMethod highToLow)
  {
    pinMode(_pinNumber, INPUT_PULLUP);
    LowToHighCallback = lowToHigh;
    HighToLowCallback = highToLow;
  }

  inline void Loop()
  {
    bool value = digitalRead(_pinNumber);
    if (_isInit)
    {
      _count++;
      if (_count >= _requiredCounts)
      {
        _count = 0;
        _lastValue = value;
        _state = value;
        _isInit = false;
      }
      return;
    }
    else if (value == _lastValue)
    {
      _count++;

      if ((_count >= _requiredCounts) && (value != _state))
      {
        _state = value;
        if (_state)
        {
          if (LowToHighCallback != nullptr) (ParentPtr->*LowToHighCallback)();
        }
        else
        {
          if (HighToLowCallback != nullptr) (ParentPtr->*HighToLowCallback)();
        }
      }
      else if (_count >= _longPressCounts)
      {
        if (_state)
        {
          if (LongHighCallback != nullptr) (ParentPtr->*LongHighCallback)();
        }
        else
        {
          if (LongLowCallback != nullptr) (ParentPtr->*LongLowCallback)();
        }
        _count = 0;
      }
      
    }
    else
    {
      _count = 0;
      _lastValue = value;
    }
  }

  inline bool GetState() {return _state;};

  TParent* ParentPtr;
  ParentMethod LowToHighCallback;
  ParentMethod HighToLowCallback;
  ParentMethod LongLowCallback;
  ParentMethod LongHighCallback;

  private:
  static constexpr unsigned int _requiredCounts = 10; // Top value for debounce counter
  static constexpr unsigned long _longPressCounts = 50000;
  uint8_t _pinNumber;
  unsigned long _count;
  bool _state;
  bool _lastValue;
  bool _isInit; // TODO : replace booleans with bits from a _flags integers
};