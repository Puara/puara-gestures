/**
* @file oneeuro.h
* @brief 1€ filter: speed-adaptive low-pass smoothing for noisy interactive input.
* @see https://github.com/Puara/puara-gestures
* @see https://gery.casiez.net/1euro/ (Casiez, Roussel, Vogel - CHI 2012)
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <puara/utils/chrono.h>

#include <cmath>
#include <numbers>

namespace puara_gestures::utils
{
/**
 * @class OneEuro
 * @brief Speed-adaptive low-pass filter (the "1€ filter").
 *
 * @details
 * The 1€ filter smooths a noisy 1D signal while keeping lag low. Unlike a
 * fixed low-pass or moving average, its cutoff frequency *adapts to the
 * signal speed*: when the input is slow it filters hard (removes jitter),
 * and when the input moves fast it filters little (keeps responsiveness).
 * This is the de-facto standard for cleaning up interactive controllers,
 * sensors, and pointing input, which makes it a good general smoothing
 * stage in front of any Puara descriptor.
 *
 * Two knobs shape the behaviour:
 *   - `mincutoff` : the cutoff (Hz) used when the signal is still. Lower =
 *                   smoother/steadier but laggier; higher = snappier but
 *                   noisier. Start around 1.0 and lower it if the output
 *                   still jitters at rest.
 *   - `beta`      : how strongly the cutoff opens up with speed. Higher =
 *                   less lag on fast moves. Start at 0.0 and raise it if
 *                   fast motion feels delayed.
 *   - `dcutoff`   : cutoff (Hz) of the internal speed estimate; the default
 *                   of 1.0 rarely needs changing.
 *
 * Timing can come from the library's monotonic clock (call `filter(value)`)
 * or be supplied explicitly (call `filter(value, dt)`), so it behaves the
 * same in an Arduino `loop()`, a thread, or an ossia score process. It is
 * header-only and uses only doubles (no STL, no Boost, no allocation).
 *
 * Example:
 * @code
 *   puara_gestures::utils::OneEuro smoother{1.0, 0.007};
 *   // in your loop, at whatever rate:
 *   double clean = smoother.filter(analogReadValue());
 * @endcode
 */
class OneEuro
{
public:
  /** @brief Cutoff frequency (Hz) applied when the signal is slow. */
  double mincutoff = 1.0;
  /** @brief Speed coefficient: how much the cutoff opens up with motion. */
  double beta = 0.0;
  /** @brief Cutoff frequency (Hz) of the internal speed (derivative) estimate. */
  double dcutoff = 1.0;
  /** @brief Most recent filtered output. */
  double current_value = 0.0;

  OneEuro() noexcept = default;
  OneEuro(const OneEuro&) noexcept = default;
  OneEuro(OneEuro&&) noexcept = default;
  OneEuro& operator=(const OneEuro&) noexcept = default;
  OneEuro& operator=(OneEuro&&) noexcept = default;

  /**
   * @brief Construct with explicit filter parameters.
   * @param mincutoff_ Cutoff (Hz) at rest.
   * @param beta_ Speed coefficient.
   * @param dcutoff_ Derivative cutoff (Hz).
   */
  explicit OneEuro(double mincutoff_, double beta_ = 0.0, double dcutoff_ = 1.0) noexcept
  : mincutoff(mincutoff_)
  , beta(beta_)
  , dcutoff(dcutoff_)
  {
  }

  /**
   * @brief Filter a new sample, timing the step with the monotonic clock.
   * @param value New raw input value.
   * @return The updated filtered output.
   */
  double filter(double value)
  {
    unsigned long long now = utils::getCurrentTimeMicroseconds();
    double dt = defaultRate;
    if(initialized && now > lastTime)
    {
      dt = static_cast<double>(now - lastTime) / 1e6;
    }
    lastTime = now;
    return filterWithDt(value, dt);
  }

  /**
   * @brief Filter a new sample with an explicit time step.
   * @param value New raw input value.
   * @param dt Time elapsed since the previous sample, in seconds.
   * @return The updated filtered output.
   */
  double filter(double value, double dt)
  {
    if(dt <= 0.0)
    {
      dt = 1e-6;
    }
    return filterWithDt(value, dt);
  }

  /** @brief Clear internal state, keeping the configured parameters. */
  void reset()
  {
    initialized = false;
    lastTime = 0;
    xPrev = 0.0;
    dxPrev = 0.0;
    current_value = 0.0;
  }

private:
  static constexpr double pi = std::numbers::pi;

  /** @brief Smoothing factor for a given cutoff frequency and time step. */
  static double alpha(double cutoff, double dt)
  {
    double tau = 1.0 / (2.0 * pi * cutoff);
    return 1.0 / (1.0 + tau / dt);
  }

  /** @brief One exponential low-pass step. */
  static double lowpass(double x, double prev, double a)
  {
    return a * x + (1.0 - a) * prev;
  }

  double filterWithDt(double value, double dt)
  {
    if(!initialized)
    {
      xPrev = value;
      dxPrev = 0.0;
      current_value = value;
      initialized = true;
      return current_value;
    }

    // Estimate and smooth the signal speed, then open the cutoff with it.
    double dx = (value - xPrev) / dt;
    double edx = lowpass(dx, dxPrev, alpha(dcutoff, dt));
    double cutoff = mincutoff + beta * std::fabs(edx);

    double xFilt = lowpass(value, xPrev, alpha(cutoff, dt));

    xPrev = xFilt;
    dxPrev = edx;
    current_value = xFilt;
    return current_value;
  }

  bool initialized = false;
  unsigned long long lastTime = 0;
  double xPrev = 0.0;
  double dxPrev = 0.0;
  double defaultRate = 1.0 / 60.0;  ///< dt assumed for the first clocked sample.
};
}
