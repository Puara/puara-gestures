#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <chrono>
#include <cmath>
#include <thread>

using namespace puara_gestures;

// Tap a discrete input at a fixed interval and release between taps, so each
// rising edge is one onset. Returns after driving `taps` onsets `intervalMs`
// apart.
static void tapRhythm(Rhythm& rhythm, int taps, int intervalMs)
{
  for(int i = 0; i < taps; ++i)
  {
    rhythm.update(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    rhythm.update(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs - 2));
  }
}

TEST_CASE("Rhythm descriptor extracts tempo, frequency, and beat position",
          "[descriptors][rhythm]")
{
  Rhythm rhythm;
  rhythm.timeSignatureNumerator = 4;

  // ~50 ms between onsets -> 20 Hz onset rate, 1200 BPM beat.
  tapRhythm(rhythm, 6, 50);

  CHECK(rhythm.onsetCount == 6);
  CHECK(rhythm.interval == Catch::Approx(50.0).margin(15.0));
  CHECK(rhythm.frequency == Catch::Approx(20.0).margin(6.0));
  CHECK(rhythm.tempo == Catch::Approx(1200.0).margin(360.0));
  // Steady taps land on the beat, so the figure stays near 1.
  CHECK(rhythm.figure == Catch::Approx(1.0).margin(0.4));
  CHECK(rhythm.subdivision == 1);
  // Six ~beat-length intervals advance the beat/bar counters.
  CHECK((rhythm.bar > 0 || rhythm.beat > 0));
}

TEST_CASE("Rhythm descriptor detects subdivisions against the beat",
          "[descriptors][rhythm]")
{
  Rhythm rhythm;

  // Establish a steady ~80 ms beat.
  tapRhythm(rhythm, 5, 80);
  double beatPeriod = rhythm.period;
  CHECK(beatPeriod > 0.0);

  // Now tap twice as fast: ~40 ms intervals should read as eighth notes.
  rhythm.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  rhythm.update(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(38));
  rhythm.update(1);

  CHECK(rhythm.figure == Catch::Approx(0.5).margin(0.25));
  CHECK(rhythm.subdivision == 2);
  // A subdivided tap must not drag the beat estimate off its pulse.
  CHECK(rhythm.period == Catch::Approx(beatPeriod).margin(beatPeriod * 0.3));
}

TEST_CASE("Rhythm descriptor tie() reads from external data",
          "[descriptors][rhythm]")
{
  int tap_data = 0;
  Rhythm rhythm;
  CHECK(rhythm.tie(&tap_data) == 1);

  tap_data = 1;
  CHECK(rhythm.update() == 1);
  CHECK(rhythm.onsetCount == 1);

  Rhythm untied;
  CHECK(untied.update() == 0);
}
