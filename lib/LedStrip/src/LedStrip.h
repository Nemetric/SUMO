#pragma once

#include "Arduino.h"

class LedStrip
{
  public:
    LedStrip();
    void Init();
    
    void HeadlightsON();


    void HeadlightsRandom();


    void LightColor(int indx, int r, int g, int b);

    void LightsOff();
  
};

