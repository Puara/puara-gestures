#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Freefall detects the weightless window and landing",
          "[descriptors][freefall]")
{
  Freefall freefall;
  freefall.threshold = 0.3;

  // Resting at ~1 g: not falling.
  freefall.update(Coord3D{0.0, 0.0, 1.0});
  CHECK(freefall.freefall == false);
  CHECK(freefall.magnitude == Catch::Approx(1.0).margin(1e-9));

  // Near weightless: free-fall begins.
  freefall.update(Coord3D{0.1, 0.0, 0.1});
  CHECK(freefall.freefall == true);
  CHECK(freefall.onset == true);

  // Still falling: no repeated onset.
  freefall.update(Coord3D{0.0, 0.1, 0.0});
  CHECK(freefall.freefall == true);
  CHECK(freefall.onset == false);

  // Impact/catch: magnitude spikes, free-fall ends.
  freefall.update(Coord3D{0.0, 0.0, 3.0});
  CHECK(freefall.freefall == false);
  CHECK(freefall.landed == true);
  CHECK(freefall.fallTime == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("Freefall accepts a pre-computed magnitude", "[descriptors][freefall]")
{
  Freefall freefall;
  freefall.threshold = 0.3;

  freefall.update(0.1);  // magnitude directly
  CHECK(freefall.freefall == true);
  CHECK(freefall.onset == true);
}

TEST_CASE("Freefall ties external acceleration", "[descriptors][freefall]")
{
  Coord3D accel{0.0, 0.0, 0.0};
  Freefall freefall;
  freefall.threshold = 0.3;
  CHECK(freefall.tie(&accel) == 1);

  CHECK(freefall.update() == 1);
  CHECK(freefall.freefall == true);  // magnitude 0 is weightless

  Freefall untied;
  CHECK(untied.update() == 0);
}
