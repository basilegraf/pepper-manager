#pragma once


enum class ManagerState : uint8_t
{
  Display = 0, 
  EditDay,
  EditMonth,
  EditHour,
  EditMinutes,
  EditSetTemperature,
  EditArtificialDayLength
};