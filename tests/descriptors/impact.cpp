#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Impact detector fires once per strike and reports peak intensity",
          "[descriptors][impact]")
{
  Impact impact;
  impact.threshold = 2.0;
  impact.releaseThreshold = 1.0;
  impact.minInterval = 0;  // hysteresis-only: fully deterministic

  // Below threshold: no strike.
  impact.update(0.5);
  CHECK(impact.strike == false);
  CHECK(impact.count == 0);

  // Rising to threshold fires exactly one strike on the onset.
  impact.update(3.0);
  CHECK(impact.strike == true);
  CHECK(impact.count == 1);
  CHECK(impact.intensity == Catch::Approx(3.0).margin(1e-9));

  // Still above threshold: no re-trigger, but intensity tracks the peak.
  impact.update(4.0);
  CHECK(impact.strike == false);
  CHECK(impact.count == 1);
  CHECK(impact.intensity == Catch::Approx(4.0).margin(1e-9));

  impact.update(3.0);
  CHECK(impact.strike == false);
  CHECK(impact.intensity == Catch::Approx(4.0).margin(1e-9));  // peak held

  // Falling below releaseThreshold re-arms the detector.
  impact.update(0.5);
  CHECK(impact.strike == false);

  // Next spike fires a second strike.
  impact.update(5.0);
  CHECK(impact.strike == true);
  CHECK(impact.count == 2);
  CHECK(impact.intensity == Catch::Approx(5.0).margin(1e-9));
}

TEST_CASE("Impact detector honours the minInterval refractory window",
          "[descriptors][impact]")
{
  Impact impact;
  impact.threshold = 2.0;
  impact.releaseThreshold = 1.0;
  impact.minInterval = 50;  // ms

  impact.update(3.0);
  CHECK(impact.strike == true);
  CHECK(impact.count == 1);

  // Drops below release, but the refractory window has not elapsed: no re-arm.
  impact.update(0.0);
  impact.update(3.0);
  CHECK(impact.strike == false);
  CHECK(impact.count == 1);

  // After the refractory window, a fresh strike is accepted.
  std::this_thread::sleep_for(std::chrono::milliseconds(60));
  impact.update(0.0);  // below release + interval elapsed -> re-arm
  impact.update(3.0);
  CHECK(impact.strike == true);
  CHECK(impact.count == 2);
}

TEST_CASE("Impact detector ties external data", "[descriptors][impact]")
{
  double magnitude = 0.0;
  Impact impact;
  impact.threshold = 2.0;
  impact.releaseThreshold = 1.0;
  impact.minInterval = 0;
  CHECK(impact.tie(&magnitude) == 1);

  magnitude = 3.0;
  CHECK(impact.update() == 1);
  CHECK(impact.strike == true);
  CHECK(impact.count == 1);
  CHECK(impact.current_value() == Catch::Approx(3.0).margin(1e-9));

  Impact untied;
  CHECK(untied.update() == 0);
}
