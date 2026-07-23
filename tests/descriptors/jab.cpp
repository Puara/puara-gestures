#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

using namespace puara_gestures;

TEST_CASE(
    "Jab descriptor detects motion range, ties external data, and supports 2D/3D",
    "[descriptors][jab]")
{
  puara_gestures::Jab jab;
  jab.threshold = 1;

  // Use the first x-axis values from imu_data_jab_shake.csv as realistic motion input.
  CHECK(jab.update(5.516652) == Catch::Approx(0.0).margin(1e-6));
  CHECK(jab.update(3.8303106) == Catch::Approx(1.6863414).margin(1e-6));
  CHECK(jab.update(4.2335014) == Catch::Approx(1.6863414).margin(1e-6));
  CHECK(jab.update(-0.5012963) == Catch::Approx(6.0179487).margin(1e-6));
  CHECK(jab.current_value() == Catch::Approx(6.0179487).margin(1e-6));

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Jab tiedJab(&tiedData);
  tiedJab.threshold = 1;
  tiedData.x = 0.0;
  tiedJab.update();
  tiedData.x = 4.0;
  tiedJab.update();
  CHECK(tiedJab.current_value() == Catch::Approx(4.0).margin(1e-6));

  puara_gestures::Jab2D jab2d;
  jab2d.threshold(1);
  jab2d.update(0.0, 0.0);
  jab2d.update(3.0, -3.0);
  auto values2d = jab2d.current_value();
  CHECK(values2d.x == Catch::Approx(3.0).margin(1e-6));
  CHECK(values2d.y == Catch::Approx(3.0).margin(1e-6));

  puara_gestures::Jab3D jab3d;
  jab3d.threshold(1);
  jab3d.update(0.0, 0.0, 0.0);
  jab3d.update(3.0, -3.0, 1.5);
  auto values3d = jab3d.current_value();
  CHECK(values3d.x == Catch::Approx(3.0).margin(1e-6));
  CHECK(values3d.y == Catch::Approx(3.0).margin(1e-6));
  CHECK(values3d.z == Catch::Approx(1.5).margin(1e-6));
}
