

#ifndef OLED_h
#define OLED_h

#include "Arduino.h"

class OLED
{
public:
  OLED(){};

  void init();
  void writeYETIBOTICS();
  void writeNEMETRIC();
  void Reset();
  void WriteString(String txt);
  void WriteString(String txt, int x, int y);
  void WriteInt(int txt);

private:
};

#endif
