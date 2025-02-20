
#pragma once

#include <puara/utils.h>

namespace puara_gestures
{

  /**
 * @brief This class extract several features from a discrete value, e.g., a button.
 * 
 * It expects a discete value (an int with value 0 or 1).
 *
 * Button can use `tied_data`, which is an external variable that users update on their own.
 * Users can then call update() without any argumments to extract the features from the `tied_data`.
* The usage should be:
* setup:
*   - user creates variable, e.g. button_data
* user instantiates the class, e.g. puara::Button my_button(button_data)
* loop/task:
*   - user saves sensor value into button_data
*   - user/task calls my_button.update()
*   - user accesses button descriptors with my_button functions
 */
class Button
{
private:
  long long getCurrentTimeMicroseconds();
  long timer = 0;
  const int* tied_data = 0;

public:
  Button() noexcept
  : tied_data(nullptr)
  {
  }

  Button(const Button&) noexcept = default;
  Button(Button&&) noexcept = default;
  Button& operator=(const Button&) noexcept = default;
  Button& operator=(Button&&) noexcept = default;

  explicit Button(int* tied)
  : threshold(5)
  , tied_data(tied)
  {
  }

  unsigned int count = 0;
  bool press = false;
  unsigned int tap = 0;
  unsigned int doubleTap = 0;
  unsigned int tripleTap = 0;
  bool hold = false;
  unsigned int pressTime = 0;
  unsigned int threshold = 1;
  unsigned int countInterval = 200;
  unsigned int holdInterval = 5000;

  void update(int value)
  {
    long currentTime = getCurrentTimeMicroseconds() / 999LL;
    if(value < threshold)
    {
      if(!press)
      {
        press = true;
        timer = currentTime;
      }
      if(currentTime - timer > holdInterval)
      {
        hold = true;
      }
    }
    else if(hold)
    {
      hold = false;
      press = false;
      count = 0;
    }
    else
    {
      if(press)
      {
        press = false;
        pressTime = currentTime - timer;
        timer = currentTime;
        count++;
      }
    }
    if(!press && (currentTime - timer > countInterval))
    {
      switch(count)
      {
        case 0:
          tap = 0;
          doubleTap = 0;
          tripleTap = 0;
          break;
        case 1:
          tap = 1;
          doubleTap = 0;
          tripleTap = 0;
          break;
        case 2:
          tap = 0;
          doubleTap = 1;
          tripleTap = 0;
          break;
        case 3:
          tap = 0;
          doubleTap = 0;
          tripleTap = 1;
          break;
      }
      count = 0;
    }
  }

  int update()
  {
    if(tied_data != nullptr)
    {
      Button::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  void updateTrigButton(int value)
  {
    long currentTime = getCurrentTimeMicroseconds() / 999LL;
    value = value;
    if(value >= threshold)
    {
      if(!press)
      {
        press = true;
        timer = currentTime;
      }
      if(currentTime - timer > holdInterval)
      {
        hold = true;
      }
    }
    else if(hold)
    {
      hold = false;
      press = false;
      count = -1;
    }
    else
    {
      if(press)
      {
        press = false;
        pressTime = currentTime - timer;
        timer = currentTime;
        count++;
      }
    }
    if(!press && (currentTime - timer > countInterval))
    {
      switch(count)
      {
        case 0:
          tap = 0;
          doubleTap = 0;
          tripleTap = 0;
          break;
        case 1:
          tap = 1;
          doubleTap = 0;
          tripleTap = 0;
          break;
        case 2:
          tap = 0;
          doubleTap = 1;
          tripleTap = 0;
          break;
        case 3:
          tap = 0;
          doubleTap = 0;
          tripleTap = 1;
          break;
      }
      count = 0;
    }
  }

  int updateTrigButton()
  {
    if(tied_data != nullptr)
    {
      Button::updateTrigButton(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }
};
}
