//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//
// Leaky integrator:                                                              //
// - Takes a new input number each step.                                          //
// - Adds the new input to a fraction of the previous output.                     //
// - That fraction is "leak" (e.g. 0.5 means keep half of previous state).        //
// - This produces a smooth value that reacts to changes slowly instead of        //
//   jumping instantly.                                                           //
//                                                                                //
// Real-world example :                                                           //
// Sensor raw data: 10, 17, 3, 22 → erratic.                                      //
// Leaky output: 10, 13.5, 8.25, 15.1 (smoothed, trend-preserving).               //
// Use cases: gesture detection, tracking smoothing, gamepad/MIDI modulation,     //
// removing high-frequency jitter while keeping responsiveness.                   //
//********************************************************************************//

#pragma once
#include <puara/utils/chrono.h>


namespace puara_gestures::utils
{
/**
 *  @brief Simple leaky integrator implementation.
 */
class LeakyIntegrator
{
public:
  double current_value{};
  double old_value{};
  double leak{};
  int frequency{}; // leaking frequency (Hz)
  unsigned long long timer{};

  explicit LeakyIntegrator(
      double currentValue = 0, double oldValue = 0, double leakValue = 0.5,
      int freq = 100, unsigned long long timerValue = 0)
      : current_value(currentValue)
      , old_value(oldValue)
      , leak(leakValue)
      , frequency(freq)
      , timer(timerValue)
  {
  }

  /**
   * @brief Call integrator
   *
   * @param reading new value to add into the integrator
   * @param custom_leak between 0 and 1
   * @param time in microseconds
   * @return double
   */

  double integrate(
      double reading, double oldValue, double leakValue, int freq,
      unsigned long long& timerValue)
  {
    auto current_time = utils::getCurrentTimeMicroseconds();

    if(freq <= 0)
    {
      current_value = reading + (oldValue * leakValue);
    }
    else if((current_time / 1000LL) - (1000 / frequency) < timerValue)
    {
      current_value = reading + old_value;
    }
    else
    {
      current_value = reading + (oldValue * leakValue);
      timer = (current_time / 1000LL);
    }
    old_value = current_value;
    return current_value;
  }

  double integrate(double reading, double leakValue, unsigned long long& timerValue)
  {
    return this->integrate(reading, old_value, leakValue, frequency, timerValue);
  }

  double integrate(double reading, double leakValue)
  {
    return this->integrate(reading, old_value, leakValue, frequency, timer);
  }

  double integrate(double reading)
  {
    return this->integrate(reading, old_value, leak, frequency, timer);
  }
};

}
