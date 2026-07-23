#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Breath tracks envelope and note start/stop", "[descriptors][breath]")
{
  Breath breath;
  breath.attack = 1.0;   // instant follow for a deterministic test
  breath.release = 1.0;
  breath.onThreshold = 2.0;
  breath.offThreshold = 1.0;
  breath.articulation = 0.5;

  // Below onThreshold: no note.
  breath.update(0.5);
  CHECK(breath.active == false);
  CHECK(breath.onset == false);

  // Rising past onThreshold starts a note (and a first attack).
  breath.update(3.0);
  CHECK(breath.pressure == Catch::Approx(3.0).margin(1e-9));
  CHECK(breath.active == true);
  CHECK(breath.onset == true);
  CHECK(breath.polarity == 1);

  // Sustained: still active, no new onset.
  breath.update(3.0);
  CHECK(breath.active == true);
  CHECK(breath.onset == false);
  CHECK(breath.tongued == false);

  // Dropping to/below offThreshold ends the note.
  breath.update(0.0);
  CHECK(breath.active == false);
  CHECK(breath.ended == true);
}

TEST_CASE("Breath detects tonguing while blowing", "[descriptors][breath]")
{
  Breath breath;
  breath.attack = 1.0;
  breath.release = 1.0;
  breath.onThreshold = 1.0;
  breath.offThreshold = 0.5;
  breath.articulation = 1.0;

  breath.update(3.0);  // onset (not counted as tongued)
  CHECK(breath.onset == true);
  CHECK(breath.tongued == false);

  breath.update(3.0);  // steady, velocity 0
  CHECK(breath.tongued == false);

  // Sharp rise while still blowing: a tongued re-articulation.
  breath.update(6.0);
  CHECK(breath.active == true);
  CHECK(breath.tongued == true);

  // Steady again re-arms; no repeated tongued without a fresh rise.
  breath.update(6.0);
  CHECK(breath.tongued == false);
}

TEST_CASE("Breath reports blow/draw polarity from a bipolar sensor",
          "[descriptors][breath]")
{
  Breath breath;
  breath.attack = 1.0;
  breath.release = 1.0;
  breath.onThreshold = 1.0;
  breath.offThreshold = 0.5;

  breath.update(4.0);
  CHECK(breath.polarity == 1);
  CHECK(breath.pressure == Catch::Approx(4.0).margin(1e-9));  // envelope tracks magnitude

  breath.update(-4.0);
  CHECK(breath.polarity == -1);
  CHECK(breath.pressure == Catch::Approx(4.0).margin(1e-9));  // still positive loudness
}

TEST_CASE("Breath ties external data", "[descriptors][breath]")
{
  double pressure = 0.0;
  Breath breath;
  breath.attack = 1.0;
  breath.onThreshold = 1.0;
  CHECK(breath.tie(&pressure) == 1);

  pressure = 3.0;
  CHECK(breath.update() == 1);
  CHECK(breath.current_value() == Catch::Approx(3.0).margin(1e-9));
  CHECK(breath.active == true);

  Breath untied;
  CHECK(untied.update() == 0);
}
