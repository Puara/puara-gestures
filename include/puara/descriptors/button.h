
#pragma once

#include <puara/utils.h>

namespace puara_gestures
{
  /**
   * @file button.h
   * @brief Simple helper for discrete button inputs to identify taps, double 
   * taps, triple taps, counts and holds.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/button.h>
   *
   * int button_data = 0;
   * puara_gestures::Button button(&button_data); // Tied to external button_data variable
   *
   * void setup() {
   *   pinMode(BUTTON_PIN, INPUT_PULLUP);
   * }
   *
   * void loop() {
   *   button_data = digitalRead(BUTTON_PIN);
   *   button.update(); // Reads from tied button_data automatically
   *
   *   Serial.print("tap: %u ", button.tap);
   *   Serial.print("doubleTap: %u ", button.doubleTap);
   *   Serial.print("tripleTap: %u ", button.tripleTap);
   *   Serial.print("hold: %u ", button.hold);
   *   Serial.print("pressTime: %u ", button.pressTime);
   *   Serial.print("count: %u\n", button.count);
   * }
   * @endcode
   *
   * The optional `tied_data` pointer lets the button instance read its
   * input value from an external integer each time `update()` is called.
   * In this example, `button_data` is updated before `button.update()` and
   * the button object uses the latest value automatically.
   *
   * If you prefer not to use `tied_data`, call `button.update(value)`
   * directly with the input value.
   */
class Button
{
private:
  // long long getCurrentTimeMicroseconds();
  long timer = 0;
  const int* tied_data{};

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
  : tied_data(tied)
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
    long currentTime = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
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
};
}
