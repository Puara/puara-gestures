#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <algorithm>

using namespace puara_gestures;

TEST_CASE(
    "Touch descriptor computes expected averages and gesture values",
    "[descriptors][touch]")
{
  constexpr int maxNumBlobs = 4;
  constexpr int touchSizeEdge = 4;
  constexpr int touchSize = 16;

  TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> touchArrayGD;
  int touchArray[touchSize] = {0};

  touchArray[0] = 1;
  touchArray[5] = 1;
  touchArray[6] = 1;
  touchArray[8] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[14] = 1;
  touchArray[15] = 1;

  touchArrayGD.update(touchArray, touchSize);

  CHECK(touchArrayGD.totalTouchAverage == Catch::Approx(0.5).margin(1e-3));
  CHECK(touchArrayGD.topTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.middleTouchAverage == Catch::Approx(0.625).margin(1e-3));
  CHECK(touchArrayGD.bottomTouchAverage == Catch::Approx(0.5).margin(1e-3));
  CHECK(touchArrayGD.totalBrush >= 0.0f);
  CHECK(touchArrayGD.totalRub >= 0.0f);

  std::fill(std::begin(touchArray), std::end(touchArray), 0);
  touchArray[1] = 1;
  touchArray[6] = 1;
  touchArray[7] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[11] = 1;
  touchArray[15] = 1;

  touchArrayGD.update(touchArray, touchSize);

  CHECK(touchArrayGD.totalTouchAverage == Catch::Approx(0.4375).margin(1e-3));
  CHECK(touchArrayGD.topTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.middleTouchAverage == Catch::Approx(0.625).margin(1e-3));
  CHECK(touchArrayGD.bottomTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.totalBrush >= 0.0f);
  CHECK(touchArrayGD.totalRub >= 0.0f);
}
