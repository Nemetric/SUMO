/*
  Motor.h - kecsot
*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor
{
  public:
    Motor(){};
    void setInputs(int pinA,int pinB);
	void goForward(int pwm);
	void goBackward(int pwm);
	void motorstop();
	void stop();
  private:
    int _pinA;
	int _pinB;
};


#endif

