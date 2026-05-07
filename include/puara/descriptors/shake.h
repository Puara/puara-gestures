//********************************************************************************//
// Puara Gestures - Shake (.h)                                                    //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @brief Simple 1D shake detector.
 *
 * Shake turns raw acceleration energy into a slowly changing value.
 * It is useful when you want a smooth gesture-like signal for shaking,
 * vibration, or repeated movement on one axis.
 *
 * Example:
 *   double accel = 0.0;
 *   puara_gestures::Shake shake(&accel);
 *   shake.threshold = 0.2;
 *   shake.fast_leak = 0.6;
 *   shake.slow_leak = 0.3;
 *
 *   // in your sensor loop:
 *   accel = sensor.read();
 *   shake.update();
 *   double energy = shake.current_value();
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

  Shake()
      : tied_value(nullptr)
  {
  }
  explicit Shake(double* tied)
      : tied_value(tied)
  {
  }
  explicit Shake(Coord1D* tied)
      : tied_value(&(tied->x))
  {
  }

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

  int update(Coord1D reading)
  {
    Shake::update(reading.x);
    return 1;
  }

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

  double frequency() const { return integrator.frequency; }

  double frequency(double freq)
  {
    integrator.frequency = freq;
    return freq;
  }

  double current_value() const { return integrator.current_value; }

  int tie(Coord1D* new_tie)
  {
    tied_value = &(new_tie->x);
    return 1;
  }

private:
  double* tied_value{};
};

/**
 * @brief Simple 2D shake detector.
 *
 * Shake2D uses two Shake objects to track motion energy on X and Y.
 * This lets you detect repeated motion or vibration across two axes, while
 * still keeping the output smooth and easy to use.
 *
 * Example:
 *   puara_gestures::Coord2D accel{0.0, 0.0};
 *   puara_gestures::Shake2D shake2d(&accel);
 *   shake2d.threshold(0.2);
 *   shake2d.frequency(0); // deterministic update behavior for testing or simple loops
 *
 *   // in your loop:
 *   accel.x = sensor.readX();
 *   accel.y = sensor.readY();
 *   shake2d.update();
 *   auto energy = shake2d.current_value();
 *
 *   // energy.x and energy.y each report a smooth shake score.
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
  int update(double readingX, double readingY)
  {
    x.update(readingX);
    y.update(readingY);
    return 1;
  }

  int update(Coord2D reading)
  {
    x.update(reading.x);
    y.update(reading.y);
    return 1;
  }

  int update()
  {
    x.update();
    y.update();
    return 1;
  }

  double frequency(double freq)
  {
    x.frequency(freq);
    y.frequency(freq);
    return freq;
  }

  Coord2D current_value() const
  {
    Coord2D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    return answer;
  }

  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    return new_threshold;
  }
};

/**
 * @brief Simple 3D shake detector.
 *
 * Shake3D tracks motion energy on X, Y and Z by combining three Shake
 * processors. It is useful when you want a smooth, gesture-like output for
 * full 3D accelerometer movement.
 *
 * Example:
 *   puara_gestures::Coord3D accel{0.0, 0.0, 0.0};
 *   puara_gestures::Shake3D shake3d(&accel);
 *   shake3d.threshold(0.2);
 *   shake3d.frequency(0);
 *
 *   // on each sensor sample:
 *   accel.x = sensor.readX();
 *   accel.y = sensor.readY();
 *   accel.z = sensor.readZ();
 *   shake3d.update();
 *   auto energy = shake3d.current_value();
 *
 *   // energy.x, energy.y and energy.z each show a smooth shake score.
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

  explicit Shake3D(Coord3D* tied)
      : x(&(tied->x))
      , y(&(tied->y))
      , z(&(tied->z))
  {
  }

  int update(double readingX, double readingY, double readingZ)
  {
    x.update(readingX);
    y.update(readingY);
    z.update(readingZ);
    return 1;
  }

  int update(Coord3D reading)
  {
    x.update(reading.x);
    y.update(reading.y);
    z.update(reading.z);
    return 1;
  }

  int update()
  {
    x.update();
    y.update();
    z.update();
    return 1;
  }

  double frequency(double freq)
  {
    x.frequency(freq);
    y.frequency(freq);
    z.frequency(freq);
    return freq;
  }

  Coord3D current_value() const
  {
    Coord3D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    answer.z = z.current_value();
    return answer;
  }
  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    z.threshold = new_threshold;
    return new_threshold;
  }
};

}