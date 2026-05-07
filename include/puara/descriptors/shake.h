/**
* @file shake.h
* @brief Simple 1D shake detector using a leaky integrator on acceleration energy.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @class Shake
 * @brief Simple 1D shake detector.
 *
 * @details Shake turns raw acceleration energy into a slowly changing value.
 * It is useful when you want a smooth gesture-like signal for shaking,
 * vibration, or repeated movement on one axis.
 *
 * Example:
 * @code{.cpp}
 * double accel = 0.0;
 * puara_gestures::Shake shake(&accel);
 * shake.threshold = 0.2;
 * shake.fast_leak = 0.6;
 * shake.slow_leak = 0.3;
 *
 * // in your sensor loop:
 * accel = sensor.read();
 * shake.update();
 * double energy = shake.current_value();
 * @endcode
 *
 * The output grows when motion is above threshold, and it decays when the
 * motion calms down.
 *
 * `Shake2D` and `Shake3D` work the same way for two or three axes.
 */
class Shake
{
public:
  utils::LeakyIntegrator integrator{0, 0, 0.6, 10, 0};
  double fast_leak = 0.6;
  double slow_leak = 0.3;
  double threshold = 0.1;

  /**
   * @brief Default constructor for a Shake detector.
   *
   * No external input is tied, so updates must use `update(reading)`.
   */
  Shake()
      : tied_value(nullptr)
  {
  }

  /**
   * @brief Construct a Shake detector tied to a raw double source.
   * @param tied Pointer to an external double value that provides the input reading.
   */
  explicit Shake(double* tied)
      : tied_value(tied)
  {
  }

  /**
   * @brief Construct a Shake detector tied to a `Coord1D` source.
   * @param tied Pointer to a `Coord1D` struct whose `x` value is used as input.
   */
  explicit Shake(Coord1D* tied)
      : tied_value(&(tied->x))
  {
  }

  /**
   * @brief Update the shake detector using a raw axis reading.
   *
   * If the detector is tied to external input, the tied value is also updated.
   *
   * @param reading The current axis reading.
   * @return The current shake energy value.
   */
  double update(double reading)
  {
    if(tied_value != nullptr)
    {
      *tied_value = reading;
    }

    double abs_reading = std::abs(reading);

    if(abs_reading > threshold)
    {
      integrator.integrate(abs_reading / 10, fast_leak);
    }
    else
    {
      integrator.integrate(0.0, slow_leak);
      if( integrator.current_value < (threshold/10) )
      {
        integrator.current_value = 0;
      }
    }
    return integrator.current_value;
  }

  /**
   * @brief Update the shake detector using a `Coord1D` sample.
   * @param reading The sampled coordinate.
   * @return 1 when the update is processed.
   */
  int update(Coord1D reading)
  {
    Shake::update(reading.x);
    return 1;
  }

  /**
   * @brief Update the shake detector using the tied external input.
   * @return 1 when the tied value exists and the update was processed; 0 otherwise.
   */
  int update()
  {
    if(tied_value != nullptr)
    {
      Shake::update(*tied_value);
      return 1;
    }
    else
    {
      return 0;
    }
  }

  /**
   * @brief Get the current update frequency.
   * @return The configured integrator frequency.
   */
  double frequency() const { return integrator.frequency; }

  /**
   * @brief Set the integrator frequency.
   * @param freq Frequency value to apply.
   * @return The frequency value that was set.
   */
  double frequency(double freq)
  {
    integrator.frequency = freq;
    return freq;
  }

  /**
   * @brief Get the current shake energy value.
   * @return The current integrator output.
   */
  double current_value() const { return integrator.current_value; }

  /**
   * @brief Tie the shake detector to a `Coord1D` source.
   * @param new_tie Pointer to the external `Coord1D` input source.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord1D* new_tie)
  {
    tied_value = &(new_tie->x);
    return 1;
  }

private:
  double* tied_value{};
};

/**
 * @class Shake2D
 * @brief Simple 2D shake detector.
 *
 * @details Shake2D uses two Shake objects to track motion energy on X and Y.
 * This lets you detect repeated motion or vibration across two axes, while
 * still keeping the output smooth and easy to use.
 *
 * Example:
 * @code{.cpp}
 * puara_gestures::Coord2D accel{0.0, 0.0};
 * puara_gestures::Shake2D shake2d(&accel);
 * shake2d.threshold(0.2);
 * shake2d.frequency(0); // deterministic update behavior for testing or simple loops
 *
 * // in your loop:
 * accel.x = sensor.readX();
 * accel.y = sensor.readY();
 * shake2d.update();
 * auto energy = shake2d.current_value();
 *
 * // energy.x and energy.y each report a smooth shake score.
 * @endcode
 */
class Shake2D
{
public:
  Shake2D() = default;
  Shake2D(const Shake2D&) noexcept = default;
  Shake2D(Shake2D&&) noexcept = default;
  Shake2D& operator=(const Shake2D&) noexcept = default;
  Shake2D& operator=(Shake2D&&) noexcept = default;

