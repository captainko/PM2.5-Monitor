#ifndef __WAITER_H
#define __WAITER_H

class Waiter
{
  private:
  unsigned long startTime = 0;
  unsigned long delayTime   = 0;
  public:
  Waiter();
  Waiter(unsigned long delay, bool isSecond = false);
  Waiter(unsigned long start, unsigned long delay, bool isSecond = false);
  void setStartTime(unsigned long time);
  void setEndTime(unsigned long time);
  bool isFinished();
  void reset();
};

#endif