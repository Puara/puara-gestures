#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

using namespace puara_gestures;

TEST_CASE(
    "Simple Tilt/Roll descriptor computes roll and tilt from accelerometer data",
    "[descriptors][simple_tilt_roll]")
{
  puara_gestures::Tilt_Roll simple;
  simple.update(0.0, 0.0, 1.0);

  CHECK(simple.current_roll_value() == Catch::Approx(M_PI_2).margin(1e-6));
  CHECK(simple.current_tilt_value() == Catch::Approx(0.0).margin(1e-6));

  simple.update(1.0, 0.0, 0.0);
  CHECK(simple.current_roll_value() == Catch::Approx(0.0).margin(1e-6));
  CHECK(simple.current_tilt_value() == Catch::Approx(M_PI_2).margin(1e-3));

  Coord3D imu_data{0.0, 0.0, 1.0};
  Tilt_Roll tied(&imu_data);
  tied.update();
  CHECK(tied.current_roll_value() == Catch::Approx(M_PI_2).margin(1e-6));
  CHECK(tied.current_tilt_value() == Catch::Approx(0.0).margin(1e-6));
}
