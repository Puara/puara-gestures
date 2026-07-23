#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Envelope follower rises with attack and falls with release",
          "[descriptors][envelope]")
{
  Envelope envelope;
  envelope.attack = 1.0;   // jump instantly to louder input
  envelope.release = 0.0;  // never fall (hold the peak)
  envelope.rectify = true;

  CHECK(envelope.update(10.0) == Catch::Approx(10.0).margin(1e-9));
  CHECK(envelope.current_value() == Catch::Approx(10.0).margin(1e-9));
  // release = 0 holds the value when the input drops.
  CHECK(envelope.update(0.0) == Catch::Approx(10.0).margin(1e-9));

  // Half-coefficient attack moves halfway toward the target each step.
  Envelope ramp;
  ramp.attack = 0.5;
  ramp.release = 0.5;
  ramp.rectify = false;
  CHECK(ramp.update(10.0) == Catch::Approx(5.0).margin(1e-9));   // 0 + 0.5*(10-0)
  CHECK(ramp.update(10.0) == Catch::Approx(7.5).margin(1e-9));   // 5 + 0.5*(10-5)
  CHECK(ramp.update(0.0) == Catch::Approx(3.75).margin(1e-9));   // 7.5 + 0.5*(0-7.5)

  // Rectify follows the magnitude of a bipolar input.
  Envelope rect;
  rect.attack = 1.0;
  rect.release = 0.0;
  rect.rectify = true;
  CHECK(rect.update(-8.0) == Catch::Approx(8.0).margin(1e-9));
}

TEST_CASE("Envelope follower ties external data", "[descriptors][envelope]")
{
  double motion = 0.0;
  Envelope envelope;
  envelope.attack = 1.0;
  envelope.release = 0.0;
  CHECK(envelope.tie(&motion) == 1);

  motion = 5.0;
  CHECK(envelope.update() == 1);
  CHECK(envelope.current_value() == Catch::Approx(5.0).margin(1e-9));

  Envelope untied;
  CHECK(untied.update() == 0);
}
