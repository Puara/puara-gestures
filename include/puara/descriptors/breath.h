/**
* @file breath.h
* @brief Breath/wind-pressure gestures: envelope, blowing gate, articulation (tonguing) and blow/draw polarity.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{
  /**
   * @class Breath
   * @brief Wind-controller features from a breath/pressure sensor.
   *
   * @details
   * `Breath` turns a pressure-sensor reading into the signals a wind
   * instrument needs: a smooth loudness envelope for the sustained tone, a
   * clean "am I blowing" gate with note start/stop events, detection of
   * sharp attacks (tonguing/articulation), and — for bipolar sensors — the
   * blow-vs-draw polarity (as on a harmonica or melodica). It is a
   * breath-domain composite of the @ref Envelope (attack/release follower),
   * a hysteresis gate (like @ref puara_gestures::utils::SchmittTrigger), and
   * an onset test (like @ref Impact), tuned for wind control.
   *
   * Feed it the pressure each frame:
   *   - `pressure` : the smoothed breath envelope (drives amplitude/timbre);
   *   - `active`   : true while blowing (hysteresis gate, chatter-free);
   *   - `onset`    : true for one update when a note starts (blowing begins);
   *   - `ended`    : true for one update when the note stops;
   *   - `tongued`  : true for one update on a sharp attack while already
   *                  blowing (re-articulation without stopping the air);
   *   - `polarity` : +1 blowing, -1 drawing, 0 silent (bipolar sensors);
   *   - `velocity` : per-update change of the envelope (breath acceleration).
   *
   * The envelope uses per-`update()` attack/release coefficients (like
   * @ref Envelope), so `Breath` is fully deterministic and clock-independent
   * — it behaves identically in an Arduino `loop()`, a thread, or an ossia
   * score process, as long as it is called at a roughly steady rate.
   * Header-only, doubles only, no STL/Boost/allocation.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/breath.h>
   *
   * double pressure_raw = 0.0;
   * puara_gestures::Breath breath(&pressure_raw);
   *
   * void loop() {
   *   pressure_raw = readPressureSensor();
   *   breath.update();
   *   double loudness = breath.pressure; // smooth envelope
   *   if (breath.onset)   noteOn();
   *   if (breath.tongued) reArticulate();
   *   if (breath.ended)   noteOff();
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `breath.update(value)`
   * directly with the input value.
   *
   * @ingroup puara_gestures_descriptors
   */
class Breath
{
public:
  /**
   * @brief Default-construct a Breath without tied input data.
   *
   * Use `update(value)` to provide input directly.
   */
  Breath() noexcept
  : tied_data(nullptr)
  {
  }

  Breath(const Breath&) noexcept = default;
  Breath(Breath&&) noexcept = default;
  Breath& operator=(const Breath&) noexcept = default;
  Breath& operator=(Breath&&) noexcept = default;

  /**
   * @brief Construct a Breath tied to external pressure data.
   * @param tied Pointer to an external double source for the pressure reading.
   */
  explicit Breath(double* tied)
  : tied_data(tied)
  {
  }

  // --- Configuration -------------------------------------------------------

  /** @brief Envelope rise coefficient in [0, 1]: how fast pressure follows a harder breath. */
  double attack = 0.7;
  /** @brief Envelope fall coefficient in [0, 1]: how fast pressure follows a softer breath. */
  double release = 0.2;
  /** @brief Envelope level that starts a note (blowing on). */
  double onThreshold = 0.1;
  /** @brief Envelope level that ends a note (blowing off); keep below onThreshold. */
  double offThreshold = 0.05;
  /** @brief Minimum per-update envelope rise that counts as a tongued attack. */
  double articulation = 0.15;
  /** @brief Magnitudes at or below this read as silence (sets polarity to 0). */
  double deadzone = 0.0;

  // --- Outputs -------------------------------------------------------------

  /** @brief Smoothed breath envelope (loudness contour). */
  double pressure = 0.0;
  /** @brief Most recent raw input value. */
  double raw = 0.0;
  /** @brief Per-update change of the envelope (breath acceleration). */
  double velocity = 0.0;
  /** @brief True while blowing (hysteresis gate). */
  bool active = false;
  /** @brief True for one update when a note starts. */
  bool onset = false;
  /** @brief True for one update when a note stops. */
  bool ended = false;
  /** @brief True for one update on a sharp attack while already blowing. */
  bool tongued = false;
  /** @brief Breath direction: +1 blowing, -1 drawing, 0 silent. */
  int polarity = 0;

  /**
   * @brief Update from a direct pressure value.
   * @param value Current pressure reading (may be bipolar for blow/draw).
   * @return The updated smoothed `pressure`.
   */
  double update(double value)
  {
    raw = value;
    onset = false;
    ended = false;
    tongued = false;

    double target = std::fabs(value);
    double coeff = (target > pressure) ? attack : release;
    double previous = pressure;
    pressure += coeff * (target - pressure);
    velocity = pressure - previous;

    // Blow/draw polarity from the signed raw input.
    polarity = (value > deadzone) ? 1 : (value < -deadzone) ? -1 : 0;

    // Chatter-free blowing gate with separate on/off levels.
    if(!active && pressure >= onThreshold)
    {
      active = true;
      onset = true;
    }
    else if(active && pressure <= offThreshold)
    {
      active = false;
      ended = true;
    }

    // Tonguing: a sharp rise while already blowing, re-armed once the rise eases.
    if(active && velocity >= articulation && previousVelocity < articulation && !onset)
    {
      tongued = true;
    }
    previousVelocity = velocity;

    return pressure;
  }

  /**
   * @brief Update from a `Coord1D` input.
   * @param reading Input coordinate; its `x` component is used.
   * @return The updated smoothed `pressure`.
   */
  double update(Coord1D reading) { return Breath::update(reading.x); }

  /**
   * @brief Update from the tied external pressure value.
   * @return 1 when update succeeds; 0 if no tied input is available.
   */
  int update()
  {
    if(tied_data != nullptr)
    {
      Breath::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  /**
   * @brief Get the current breath envelope.
   * @return The latest smoothed `pressure`.
   */
  double current_value() const { return pressure; }

  /**
   * @brief Connect to an external double source.
   * @param new_tie Pointer to the external pressure value.
   * @return 1 when the tie succeeds.
   */
  int tie(double* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

  /**
   * @brief Connect to an external `Coord1D` source.
   * @param new_tie Pointer to the external coordinate struct.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

  /** @brief Clear all state, keeping the configuration. */
  void reset()
  {
    pressure = 0.0;
    raw = 0.0;
    velocity = 0.0;
    active = false;
    onset = false;
    ended = false;
    tongued = false;
    polarity = 0;
    previousVelocity = 0.0;
  }

private:
  const double* tied_data{};
  double previousVelocity = 0.0;
};
}
