#include <puara/gestures.h>

#include <iostream>

using namespace puara_gestures;

int main()
{
  Touch touch;

  int touchSize = 16;
  int discrete_touch[16] = {0};

  // simulate a touch (1) at a position 5
  discrete_touch[5] = 1;

  // Update the touch data
  touch.updateTouchArray(discrete_touch, touchSize);

  // Output the computed values
  std::cout << "touchAll: " << touch.touchAll << std::endl;
  std::cout << "brush: " << touch.brush << std::endl;
  std::cout << "rub: " << touch.rub << std::endl;

  // multi touch
}
