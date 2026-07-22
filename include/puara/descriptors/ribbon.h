/**
* @file ribbon.h
* @brief 1D touch-strip / position-sensor gestures: position, slide velocity, direction, taps.
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
   * @class Ribbon
   * @brief Gestures for a 1D touch strip (ribbon / SoftPot / linear position
   * sensor): absolute position, slide velocity and direction, and taps.
   *
   * @details
   * A ribbon controller reports where along a strip a finger is, plus whether
   * it is being touched at all. `Ribbon` turns that pair into playable
   * gestures: the absolute `position`, the `velocity` and `direction` of a
   * glissando, touch-down/up events, and a quick `tap` (a brief touch that
   * barely moves). It handles the awkward parts for you — velocity is not
   * computed across a lift-and-retouch (so a new touch elsewhere does not
   * produce a huge spurious jump), and it resets cleanly on release.
   *
   * Feed it the current position and a touched flag each frame. Positions are
   * in whatever units the sensor uses (e.g. 0..1 normalized, or raw counts).
   * `tapMaxTime` / `tapMaxTravel` define what counts as a tap. Timing uses
   * the library monotonic clock (`update(pos, touched)`), or supply an
   * explicit step (`update(pos, touched, dt)`) for reproducible results. It
   * behaves identically in an Arduino `loop()`, a thread, or an ossia score
   * process. Header-only, doubles only, no STL/Boost.
   *
   * Example:
   * @code
   * #include <puara/descriptors/ribbon.h>
   *
   * puara_gestures::Ribbon ribbon;
   *
   * void loop() {
   *   bool touched = ribbonTouched();
   *   double pos = ribbonPosition(); // e.g. 0..1
   *   ribbon.update(pos, touched);
   *   double pitch = ribbon.position;   // map to pitch
   *   double slide = ribbon.velocity;   // map to a glissando amount
   *   if (ribbon.tap) triggerNote();
   * }
   * @endcode
   *
   * @ingroup puara_gestures_descriptors
   */
class Ribbon
{
public:
  Ribbon() noexcept = default;
  Ribbon(const Ribbon&) noexcept = default;
  Ribbon(Ribbon&&) noexcept = default;
  Ribbon& operator=(const Ribbon&) noexcept = default;
  Ribbon& operator=(Ribbon&&) noexcept = default;

  // --- Configuration -------------------------------------------------------

  /** @brief Maximum touch duration (ms) that can still count as a tap. */
  double tapMaxTime = 200.0;
  /** @brief Maximum travel (position units) during a touch that still counts as a tap. */
  double tapMaxTravel = 0.05;

  // --- Outputs -------------------------------------------------------------

  /** @brief Current position along the strip (last touched value). */
  double position = 0.0;
  /** @brief Slide velocity in position units per second (0 when not sliding). */
  double velocity = 0.0;
  /** @brief Slide direction: 1 = increasing, -1 = decreasing, 0 = still/lifted. */
  int direction = 0;
  /** @brief True while the strip is being touched. */
  bool touched = false;
  /** @brief True for one update when a touch begins. */
  bool onset = false;
  /** @brief True for one update when the finger lifts. */
  bool release = false;
  /** @brief True for one update on lift if the touch was a quick, short tap. */
  bool tap = false;

  /**
   * @brief Update from position + touch state, timing with the monotonic clock.
   * @param pos Current position along the strip.
   * @param isTouched Whether the strip is currently touched.
   * @return The current position.
   */
  double update(double pos, bool isTouched)
  {
    unsigned long long now = puara_gestures::utils::getCurrentTimeMicroseconds();
    double dt = 0.0;
    if(touched && isTouched && now > lastTime)
    {
      dt = static_cast<double>(now - lastTime) / 1e6;
    }
    lastTime = now;
    return process(pos, isTouched, dt, static_cast<long>(now / 1000ULL));
  }

  /**
   * @brief Update from position + touch state with an explicit time step.
   * @param pos Current position along the strip.
   * @param isTouched Whether the strip is currently touched.
   * @param dt Time elapsed since the previous sample, in seconds.
   * @return The current position.
   */
  double update(double pos, bool isTouched, double dt)
  {
    if(dt < 0.0)
    {
      dt = 0.0;
    }
    nowMsExplicit += static_cast<long>(dt * 1000.0);
    return process(pos, isTouched, dt, nowMsExplicit);
  }

  /**
   * @brief Get the current position.
   * @return The latest touched position.
   */
  double current_value() const { return position; }

  /** @brief Clear all state. */
  void reset()
  {
    position = 0.0;
    velocity = 0.0;
    direction = 0;
    touched = false;
    onset = false;
    release = false;
    tap = false;
    lastTime = 0;
    nowMsExplicit = 0;
    touchStartTime = 0;
    touchStartPosition = 0.0;
    travel = 0.0;
  }

private:
  double process(double pos, bool isTouched, double dt, long nowMs)
  {
    onset = false;
    release = false;
    tap = false;

    if(isTouched)
    {
      if(!touched)
      {
        // Touch-down: anchor, no velocity across the (re)touch.
        touched = true;
        onset = true;
        position = pos;
        velocity = 0.0;
        direction = 0;
        touchStartTime = nowMs;
        touchStartPosition = pos;
        travel = 0.0;
        return position;
      }

      double delta = pos - position;
      travel += std::fabs(delta);
      velocity = (dt > 0.0) ? delta / dt : 0.0;
      direction = (delta > 0.0) ? 1 : (delta < 0.0) ? -1 : 0;
      position = pos;
    }
    else
    {
      if(touched)
      {
        // Lift: decide whether the whole touch was a quick tap.
        touched = false;
        release = true;
        double heldMs = static_cast<double>(nowMs - touchStartTime);
        if(heldMs <= tapMaxTime && travel <= tapMaxTravel)
        {
          tap = true;
        }
      }
      velocity = 0.0;
      direction = 0;
    }
    return position;
  }

  unsigned long long lastTime = 0;
  long nowMsExplicit = 0;
  long touchStartTime = 0;
  double touchStartPosition = 0.0;
  double travel = 0.0;
};
}
