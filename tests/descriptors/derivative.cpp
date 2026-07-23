#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Derivative computes velocity, acceleration, and jerk",
          "[descriptors][derivative]")
{
  Derivative d;

  // First sample anchors; no derivative yet.
  d.update(0.0, 0.1);
  CHECK(d.velocity == Catch::Approx(0.0).margin(1e-9));

  // Move 1.0 over 0.1 s -> velocity 10.
  d.update(1.0, 0.1);
  CHECK(d.position == Catch::Approx(1.0).margin(1e-9));
  CHECK(d.velocity == Catch::Approx(10.0).margin(1e-9));

  // Move another 1.0 over 0.1 s -> velocity still 10, acceleration 0.
  d.update(2.0, 0.1);
  CHECK(d.velocity == Catch::Approx(10.0).margin(1e-9));
  CHECK(d.acceleration == Catch::Approx(0.0).margin(1e-9));

  // Jump to 5.0 over 0.1 s -> velocity 30, acceleration (30-10)/0.1 = 200.
  d.update(5.0, 0.1);
  CHECK(d.velocity == Catch::Approx(30.0).margin(1e-9));
  CHECK(d.acceleration == Catch::Approx(200.0).margin(1e-9));
}

TEST_CASE("Derivative ties external data", "[descriptors][derivative]")
{
  double position = 0.0;
  Derivative d;
  CHECK(d.tie(&position) == 1);

  position = 2.0;
  CHECK(d.update() == 1);  // first (clock) sample anchors

  Derivative untied;
  CHECK(untied.update() == 0);
}
