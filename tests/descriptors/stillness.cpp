#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Stillness detects still onset and motion resume", "[descriptors][stillness]")
{
  Stillness stillness;
  stillness.threshold = 0.1;
  stillness.settleTime = 1000;  // long, so `settled` stays false in this test

  // Moving: not still.
  CHECK(stillness.update(1.0) == 0);
  CHECK(stillness.still == false);

  // Drops below threshold: stillness begins.
  CHECK(stillness.update(0.05) == 1);
  CHECK(stillness.still == true);
  CHECK(stillness.onset == true);
  CHECK(stillness.moved == false);

  // Continues still: no repeated onset.
  stillness.update(0.02);
  CHECK(stillness.still == true);
  CHECK(stillness.onset == false);

  // Motion resumes.
  stillness.update(2.0);
  CHECK(stillness.still == false);
  CHECK(stillness.moved == true);
  CHECK(stillness.stillTime == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("Stillness flags settled after the settle time", "[descriptors][stillness]")
{
  Stillness stillness;
  stillness.threshold = 0.1;
  stillness.settleTime = 20;  // ms

  stillness.update(0.0);  // still onset
  CHECK(stillness.still == true);
  CHECK(stillness.settled == false);  // not yet long enough

  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  stillness.update(0.0);
  CHECK(stillness.settled == true);
  CHECK(stillness.stillTime >= 20.0);
}

TEST_CASE("Stillness ties external data", "[descriptors][stillness]")
{
  double activity = 0.0;
  Stillness stillness;
  stillness.threshold = 0.1;
  CHECK(stillness.tie(&activity) == 1);

  activity = 0.0;
  CHECK(stillness.update() == 1);
  CHECK(stillness.still == true);

  Stillness untied;
  CHECK(untied.update() == 0);
}