  Shake2D(Coord2D* tied)
      : x(&(tied->x))
      , y(&(tied->y))
  {
  }

  Shake x;
  Shake y;

  /**
   * @brief Update both X and Y shake detectors using direct readings.
   * @param readingX X-axis reading.
   * @param readingY Y-axis reading.
   * @return 1 when the update is processed.
   */
  int update(double readingX, double readingY)
  {
    x.update(readingX);
    y.update(readingY);
    return 1;
  }

  /**
   * @brief Update both X and Y shake detectors from a `Coord2D` sample.
   * @param reading The sampled 2D coordinate.
   * @return 1 when the update is processed.
   */
  int update(Coord2D reading)
  {
    x.update(reading.x);
    y.update(reading.y);
    return 1;
  }

  /**
   * @brief Update both X and Y shake detectors using tied input values.
   * @return 1 when the update is processed.
   */
  int update()
  {
    x.update();
    y.update();
    return 1;
  }

  /**
   * @brief Set a common update frequency for both X and Y detectors.
   * @param freq Frequency value to apply.
   * @return The frequency value that was set.
   */
  double frequency(double freq)
  {
    x.frequency(freq);
    y.frequency(freq);
    return freq;
  }

  /**
   * @brief Get the current 2D shake energy value.
   * @return A `Coord2D` containing X and Y shake energy values.
   */
  Coord2D current_value() const
  {
    Coord2D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    return answer;
  }

  /**
   * @brief Set the threshold for both X and Y detectors.
   * @param new_threshold Threshold value to apply.
   * @return The threshold value that was set.
   */
  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    return new_threshold;
  }
};

/**
 * @class Shake3D
 * @brief Simple 3D shake detector.
 *
 * @details Shake3D tracks motion energy on X, Y and Z by combining three Shake
 * processors. It is useful when you want a smooth, gesture-like output for
 * full 3D accelerometer movement.
 *
 * Example:
 * @code{.cpp}
 * puara_gestures::Coord3D accel{0.0, 0.0, 0.0};
 * puara_gestures::Shake3D shake3d(&accel);
 * shake3d.threshold(0.2);
 * shake3d.frequency(0);
 *
 * // on each sensor sample:
 * accel.x = sensor.readX();
 * accel.y = sensor.readY();
 * accel.z = sensor.readZ();
 * shake3d.update();
 * auto energy = shake3d.current_value();
 *
 * // energy.x, energy.y and energy.z each show a smooth shake score.
 * @endcode
 */
class Shake3D
{
public:
  Shake x, y, z;

  Shake3D() = default;
  Shake3D(const Shake3D&) noexcept = default;
  Shake3D(Shake3D&&) noexcept = default;
  Shake3D& operator=(const Shake3D&) noexcept = default;
  Shake3D& operator=(Shake3D&&) noexcept = default;

  /**
   * @brief Construct a Shake3D detector tied to a `Coord3D` source.
   * @param tied Pointer to external 3D accelerometer data.
   */
  explicit Shake3D(Coord3D* tied)
      : x(&(tied->x))
      , y(&(tied->y))
      , z(&(tied->z))
  {
  }

  /**
   * @brief Update the 3D shake detector using direct X/Y/Z readings.
   * @param readingX X-axis reading.
   * @param readingY Y-axis reading.
   * @param readingZ Z-axis reading.
   * @return 1 when the update is processed.
   */
  int update(double readingX, double readingY, double readingZ)
  {
    x.update(readingX);
    y.update(readingY);
    z.update(readingZ);
    return 1;
  }

  /**
   * @brief Update the 3D shake detector from a `Coord3D` sample.
   * @param reading The sampled 3D coordinate.
   * @return 1 when the update is processed.
   */
  int update(Coord3D reading)
  {
    x.update(reading.x);
    y.update(reading.y);
    z.update(reading.z);
    return 1;
  }

  /**
   * @brief Update the 3D shake detector using tied input values.
   * @return 1 when the update is processed.
   */
  int update()
  {
    x.update();
    y.update();
    z.update();
    return 1;
  }

  /**
   * @brief Set a common update frequency for all three axes.
   * @param freq Frequency value to apply.
   * @return The frequency value that was set.
   */
  double frequency(double freq)
  {
    x.frequency(freq);
    y.frequency(freq);
    z.frequency(freq);
    return freq;
  }

  /**
   * @brief Get the current 3D shake energy value.
   * @return A `Coord3D` containing X, Y, and Z shake energy values.
   */
  Coord3D current_value() const
  {
    Coord3D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    answer.z = z.current_value();
    return answer;
  }
  /**
   * @brief Set the threshold for all three axes.
   * @param new_threshold Threshold value to apply.
   * @return The threshold value that was set.
   */
  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    z.threshold = new_threshold;
    return new_threshold;
  }
};

}