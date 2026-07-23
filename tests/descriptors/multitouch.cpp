#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("MultiTouch reports distance, center, scale and spread",
          "[descriptors][multitouch]")
{
  MultiTouch touch;

  // First update anchors the reference: distance 2, scale 1.
  touch.update(Coord2D{0.0, 0.0}, Coord2D{2.0, 0.0});
  CHECK(touch.distance == Catch::Approx(2.0).margin(1e-9));
  CHECK(touch.center.x == Catch::Approx(1.0).margin(1e-9));
  CHECK(touch.center.y == Catch::Approx(0.0).margin(1e-9));
  CHECK(touch.scale == Catch::Approx(1.0).margin(1e-9));

  // Spread the fingers to distance 4: scale doubles, spread +2.
  touch.update(Coord2D{0.0, 0.0}, Coord2D{4.0, 0.0});
  CHECK(touch.distance == Catch::Approx(4.0).margin(1e-9));
  CHECK(touch.scale == Catch::Approx(2.0).margin(1e-9));
  CHECK(touch.spread == Catch::Approx(2.0).margin(1e-9));
  CHECK(touch.distanceDelta == Catch::Approx(2.0).margin(1e-9));

  // Pinch back to distance 1: scale 0.5.
  touch.update(Coord2D{0.0, 0.0}, Coord2D{1.0, 0.0});
  CHECK(touch.scale == Catch::Approx(0.5).margin(1e-9));
}

TEST_CASE("MultiTouch accumulates rotation as the pair twists",
          "[descriptors][multitouch]")
{
  MultiTouch touch;
  const double pi = std::acos(-1.0);

  // Anchor along the x-axis (angle 0).
  touch.update(Coord2D{0.0, 0.0}, Coord2D{2.0, 0.0});

  // Rotate the pair to the y-axis: +90 degrees.
  touch.update(Coord2D{0.0, 0.0}, Coord2D{0.0, 2.0});
  CHECK(touch.rotation == Catch::Approx(pi / 2.0).margin(1e-9));
  CHECK(touch.rotationDelta == Catch::Approx(pi / 2.0).margin(1e-9));

  // Continue to the negative x-axis: another +90, total +180.
  touch.update(Coord2D{0.0, 0.0}, Coord2D{-2.0, 0.0});
  CHECK(touch.rotation == Catch::Approx(pi).margin(1e-9));
}

TEST_CASE("MultiTouch anchor() re-references the current geometry",
          "[descriptors][multitouch]")
{
  MultiTouch touch;
  touch.update(Coord2D{0.0, 0.0}, Coord2D{2.0, 0.0});  // reference dist 2
  touch.update(Coord2D{0.0, 0.0}, Coord2D{4.0, 0.0});  // scale 2

  touch.anchor();  // re-reference at distance 4
  CHECK(touch.scale == Catch::Approx(1.0).margin(1e-9));
  CHECK(touch.rotation == Catch::Approx(0.0).margin(1e-9));

  touch.update(Coord2D{0.0, 0.0}, Coord2D{8.0, 0.0});
  CHECK(touch.scale == Catch::Approx(2.0).margin(1e-9));
}

TEST_CASE("MultiTouch ties external touch points", "[descriptors][multitouch]")
{
  Coord2D a{0.0, 0.0}, b{3.0, 4.0};
  MultiTouch touch;
  CHECK(touch.tie(&a, &b) == 1);

  CHECK(touch.update() == 1);
  CHECK(touch.distance == Catch::Approx(5.0).margin(1e-9));  // 3-4-5 triangle

  MultiTouch untied;
  CHECK(untied.update() == 0);
}
