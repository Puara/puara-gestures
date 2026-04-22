
#pragma once

#include <puara/utils.h>

namespace puara_gestures
{

  /**
   * @file button.h
   * @brief Simple button descriptor helper for discrete button inputs.
   *
   * Example usage:
   * @code
   * #include <puara/descriptors/button.h>
   *
   * int button_data = 0;
   * puara_gestures::Button button(button_data);
   *
   * void setup() {
   *   pinMode(BUTTON_PIN, INPUT_PULLUP);
   * }
   *
   * void loop() {
   *   button_data = digitalRead(BUTTON_PIN);
   *   button.update();
   *
   *   if (button.tap) {
   *     Serial.println("Button tapped");
   *   }
   *   if (button.doubleTap) {
   *     Serial.println("Button double tapped");
   *   }
   *   if (button.tripleTap) {
   *     Serial.println("Button triple tapped");
   *   }
   *   if (button.hold) {
   *     Serial.println("Button is held");
   *   }
   *
   *   // How long the button was pressed before release.
   *   Serial.print("Press time: ");
   *   Serial.println(button.pressTime);
   *
   *   // The internal press count that drives tap/doubleTap/tripleTap logic.
   *   Serial.print("Press count: ");
   *   Serial.println(button.count);
   *
   *   // Optional tuning for how long a press must last to become a hold.
   *   button.holdInterval = 5000; // milliseconds
   * }
   * @endcode
   *
   * This class can also be used without a tied variable by calling
   * `button.update(value)` directly.
   *
   * The optional `tied_data` pointer allows the user to keep a shared
   * integer state that is updated before each `update()` call.
   */
class Button
{
private:
  // long long getCurrentTimeMicroseconds();
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
    long currentTime = puara_gestures::utils::getCurrentTimeMicroseconds() / 999LL;
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
