#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Spin integrates angular velocity into angle, turns, and direction",
          "[descriptors][spin]")
{
  Spin spin;
  spin.revolution = 360.0;  // degrees per full turn

  // First sample has no elapsed time: sets velocity, no angle yet.
  spin.update(90.0, 0.0);
  CHECK(spin.velocity == Catch::Approx(90.0).margin(1e-9));
  CHECK(spin.angle == Catch::Approx(0.0).margin(1e-9));
  CHECK(spin.direction == 1);

  // 90 deg/s for 2 s -> 180 deg, half a turn.
  spin.update(90.0, 2.0);
  CHECK(spin.angle == Catch::Approx(180.0).margin(1e-9));
  CHECK(spin.revolutions == Catch::Approx(0.5).margin(1e-9));
  CHECK(spin.turns == 0);

  // Another 90 deg/s for 2 s -> 360 deg total, one full turn.
  spin.update(90.0, 2.0);
  CHECK(spin.angle == Catch::Approx(360.0).margin(1e-9));
  CHECK(spin.turns == 1);

  // Reverse direction.
  spin.update(-90.0, 1.0);
  CHECK(spin.direction == -1);
  CHECK(spin.angle == Catch::Approx(270.0).margin(1e-9));
}

TEST_CASE("Spin deadzone suppresses small rates", "[descriptors][spin]")
{
  Spin spin;
  spin.deadzone = 5.0;

  spin.update(3.0, 0.0);   // within deadzone
  spin.update(3.0, 10.0);  // should not accumulate
  CHECK(spin.velocity == Catch::Approx(0.0).margin(1e-9));
  CHECK(spin.angle == Catch::Approx(0.0).margin(1e-9));
  CHECK(spin.direction == 0);
}

TEST_CASE("Spin ties external data", "[descriptors][spin]")
{
  double gyro = 0.0;
  Spin spin;
  CHECK(spin.tie(&gyro) == 1);

  gyro = 45.0;
  CHECK(spin.update() == 1);
  CHECK(spin.velocity == Catch::Approx(45.0).margin(1e-9));

  Spin untied;
  CHECK(untied.update() == 0);
}

TEST_CASE("Spin3D reports per-axis velocity and overall speed", "[descriptors][spin]")
{
  Spin3D spin3d;
  spin3d.update(3.0, 4.0, 0.0, 1.0);

  auto v = spin3d.current_value();
  CHECK(v.x == Catch::Approx(3.0).margin(1e-9));
  CHECK(v.y == Catch::Approx(4.0).margin(1e-9));
  CHECK(spin3d.magnitude() == Catch::Approx(5.0).margin(1e-9));
}
