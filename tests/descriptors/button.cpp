#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Button descriptor tracks taps, press time, and hold", "[descriptors][button]")
{
  Button button;
  button.countInterval = 1;
  button.holdInterval = 1000;

  button.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  button.update(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  button.update(0);

  CHECK(button.tap == 1);
  CHECK(button.doubleTap == 0);
  CHECK(button.tripleTap == 0);
  CHECK(button.pressTime > 0);
  CHECK(button.count == 0);

  Button doubleTapButton;
  doubleTapButton.countInterval = 1;
  doubleTapButton.holdInterval = 1000;

  doubleTapButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);
  doubleTapButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);

  CHECK(doubleTapButton.tap == 0);
  CHECK(doubleTapButton.doubleTap == 1);
  CHECK(doubleTapButton.tripleTap == 0);

  Button holdButton;
  holdButton.countInterval = 1000;
  holdButton.holdInterval = 5;

  holdButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  holdButton.update(1);

  CHECK(holdButton.hold == true);
}
