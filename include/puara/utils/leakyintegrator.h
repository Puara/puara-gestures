#pragma once


#include <puara/utils/chrono.h>


namespace puara_gestures::utils
{
/**
 *  @brief Simple leaky integrator implementation.
 *
 *  This class combines the current reading with a fraction of the previous
 *  output, so the signal changes more smoothly over time.
 *
 *  Example:
 *    puara_gestures::utils::LeakyIntegrator integrator(0.0, 0.0, 0.5, 0, 0);
 *    double out1 = integrator.integrate(10.0); // 10.0
 *    double out2 = integrator.integrate(17.0); // 17.0 + 10.0 * 0.5 = 22.0
 *
 *  `leak` controls memory: 0.0 ignores history, 1.0 fully retains it.
 */
class LeakyIntegrator
{
public:
  /**
   * @brief The most recent filtered output.
   */
  double current_value{};

  /**
   * @brief Previous integrator output used by the leak formula.
   */
  double old_value{};

  /**
   * @brief Leak factor between 0 and 1.
   */
  double leak{};

  /**
   * @brief Target update frequency in Hz. Use 0 or negative to disable timing.
   */
  int frequency{};

  /**
   * @brief Last update timestamp in milliseconds.
   */
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
   * @brief Integrate a new sample using the configured leak and frequency.
   *
   * This behaves like an attenuator: each new reading is combined with a
   * fraction of the previous value, controlled by `leak`.
   *
   * @param reading New value to add into the integrator.
   * @param custom_leak Leak factor between 0 and 1.
   * @param timerValue Reference to the current timer value, in milliseconds.
   * @return The updated integrator output.
   */

  double integrate(
      double reading, double oldValue, double leakValue, int freq,
      unsigned long long& timerValue)
  {
    const auto current_time = utils::getCurrentTimeMicroseconds();

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

