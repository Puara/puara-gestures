#include <puara/gestures.h>
#include <iostream>

using namespace puara_gestures;

int main()
{
  Touch touch;

  constexpr int touchSize = 16;
  int discrete_touch[touchSize] = {0};

  // simulate a blob of size 1 starting at position 0
  discrete_touch[0] = 1;

  // simulate a blob of size 2 starting at position 5
  discrete_touch[5] = 1;
  discrete_touch[6] = 1;

  // simulate a blob of size 3 starting at position 8
  discrete_touch[8] = 1;
  discrete_touch[9] = 1;
  discrete_touch[10] = 1;

  // simulate a blob of size 1 at position 12 -- commented out to test the end of the array in the next blob
  //discrete_touch[12] = 1;

  // simulate a blob of size 2 starting at position 14
  discrete_touch[14] = 1;
  discrete_touch[15] = 1;

  // Update the touch data and print the computed values
  touch.updateTouchArray(discrete_touch, touchSize);
  std::cout << "touchAll: " << touch.touchAll << std::endl;
  std::cout << "brush: " << touch.brush << std::endl;
  std::cout << "rub: " << touch.rub << std::endl;

  //reset the touch array, and next we'll repeat the above blobs but offset by 1 to simulate movement
  for(int i = 0; i < touchSize; ++i)
    discrete_touch[i] = 0;

  // simulate a blob of size 1 starting at position 1
  discrete_touch[1] = 1;

  // simulate a blob of size 2 starting at position 6
  discrete_touch[6] = 1;
  discrete_touch[7] = 1;

  // simulate a blob of size 3 starting at position 9
  discrete_touch[9] = 1;
  discrete_touch[10] = 1;
  discrete_touch[11] = 1;

  // simulate a blob of size 1 at position 13 -- commented out to test the end of the array in the next blob
  //discrete_touch[13] = 1;

  // simulate a blob of size 1 starting at position 15
  discrete_touch[15] = 1;

  // Update the touch data and print the computed values
  touch.updateTouchArray(discrete_touch, touchSize);
  std::cout << "touchAll: " << touch.touchAll << std::endl;
  std::cout << "brush: " << touch.brush << std::endl;
  std::cout << "rub: " << touch.rub << std::endl;
}
