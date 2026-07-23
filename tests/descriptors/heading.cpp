#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Heading computes a level compass bearing", "[descriptors][heading]")
{
  Heading heading;

  // Device flat (gravity on +z). Magnetic north along +x -> heading 0.
  Coord3D level{0.0, 0.0, 1.0};
  CHECK(heading.update(level, Coord3D{1.0, 0.0, 0.0}) == Catch::Approx(0.0).margin(1e-6));

  // Field along -y -> heading 90 deg with this convention.
  CHECK(heading.update(level, Coord3D{0.0, -1.0, 0.0}) == Catch::Approx(90.0).margin(1e-6));

  // Field along -x -> heading 180 deg.
  CHECK(heading.update(level, Coord3D{-1.0, 0.0, 0.0}) == Catch::Approx(180.0).margin(1e-6));

  // Heading is always wrapped into [0, 360).
  CHECK(heading.heading >= 0.0);
  CHECK(heading.heading < 360.0);
}

TEST_CASE("Heading applies magnetic declination", "[descriptors][heading]")
{
  Heading heading;
  heading.declination = 10.0;

  Coord3D level{0.0, 0.0, 1.0};
  CHECK(heading.update(level, Coord3D{1.0, 0.0, 0.0}) == Catch::Approx(10.0).margin(1e-6));
}

TEST_CASE("Heading is tilt-compensated for roll", "[descriptors][heading]")
{
  Heading heading;

  // Level, north along +x -> heading 0.
  double level = heading.update(Coord3D{0.0, 0.0, 1.0}, Coord3D{1.0, 0.0, 0.0});

  // Rolled 90 deg (gravity on +y) but the horizontal field is still +x:
  // compensation should keep the heading at ~0, not swing with the tilt.
  double rolled = heading.update(Coord3D{0.0, 1.0, 0.0}, Coord3D{1.0, 0.0, 0.0});
  CHECK(rolled == Catch::Approx(level).margin(1e-6));
}

TEST_CASE("Heading ties external IMU data", "[descriptors][heading]")
{
  Imu9Axis imu;
  Heading heading;
  CHECK(heading.tie(&imu) == 1);

  imu.accl = Coord3D{0.0, 0.0, 1.0};
  imu.magn = Coord3D{1.0, 0.0, 0.0};
  CHECK(heading.update() == 1);
  CHECK(heading.current_value() == Catch::Approx(0.0).margin(1e-6));

  Heading untied;
  CHECK(untied.update() == 0);
}
