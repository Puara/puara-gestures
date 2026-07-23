#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Ribbon reports position, slide velocity and direction",
          "[descriptors][ribbon]")
{
  Ribbon ribbon;

  // Touch down: onset, no velocity yet.
  ribbon.update(0.5, true, 0.0);
  CHECK(ribbon.touched == true);
  CHECK(ribbon.onset == true);
  CHECK(ribbon.position == Catch::Approx(0.5).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(0.0).margin(1e-9));

  // Slide up by 0.1 over 0.1 s -> velocity 1.0, direction +1.
  ribbon.update(0.6, true, 0.1);
  CHECK(ribbon.position == Catch::Approx(0.6).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(1.0).margin(1e-9));
  CHECK(ribbon.direction == 1);

  // Slide back down.
  ribbon.update(0.5, true, 0.1);
  CHECK(ribbon.direction == -1);
  CHECK(ribbon.velocity == Catch::Approx(-1.0).margin(1e-9));
}

TEST_CASE("Ribbon does not compute velocity across a re-touch",
          "[descriptors][ribbon]")
{
  Ribbon ribbon;

  ribbon.update(0.1, true, 0.0);  // touch near one end
  ribbon.update(0.1, false, 0.0); // lift
  CHECK(ribbon.release == true);

  // New touch far away: onset resets, no giant spurious velocity.
  ribbon.update(0.9, true, 0.1);
  CHECK(ribbon.onset == true);
  CHECK(ribbon.position == Catch::Approx(0.9).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("Ribbon detects a quick tap and rejects long/large touches",
          "[descriptors][ribbon]")
{
  Ribbon tapRibbon;
  tapRibbon.tapMaxTime = 200.0;
  tapRibbon.tapMaxTravel = 0.05;

  // Short touch that barely moves -> tap on release.
  tapRibbon.update(0.5, true, 0.0);
  tapRibbon.update(0.51, true, 0.05);  // +50 ms, travel 0.01
  tapRibbon.update(0.51, false, 0.0);
  CHECK(tapRibbon.release == true);
  CHECK(tapRibbon.tap == true);

  // A touch that travels too far is not a tap.
  Ribbon slideRibbon;
  slideRibbon.tapMaxTime = 200.0;
  slideRibbon.tapMaxTravel = 0.05;
  slideRibbon.update(0.2, true, 0.0);
  slideRibbon.update(0.6, true, 0.05);  // travels 0.4
  slideRibbon.update(0.6, false, 0.0);
  CHECK(slideRibbon.release == true);
  CHECK(slideRibbon.tap == false);
}
