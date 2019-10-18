#include "Waiter.h"
#include "Arduino.h"

Waiter::Waiter()
{
  startTime = millis();
}

Waiter::Waiter(unsigned long delay, bool isSecond = 0)
{
  startTime = millis();
  delayTime = isSecond ? delay * 1000 : delay;
}

Waiter::Waiter(unsigned long start, unsigned long delay = 0, bool isSecond = 0)
{
  if (isSecond)
  {
    start = start * 1000;
    delay = delay * 1000;
  }

  startTime = start;
  delayTime = delay;
}

boolean Waiter::isFinished()
{
  return (millis() - startTime) >= delayTime;
}

void Waiter::reset()
{
  startTime = millis();
}