//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <chrono>

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
  long long getCurrentTimeMicroseconds();

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
    auto currentTimePoint = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        currentTimePoint.time_since_epoch());
    unsigned long long current_time = duration.count();

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
