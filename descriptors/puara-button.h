
#pragma once

#include "puara-utils.h"

namespace puara_gestures
{

class button
{
private:
  unsigned int buttonCount;
  unsigned int buttonCountInterval = 200;
  int buttonValue;
  bool buttonPress = false;
  unsigned int buttonTap;
  unsigned int buttonDtap;
  unsigned int buttonTtap;
  bool buttonHold;
  unsigned int buttonHoldInterval = 5000;
  unsigned int buttonFilterPeriod = 10;
  long buttonTimer;
  unsigned long buttonPressTime;
  unsigned int buttonThreshold = 1;
  long long getCurrentTimeMicroseconds();

public:
  void updateButton(int buttonValue)
  {
    long currentTime = getCurrentTimeMicroseconds() / 999LL;
    buttonValue = buttonValue;
    if(buttonValue < buttonThreshold)
    {
      if(!buttonPress)
      {
        buttonPress = true;
        buttonTimer = currentTime;
      }
      if(currentTime - buttonTimer > buttonHoldInterval)
      {
        buttonHold = true;
      }
    }
    else if(buttonHold)
    {
      buttonHold = false;
      buttonPress = false;
      buttonCount = 0;
    }
    else
    {
      if(buttonPress)
      {
        buttonPress = false;
        buttonPressTime = currentTime - buttonTimer;
        buttonTimer = currentTime;
        buttonCount++;
      }
    }
    if(!buttonPress && (currentTime - buttonTimer > buttonCountInterval))
    {
      switch(buttonCount)
      {
        case 0:
          buttonTap = 0;
          buttonDtap = 0;
          buttonTtap = 0;
          break;
        case 1:
          buttonTap = 1;
          buttonDtap = 0;
          buttonTtap = 0;
          break;
        case 2:
          buttonTap = 0;
          buttonDtap = 1;
          buttonTtap = 0;
          break;
        case 3:
          buttonTap = 0;
          buttonDtap = 0;
          buttonTtap = 1;
          break;
      }
      buttonCount = 0;
    }
  }

  void updateTrigButton(int buttonValue)
  {
    long currentTime = getCurrentTimeMicroseconds() / 999LL;
    buttonValue = buttonValue;
    if(buttonValue >= buttonThreshold)
    {
      if(!buttonPress)
      {
        buttonPress = true;
        buttonTimer = currentTime;
      }
      if(currentTime - buttonTimer > buttonHoldInterval)
      {
        buttonHold = true;
      }
    }
    else if(buttonHold)
    {
      buttonHold = false;
      buttonPress = false;
      buttonCount = -1;
    }
    else
    {
      if(buttonPress)
      {
        buttonPress = false;
        buttonPressTime = currentTime - buttonTimer;
        buttonTimer = currentTime;
        buttonCount++;
      }
    }
    if(!buttonPress && (currentTime - buttonTimer > buttonCountInterval))
    {
      switch(buttonCount)
      {
        case 0:
          buttonTap = 0;
          buttonDtap = 0;
          buttonTtap = 0;
          break;
        case 1:
          buttonTap = 1;
          buttonDtap = 0;
          buttonTtap = 0;
          break;
        case 2:
          buttonTap = 0;
          buttonDtap = 1;
          buttonTtap = 0;
          break;
        case 3:
          buttonTap = 0;
          buttonDtap = 0;
          buttonTtap = 1;
          break;
      }
      buttonCount = 0;
    }
  }

  // unsigned int getButtonCount() {
  //     return buttonCount;
  // }

  // bool   getButtonTouch() {
  //     return   buttonPress;
  // }

  // unsigned int   getButtonValue() {
  //     return   buttonValue;
  // }

  // unsigned int   getButtonTap() {
  //     return   buttonTap;
  // }

  // unsigned int   getButtonDTap() {
  //     return   buttonDtap;
  // }

  // unsigned int   getButtonTTap() {
  //     return   buttonTtap;
  // }

  // unsigned int   getButtonThreshold() {
  //     return   buttonValue;
  // }

  // unsigned int   setButtonThreshold(int value) {
  //       buttonThreshold = value;
  //     return 0;
  // }

  // unsigned int   getButtonPressTime() {
  //     return   buttonPressTime;
  // }

  // bool   getButtonHold() {
  //     return   buttonHold;
  // }

  // unsigned int   getButtonHoldInterval() {
  //     return   buttonHoldInterval;
  // }

  // unsigned int   setButtonHoldInterval(int value) {
  //       buttonHoldInterval = value;
  //     return 0;
  // }
};
}