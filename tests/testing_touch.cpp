#include <puara/gestures.h>

#include <iostream>

using namespace puara_gestures;

int main()
{
  Touch touch;

  const int touchSize = 16;
  int discrete_touch[touchSize] = {0};

  // simulate a blob of size 2 starting at position 5
  discrete_touch[5] = 1;
  discrete_touch[6] = 1;

  // Update the touch data
  touch.updateTouchArray(discrete_touch, touchSize);

  // Output the computed values
  std::cout << "touchAll: " << touch.touchAll << std::endl;
  std::cout << "brush: " << touch.brush << std::endl;
  std::cout << "rub: " << touch.rub << std::endl;

  // multi touch
}
