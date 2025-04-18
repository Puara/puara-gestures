#include <puara/gestures.h>
#include <iostream>

using namespace puara_gestures;

int main()
{
  constexpr int maxNumBlobs{4};
  constexpr int touchSizeEdge{4};
  TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> touchArrayGD;

  constexpr int touchSize = 16;
  int touchArray[touchSize] = {0};

  // simulate a blob of size 1 starting at position 0
  touchArray[0] = 1;

  // simulate a blob of size 2 starting at position 5
  touchArray[5] = 1;
  touchArray[6] = 1;

  // simulate a blob of size 3 starting at position 8
  touchArray[8] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;

  // simulate a blob of size 1 at position 12 -- commented out to test the end of the array in the next blob
  //touchArray[12] = 1;

  // simulate a blob of size 2 starting at position 14
  touchArray[14] = 1;
  touchArray[15] = 1;

  // Update the touch data and print the computed values
  touchArrayGD.update(touchArray, touchSize);
  std::cout << "totalTouchAverage: " << touchArrayGD.totalTouchAverage << std::endl;
  std::cout << "brush: " << touchArrayGD.totalBrush << std::endl;
  std::cout << "rub: " << touchArrayGD.totalRub << std::endl;

  //reset the touch array, and next we'll repeat the above blobs but offset by 1 to simulate movement
  for(int i = 0; i < touchSize; ++i)
    touchArray[i] = 0;

  // simulate a blob of size 1 starting at position 1
  touchArray[1] = 1;

  // simulate a blob of size 2 starting at position 6
  touchArray[6] = 1;
  touchArray[7] = 1;

  // simulate a blob of size 3 starting at position 9
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[11] = 1;

  // simulate a blob of size 1 at position 13 -- commented out to test the end of the array in the next blob
  //touchArray[13] = 1;

  // simulate a blob of size 1 starting at position 15
  touchArray[15] = 1;

  // Update the touch data and print the computed values
  touchArrayGD.update(touchArray, touchSize);
  std::cout << "totalTouchAverage: " << touchArrayGD.totalTouchAverage << std::endl;
  std::cout << "brush: " << touchArrayGD.totalBrush << std::endl;
  std::cout << "rub: " << touchArrayGD.totalRub << std::endl;
}
