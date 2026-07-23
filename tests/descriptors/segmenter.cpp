#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

TEST_CASE("Segmenter marks phrase onset and offset with hysteresis",
          "[descriptors][segmenter]")
{
  Segmenter phrase;
  phrase.onThreshold = 1.0;
  phrase.offThreshold = 0.5;
  phrase.minActiveSamples = 1;
  phrase.minSilentSamples = 1;

  // Below onThreshold: idle.
  CHECK(phrase.update(0.2) == 0);
  CHECK(phrase.onset == false);

  // Cross onThreshold: phrase starts.
  CHECK(phrase.update(2.0) == 1);
  CHECK(phrase.active == true);
  CHECK(phrase.onset == true);
  CHECK(phrase.offset == false);

  // In the hysteresis band: still active, no events.
  phrase.update(0.7);
  CHECK(phrase.active == true);
  CHECK(phrase.onset == false);

  // Fall to offThreshold: phrase ends.
  phrase.update(0.3);
  CHECK(phrase.active == false);
  CHECK(phrase.offset == true);
  CHECK(phrase.count == 1);
}

TEST_CASE("Segmenter debounces onset and offset by sample count",
          "[descriptors][segmenter]")
{
  Segmenter phrase;
  phrase.onThreshold = 1.0;
  phrase.offThreshold = 0.5;
  phrase.minActiveSamples = 3;
  phrase.minSilentSamples = 2;

  // Needs 3 consecutive above-threshold updates to confirm onset.
  phrase.update(2.0);
  CHECK(phrase.active == false);
  phrase.update(2.0);
  CHECK(phrase.active == false);
  phrase.update(2.0);
  CHECK(phrase.active == true);
  CHECK(phrase.onset == true);

  // A single dip does not end it (needs 2 consecutive below-threshold).
  phrase.update(0.0);
  CHECK(phrase.active == true);
  phrase.update(0.0);
  CHECK(phrase.active == false);
  CHECK(phrase.offset == true);
}

TEST_CASE("Segmenter ties external data", "[descriptors][segmenter]")
{
  double energy = 0.0;
  Segmenter phrase;
  phrase.onThreshold = 1.0;
  phrase.offThreshold = 0.5;
  CHECK(phrase.tie(&energy) == 1);

  energy = 2.0;
  CHECK(phrase.update() == 1);
  CHECK(phrase.active == true);

  Segmenter untied;
  CHECK(untied.update() == 0);
}
