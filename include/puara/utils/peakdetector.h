/**
* @file peakdetector.h
* @brief Detect local maxima and minima (peaks and troughs) in a streaming signal.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <cmath>

namespace puara_gestures::utils
{
/**
 * @class PeakDetector
 * @brief Streaming local-extremum detector: flags peaks (local maxima) and
 * troughs (local minima) as they pass.
 *
 * @details
 * Feed a signal one sample at a time and `PeakDetector` reports the instant
 * it turns around: a `peak` when the value stops rising and starts falling,
 * a `trough` when it stops falling and starts rising. Useful for turning a
 * continuous control into events — a gesture reaching its extent, a bounce,
 * an oscillation cycle — without buffering the whole signal.
 *
 * Detection works on the slope sign, so a turning point is confirmed on the
 * sample *after* the extremum (one-sample latency) and the reported
 * `peakValue`/`troughValue` is the extremum itself. A `delta` guards against
 * noise: the signal must move at least `delta` away from the last extremum
 * before the opposite kind of extremum can be reported, so small wiggles do
 * not produce spurious peaks.
 *
 * No timing or allocation, comparisons only, so it is fully deterministic
 * and behaves identically in an Arduino `loop()`, a thread, or an ossia
 * score process. Header-only, no STL/Boost.
 *
 * Example:
 * @code
 *   puara_gestures::utils::PeakDetector detector;
 *   detector.delta = 0.05; // ignore wiggles smaller than this
 *   detector.update(sensorValue());
 *   if(detector.peak)   { onPeak(detector.peakValue); }
 *   if(detector.trough) { onTrough(detector.troughValue); }
 * @endcode
 */
class PeakDetector
{
public:
  /** @brief Minimum swing away from the last extremum before the opposite one counts. */
  double delta = 0.0;

  /** @brief True for one update when a local maximum was just confirmed. */
  bool peak = false;
  /** @brief True for one update when a local minimum was just confirmed. */
  bool trough = false;
  /** @brief Value of the most recent peak. */
  double peakValue = 0.0;
  /** @brief Value of the most recent trough. */
  double troughValue = 0.0;

  PeakDetector() noexcept = default;
  PeakDetector(const PeakDetector&) noexcept = default;
  PeakDetector(PeakDetector&&) noexcept = default;
  PeakDetector& operator=(const PeakDetector&) noexcept = default;
  PeakDetector& operator=(PeakDetector&&) noexcept = default;

  /**
   * @brief Construct with an explicit noise threshold.
   * @param deltaValue Minimum swing before the opposite extremum is reported.
   */
  explicit PeakDetector(double deltaValue) noexcept
  : delta(deltaValue)
  {
  }

  /**
   * @brief Feed a new sample and look for a turning point.
   * @param value Current input value.
   * @return true if this sample confirmed a peak or a trough.
   */
  bool update(double value)
  {
    peak = false;
    trough = false;

    if(!initialized)
    {
      initialized = true;
      last = value;
      candidateMax = value;
      candidateMin = value;
      return false;
    }

    if(looking == Rising)
    {
      if(value > candidateMax)
      {
        candidateMax = value;
      }
      // Turned down by at least delta from the running max: that max was a peak.
      if(value < candidateMax - delta)
      {
        peak = true;
        peakValue = candidateMax;
        candidateMin = value;
        looking = Falling;
      }
    }
    else if(looking == Falling)
    {
      if(value < candidateMin)
      {
        candidateMin = value;
      }
      // Turned up by at least delta from the running min: that min was a trough.
      if(value > candidateMin + delta)
      {
        trough = true;
        troughValue = candidateMin;
        candidateMax = value;
        looking = Rising;
      }
    }
    else  // Unknown: pick a direction once the signal moves past delta.
    {
      if(value > candidateMax)
      {
        candidateMax = value;
      }
      if(value < candidateMin)
      {
        candidateMin = value;
      }
      if(value < candidateMax - delta)
      {
        peak = true;
        peakValue = candidateMax;
        candidateMin = value;
        looking = Falling;
      }
      else if(value > candidateMin + delta)
      {
        trough = true;
        troughValue = candidateMin;
        candidateMax = value;
        looking = Rising;
      }
    }

    last = value;
    return peak || trough;
  }

  /** @brief Get the most recent peak value. */
  double current_value() const { return peakValue; }

  /** @brief Clear all state. */
  void reset()
  {
    peak = false;
    trough = false;
    peakValue = 0.0;
    troughValue = 0.0;
    initialized = false;
    looking = Unknown;
    last = 0.0;
    candidateMax = 0.0;
    candidateMin = 0.0;
  }

private:
  enum Direction
  {
    Unknown,
    Rising,
    Falling
  };

  bool initialized = false;
  Direction looking = Unknown;
  double last = 0.0;
  double candidateMax = 0.0;
  double candidateMin = 0.0;
};
}
