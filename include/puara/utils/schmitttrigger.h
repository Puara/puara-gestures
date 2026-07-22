/**
* @file schmitttrigger.h
* @brief Hysteresis gate (Schmitt trigger): chatter-free on/off from a noisy signal.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

namespace puara_gestures::utils
{
/**
 * @class SchmittTrigger
 * @brief Two-threshold gate that turns a noisy analog signal into a clean
 * on/off state without chatter.
 *
 * @details
 * A single threshold flickers on and off when a noisy signal hovers right
 * around it. A Schmitt trigger fixes this with *hysteresis*: it switches ON
 * only when the input rises to `high`, and back OFF only when it falls to
 * `low`. Between the two thresholds the state is held, so noise in that band
 * cannot make it chatter. This is the right building block for deriving a
 * stable gate/trigger from any continuous sensor (pressure, proximity,
 * envelope, motion energy) before feeding it to a note-on, a hold, or a
 * state machine.
 *
 * `update()` returns the current boolean state and also exposes one-shot
 * edge flags (`rose`, `fell`) for the exact call where the state changed,
 * handy for triggering events. It keeps no timing state and uses only
 * comparisons, so it is fully deterministic and behaves identically in an
 * Arduino `loop()`, a thread, or an ossia score process. Header-only, no
 * STL/Boost/allocation.
 *
 * Example:
 * @code
 *   puara_gestures::utils::SchmittTrigger gate{0.3, 0.7}; // low, high
 *   bool on = gate.update(sensorValue());
 *   if(gate.rose) { noteOn(); }
 *   if(gate.fell) { noteOff(); }
 * @endcode
 */
class SchmittTrigger
{
public:
  /** @brief Input must fall to/below this to switch OFF. */
  double low = 0.25;
  /** @brief Input must rise to/above this to switch ON. */
  double high = 0.75;

  /** @brief Current gate state (true = on). */
  bool state = false;
  /** @brief True only on the update() call that switched the gate ON. */
  bool rose = false;
  /** @brief True only on the update() call that switched the gate OFF. */
  bool fell = false;

  SchmittTrigger() noexcept = default;
  SchmittTrigger(const SchmittTrigger&) noexcept = default;
  SchmittTrigger(SchmittTrigger&&) noexcept = default;
  SchmittTrigger& operator=(const SchmittTrigger&) noexcept = default;
  SchmittTrigger& operator=(SchmittTrigger&&) noexcept = default;

  /**
   * @brief Construct with explicit switching thresholds.
   * @param lowValue Threshold to switch OFF.
   * @param highValue Threshold to switch ON.
   */
  SchmittTrigger(double lowValue, double highValue) noexcept
  : low(lowValue)
  , high(highValue)
  {
  }

  /**
   * @brief Feed a new sample and update the gate.
   * @param value Current input value.
   * @return The gate state after this sample (true = on).
   */
  bool update(double value)
  {
    rose = false;
    fell = false;
    if(!state && value >= high)
    {
      state = true;
      rose = true;
    }
    else if(state && value <= low)
    {
      state = false;
      fell = true;
    }
    return state;
  }

  /** @brief Get the current gate state without feeding a new sample. */
  bool current_value() const { return state; }

  /** @brief Reset the gate to OFF and clear edge flags. */
  void reset()
  {
    state = false;
    rose = false;
    fell = false;
  }
};
}
