#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

using namespace puara_gestures;

TEST_CASE(
    "Shake descriptor integrates high movement and decays when motion stops",
    "[descriptors][shake]")
{
  puara_gestures::Shake shake;
  shake.threshold = 0.1;
  shake.fast_leak = 0.6;
  shake.slow_leak = 0.3;
  shake.frequency(0);

  CHECK(shake.update(10.0) == Catch::Approx(1.0).margin(1e-6));
  CHECK(shake.current_value() == Catch::Approx(1.0).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.3).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.09).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.027).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.0).margin(1e-6));

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Shake tiedShake(&tiedData);
  tiedShake.threshold = 0.1;
  tiedShake.frequency(0);
  tiedData.x = 0.0;
  tiedShake.update();
  tiedData.x = 10.0;
  tiedShake.update();
  CHECK(tiedData.x == Catch::Approx(10.0).margin(1e-6));
  CHECK(tiedShake.current_value() == Catch::Approx(1.0).margin(1e-6));

  puara_gestures::Shake2D shake2d;
  shake2d.threshold(0.1);
  shake2d.frequency(0);
  shake2d.update(10.0, 5.0);
  auto values2d = shake2d.current_value();
  CHECK(values2d.x == Catch::Approx(1.0).margin(1e-6));
  CHECK(values2d.y == Catch::Approx(0.5).margin(1e-6));

  puara_gestures::Shake3D shake3d;
  shake3d.threshold(0.1);
  shake3d.frequency(0);
  shake3d.update(10.0, 5.0, 2.0);
  auto values3d = shake3d.current_value();
  CHECK(values3d.x == Catch::Approx(1.0).margin(1e-6));
  CHECK(values3d.y == Catch::Approx(0.5).margin(1e-6));
  CHECK(values3d.z == Catch::Approx(0.2).margin(1e-6));
}
