/*
  Motor.cpp - kecsot
*/

#include "Arduino.h"
#include "Motor.h"
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

void Motor::setInputs(int pinA,int pinB, int chA, int chB)
{
	pinMode(pinA, OUTPUT);
	pinMode(pinB, OUTPUT);

	ledcAttachPin(pinA, chA);
  	ledcSetup(chA, 500, 8);

	ledcAttachPin(pinB, chB);
  	ledcSetup(chB, 500, 8);
	
	_pinA = pinA;
	_pinB = pinB;
	_chA = chA;
	_chB = chB;
}

void Motor::goForward(int pwm)
{
	//digitalWrite(_pinA, HIGH);
	//digitalWrite(_pinB, LOW);
	//analogWrite(_pinA, pwm);
	ledcWrite(_chA, pwm);
	ledcWrite(_chB, 0);
}

void Motor::goBackward(int pwm)
{
	//digitalWrite(_pinA, LOW);
	ledcWrite(_chA, 0);
	//digitalWrite(_pinB, HIGH);
	//analogWrite(_pinB, pwm);
	ledcWrite(_chB, pwm);
}

void Motor::motorstop()
{
	ledcWrite(_chA, 255);
	ledcWrite(_chB, 255);
	//digitalWrite(_pinA, HIGH);
	//digitalWrite(_pinB, HIGH);
}

void Motor::stop()
{
	ledcWrite(_chA, 0);
	ledcWrite(_chB, 0);
	//digitalWrite(_pinA, LOW);
	//digitalWrite(_pinB, LOW);
}





