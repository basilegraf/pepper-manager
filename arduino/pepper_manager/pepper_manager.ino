#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

#include "SunHours.h"
#include "DateTime.h"
#include "Button.h"
#include "PepperManager.h"
#include "Constants.h"

PepperManager PepperManager;

byte ovfCounts = 0;
bool needsProcessing = false;

ISR(TIMER1_OVF_vect)
{
  ovfCounts++;
  if (ovfCounts >= 100)
  {
    ovfCounts = 0;
    needsProcessing = true;
  }
  
}
 
void setup()
{
  // Waveform generation mode (See page 109)
  // Use waveform generation mode 8  ==>  TIMER1 counts for 0 to ICR1
  //    Mode 8 : WGM13 = 1
  TCCR1A = B00000000;           // Init Timer1
  TCCR1B = B00010000;           // Init Timer1
  ICR1 = 10000; // TOP count value : 16MHz / 8 / 20000 => 100Hz   ??? Factor2?

  
  TCCR1B |= B00000010;  // Prescalar = 8
  TCNT1 = 40535;        // Timer Preloading
  TIMSK1 |= B00000001;  // Enable Timer Overflow Interrupt

  Serial.begin(9600);
  Serial.print("Setup...\n");
  PepperManager.Setup();
  Serial.print("Setup done.\n");
}

void loop()
{
  bool newSecond = needsProcessing;
  if (needsProcessing)
  {
    needsProcessing = false;
  }

  PepperManager.Loop(newSecond);
}

