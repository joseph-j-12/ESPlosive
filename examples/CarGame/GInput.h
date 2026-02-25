#ifndef HEADER_GINPUT
#define  HEADER_GINPUT

#include "Arduino.h"

class GInput_JoyStick{
  public:
  float horizontal = 0.f;
  float vertical = 0.f;

  float maxAnalogVal = 4096;
  float minAnalogVal = 0;

  float deadZoneH = 0.f;
  float errorH = 0.f;

  float deadZoneV = 0.f;
  float errorV = 0.f;

  GInput_JoyStick(int pinh, int pinv)
  {
    pin1 = pinv;
    pin2 = pinh;
  }

  void Tick()
  {
    float valh = mapfloat(analogRead(pin2), minAnalogVal, maxAnalogVal, -1.f, 1.f);
    float valv = mapfloat(analogRead(pin1), minAnalogVal, maxAnalogVal, -1.f, 1.f);


    if ((valh <= deadZoneH + errorH) && (valh >= errorH - deadZoneH))
    {
      horizontal = 0;
    }
    else
    {
      if (valh < errorH - deadZoneH)
      {
        horizontal = -(valh-errorH+deadZoneH)/(-1-errorH+deadZoneH);
      }
      else if (valh > deadZoneH + errorH)
      {
        horizontal = (valh-errorH-deadZoneH)/(1-errorH-deadZoneH);
      }
    }
    if ((valv <= deadZoneV + errorV) && (valv >= errorV - deadZoneV))
    {
      vertical = 0;
    }
    else
    {
      if (valv < errorV - deadZoneV)
      {
        vertical = -(valv-errorV+deadZoneV)/(-1-errorV+deadZoneV);
      }
      else if (valv > deadZoneV + errorV)
      {
        vertical = (valv-errorV-deadZoneV)/(1-errorV-deadZoneV);
      }
    }
  }
  private:
  int pin1;
  int pin2;

  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  // float errorV_right = 0.f;


};
class GInput_Button{
  public:
  //button states
  enum ButtonState
  {
    NOT_PRESSED,
    ONCLICK,
    ONHOLD,
    ONRELEASE,
  };

  int pin;  
  bool myPressedState = false;
  GInput_Button(const int buttonPin, bool pressedState = false)
  {
    pin = buttonPin;
    myButtonState = ButtonState::NOT_PRESSED;
    myPressedState = pressedState;
    prevState = !myPressedState;
  }

  void Tick()
  {
    bool val = digitalRead(pin);
    if (val == prevState)
    {
      if (val == myPressedState)
      {
        myButtonState = ButtonState::ONHOLD;
      }
      else
        myButtonState = ButtonState::NOT_PRESSED;
    }
    else
    {
      if (val == myPressedState)
      {
        myButtonState = ButtonState::ONCLICK;
      }
      else
      {
        myButtonState = ButtonState::ONRELEASE;
      }
      prevState = val;
    }
    // Serial.println(myButtonState);
  }
  ButtonState myButtonState;

  private:
  bool prevState;  
};
#endif