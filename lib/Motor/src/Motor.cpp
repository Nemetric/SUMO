/*
  Motor.cpp - kecsot
*/

#include "Arduino.h"
#include "Motor.h"
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

void Motor::setInputs(int pinA,int pinB)
{
	pinMode(pinA, OUTPUT);
	pinMode(pinB, OUTPUT);
	
	_pinA = pinA;
	_pinB = pinB;
}

void Motor::goForward(int pwm)
{
	digitalWrite(_pinA, HIGH);
	digitalWrite(_pinB, LOW);
	//analogWrite(_pinA, pwm);
}

void Motor::goBackward(int pwm)
{
	digitalWrite(_pinA, LOW);
	digitalWrite(_pinB, HIGH);
	//analogWrite(_pinB, pwm);
}

void Motor::motorstop()
{
	digitalWrite(_pinA, HIGH);
	digitalWrite(_pinB, HIGH);
}

void Motor::stop()
{
	digitalWrite(_pinA, LOW);
	digitalWrite(_pinB, LOW);
}





