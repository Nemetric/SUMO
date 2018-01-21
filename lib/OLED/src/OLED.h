

#ifndef OLED_h
#define OLED_h

#include "Arduino.h"

class OLED
{
public:
  OLED(){};

  void init();
  void writeHelloWold();
  void Reset();
  void WriteString(String txt);
  void WriteInt(int txt);

private:
};

#endif
