/**
* @file rhythm.h
* @brief Helper to extract rhythmic features (frequency, tempo, time signature, subdivision) from discrete inputs over time.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{
  /**
   * @class Rhythm
   * @brief Rhythmic feature extractor for discrete inputs (e.g. a button, a
   * tap sensor, a footswitch) sampled over time.
   *
   * @details
   * Feed `Rhythm` the same kind of discrete signal you would feed a
   * @ref Button: a value that goes above `threshold` on an event ("onset")
   * and back below it afterwards. From the timing of successive onsets it
   * derives:
   *   - `interval`  : the last inter-onset interval (IOI) in milliseconds;
   *   - `frequency` : instantaneous onset rate in Hz (`1000 / interval`);
   *   - `period`    : a smoothed estimate of the underlying beat/pulse (ms);
   *   - `tempo`     : the beat tempo in BPM (`60000 / period`);
   *   - `figure`    : the last IOI expressed relative to the beat
   *                   (1.0 = beat, 0.5 = half a beat, 2.0 = two beats);
   *   - `subdivision`: how many equal parts the last IOI splits the beat into
   *                   (2 = eighths, 3 = triplet, 4 = sixteenths, 1 = on/over the beat);
   *   - `beat` / `bar`: the current beat position inside the bar and the bar
   *                   count, advanced by elapsed time using the estimated period;
   *   - `timeSignatureNumerator` / `timeSignatureDenominator`: the working
   *                   meter, optionally re-estimated from bar-length gaps.
   *
   * Only integer/double/bool state is used and the IOI history lives in a
   * fixed-size C array, so `Rhythm` is header-only and Arduino-friendly (no
   * STL containers, no Boost, no dynamic allocation). Timing uses the same
   * monotonic clock as @ref Button
   * (`puara_gestures::utils::getCurrentTimeMicroseconds()`), which makes it
   * work identically whether `update()` is called from an Arduino `loop()`,
   * a dedicated thread, or an ossia score process callback.
   *
   * @warning Time-signature detection is a best-effort heuristic, not a
   * reliable meter tracker. Frequency, interval, period, tempo, figure and
   * subdivision are derived directly from onset timing and are dependable;
   * `beat`/`bar` are a plain phase counter over the estimated period. The
   * meter itself (`timeSignatureNumerator`/`timeSignatureDenominator`)
   * cannot be recovered from onset timing alone without accent/velocity
   * information, so it defaults to whatever you configure. The optional
   * `estimateTimeSignature` guess (see below) is experimental and only works
   * when bars are separated by a clear pause; treat its output as a hint.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/rhythm.h>
   *
   * int tap_data = 0;
   * puara_gestures::Rhythm rhythm(&tap_data); // Tied to external tap_data variable
   *
   * void loop() {
   *   tap_data = 1 - digitalRead(BUTTON_PIN);
   *   rhythm.update(); // Reads from tied tap_data automatically
   *
   *   Serial.print("tempo: ");   Serial.print(rhythm.tempo);
   *   Serial.print(" bpm, freq: "); Serial.print(rhythm.frequency);
   *   Serial.print(" Hz, figure: "); Serial.print(rhythm.figure);
   *   Serial.print(", subdivision: "); Serial.print(rhythm.subdivision);
   *   Serial.print(", beat: "); Serial.print(rhythm.beat + 1);
   *   Serial.print("/"); Serial.println(rhythm.timeSignatureNumerator);
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `rhythm.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Rhythm
{
public:
  /**
   * @brief Default-construct a Rhythm without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Rhythm() noexcept
  : tied_data(nullptr)
  {
  }

  Rhythm(const Rhythm&) noexcept = default;
  Rhythm(Rhythm&&) noexcept = default;
  Rhythm& operator=(const Rhythm&) noexcept = default;
  Rhythm& operator=(Rhythm&&) noexcept = default;

  /**
   * @brief Construct a Rhythm tied to external input data.
   *
   * When tied, calling `update()` reads the current value from `tied_data`.
   * @param tied Pointer to an external integer source for the discrete input.
   */
  explicit Rhythm(int* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Minimum input value that counts as an onset (rising edge). */
  int threshold = 1;
  /**
   * @brief Beat-period smoothing factor in [0, 1).
   *
   * Higher values track the incoming pulse more slowly (steadier tempo);
   * lower values react faster to tempo changes.
   */
  double smoothing = 0.5;
  /**
   * @brief Intervals longer than this many ms without an onset reset the
   * pulse estimate (a long silence ends the current rhythmic phrase).
   */
  double timeout = 3000.0;
  /**
   * @brief Enable experimental meter estimation from bar-length gaps.
   *
   * When true, an inter-onset interval markedly longer than the beat is
   * treated as a bar boundary and `timeSignatureNumerator` is set to the
   * number of onsets counted since the previous such gap. Off by default so
   * the meter stays whatever you configure it to.
   *
   * @warning Experimental and low-confidence. It only fires when bars are
   * separated by a pause of roughly 1.75x the beat or more; steady playing
   * with no gap between bars, or subdivisions longer than that ratio, will
   * mislead it. It never sets the denominator. Prefer configuring
   * `timeSignatureNumerator`/`timeSignatureDenominator` directly when the
   * meter is known.
   */
  bool estimateTimeSignature = false;

  // --- Outputs -------------------------------------------------------------

  /** @brief True for exactly one update() call when a new onset is detected. */
  bool onset = false;
  /** @brief Total number of onsets detected since construction/reset. */
  unsigned int onsetCount = 0;
  /** @brief Last inter-onset interval in milliseconds. */
  double interval = 0.0;
  /** @brief Instantaneous onset rate in Hz (1000 / interval). */
  double frequency = 0.0;
  /** @brief Smoothed estimate of the beat/pulse period in milliseconds. */
  double period = 0.0;
  /** @brief Beat tempo in beats per minute (60000 / period). */
  double tempo = 0.0;
  /** @brief Last IOI relative to the beat (1.0 = beat, 0.5 = eighth, 2.0 = half note). */
  double figure = 0.0;
  /** @brief Equal parts the last IOI splits the beat into (2 = eighths, 3 = triplet, ...). */
  int subdivision = 1;
  /** @brief Current beat position inside the bar (0-based). */
  unsigned int beat = 0;
  /** @brief Number of completed bars since construction/reset. */
  unsigned int bar = 0;
  /**
   * @brief Working meter numerator (beats per bar).
   * @warning Not reliably detectable from onset timing alone. Defaults to the
   * value you set; only re-estimated (numerator only) when
   * `estimateTimeSignature` is on and a bar-length gap is seen. See the class
   * warning.
   */
  int timeSignatureNumerator = 4;
  /**
   * @brief Working meter denominator (note value that gets the beat).
   * @warning Never auto-detected; always the value you configure.
   */
  int timeSignatureDenominator = 4;

  /**
   * @brief Update the rhythm detector from a direct input value.
   *
   * Detects the rising edge of `value` across `threshold` as an onset and,
   * on each onset, recomputes the rhythmic features from the elapsed time
   * since the previous onset.
   * @param value Current discrete input value.
   */
  void update(int value)
  {
    long currentTime = puara_gestures::utils::getCurrentTimeMicroseconds() / 1000LL;
    onset = false;

    if(value >= threshold)
    {
      if(!active)
      {
        active = true;
        registerOnset(currentTime);
      }
    }
    else
    {
      active = false;
    }
  }

  /**
   * @brief Update the rhythm detector from the tied external input value.
   *
   * This overload reads the latest input from `tied_data` and processes it.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Rhythm::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current tempo estimate.
   * @return The latest computed tempo in BPM.
   */
  double current_value() const { return tempo; }

  /**
   * @brief Connect the detector to an external integer source.
   * @param new_tie Pointer to the external input value.
   * @return 1 when the tie succeeds.
   */
  int tie(int* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /**
   * @brief Clear all state, keeping the current configuration.
   */
  void reset()
  {
    active = false;
    lastOnsetTime = 0;
    haveLastOnset = false;
    onsetsSinceBar = 0;
    onset = false;
    onsetCount = 0;
    interval = 0.0;
    frequency = 0.0;
    period = 0.0;
    tempo = 0.0;
    figure = 0.0;
    subdivision = 1;
    beat = 0;
    bar = 0;
  }

private:
  /** @brief Process a freshly detected onset at time `now` (ms). */
  void registerOnset(long now)
  {
    onset = true;
    onsetCount++;

    if(!haveLastOnset)
    {
      // First onset: nothing to measure yet, just anchor the timeline.
      haveLastOnset = true;
      lastOnsetTime = now;
      onsetsSinceBar = 1;
      return;
    }

    interval = static_cast<double>(now - lastOnsetTime);
    lastOnsetTime = now;

    if(interval <= 0.0)
    {
      return;
    }

    // A long silence ends the current phrase: restart the pulse estimate.
    if(interval > timeout)
    {
      period = 0.0;
      tempo = 0.0;
      frequency = 0.0;
      figure = 0.0;
      subdivision = 1;
      beat = 0;
      onsetsSinceBar = 1;
      return;
    }

    frequency = 1000.0 / interval;

    if(period <= 0.0)
    {
      // Seed the beat estimate with the first measured interval.
      period = interval;
    }
    else
    {
      // Only fold intervals that look like a beat into the pulse estimate,
      // so subdivided taps (eighths, triplets) do not drag the tempo around.
      double ratio = interval / period;
      if(std::fabs(ratio - 1.0) < 0.25)
      {
        period = smoothing * period + (1.0 - smoothing) * interval;
      }
    }

    tempo = period > 0.0 ? 60000.0 / period : 0.0;
    figure = period > 0.0 ? interval / period : 0.0;
    subdivision = snapSubdivision(figure);

    advanceBeats(interval);
    estimateMeter(interval);
  }

  /**
   * @brief Advance the beat/bar counters by `elapsed` ms of the pulse.
   *
   * Uses a phase accumulator so the position advances by elapsed time,
   * independently of whether the last onset fell on a beat or a subdivision.
   */
  void advanceBeats(double elapsed)
  {
    if(period <= 0.0)
    {
      return;
    }
    phase += elapsed;
    while(phase >= period)
    {
      phase -= period;
      beat++;
      if(timeSignatureNumerator > 0 && beat >= static_cast<unsigned int>(timeSignatureNumerator))
      {
        beat = 0;
        bar++;
      }
    }
  }

  /**
   * @brief Experimental meter guess from bar-length gaps.
   *
   * Counts onsets between intervals that are clearly longer than a beat and
   * treats that count as the number of beats per bar.
   */
  void estimateMeter(double lastInterval)
  {
    if(!estimateTimeSignature || period <= 0.0)
    {
      return;
    }
    if(lastInterval > 1.75 * period)
    {
      // The gap that just elapsed likely spanned a bar boundary.
      int guess = onsetsSinceBar;
      if(guess >= 2 && guess <= 12)
      {
        timeSignatureNumerator = guess;
      }
      onsetsSinceBar = 1;
    }
    else
    {
      onsetsSinceBar++;
    }
  }

  /** @brief Snap a beat-relative figure to the nearest integer subdivision. */
  static int snapSubdivision(double fig)
  {
    if(fig <= 0.0)
    {
      return 1;
    }
    // fig < 1 -> the beat is subdivided; round to how many parts it split into.
    int parts = static_cast<int>(std::lround(1.0 / fig));
    if(parts < 1)
    {
      parts = 1;
    }
    return parts;
  }

  const int* tied_data{};

  bool active = false;         ///< Currently above threshold (for edge detection).
  bool haveLastOnset = false;  ///< Whether lastOnsetTime holds a real onset.
  long lastOnsetTime = 0;      ///< Timestamp of the previous onset (ms).
  double phase = 0.0;          ///< Beat phase accumulator (ms).
  int onsetsSinceBar = 0;      ///< Onsets counted since the last detected bar gap.
};
}
