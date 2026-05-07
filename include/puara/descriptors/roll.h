//********************************************************************************//
// Puara Gestures - Roll (.h)                                                     //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
// Maggie Needham (2024)                                                          //
//********************************************************************************//

#pragma once

#include "IMU_Sensor_Fusion/imu_orientation.h"
#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{

/**
 * @brief Measure roll gestures using 3DoF data from accelerometer, gyroscope, and magnetometer.
 *
 * This class wraps an IMU orientation filter and provides optional unwrap,
 * smoothing, and wrapping helpers for roll values.
 *
 * Example:
 * @code
 * #include <puara/descriptors/roll.h>
 *
 * puara_gestures::Coord3D accel{0.0, 0.0, 1.0};
 * puara_gestures::Coord3D gyro{0.0, 0.0, 0.0};
 * puara_gestures::Coord3D mag{0.3, 0.0, 0.5};
 * puara_gestures::Roll roll;
 *
 * double value = roll.roll(accel, gyro, mag, 0.01);
 * double smoothValue = roll.smooth(value);
 * @endcode
 */
class Roll
{
public:
  IMU_Orientation orientation;
  utils::Unwrap unwrapper;
  utils::Smooth smoother;
  utils::Wrap wrapper;

  /**
   * @brief Default construct a Roll detector.
   *
   * The default constructor configures unwrap to [-PI, PI], smoothing over 50
   * samples, and wrapping to the range [0, 2*PI].
   */
  Roll()
      : unwrapper(-M_PI, M_PI)
      , smoother(50)
      , wrapper{0, 2 * M_PI}
  {
  }

  /**
   * @brief Construct a Roll detector with custom smoothing.
   *
   * @param smoothValue Number of previous values that the smoother averages.
   */
  explicit Roll(double smoothValue)
      : unwrapper(-M_PI, M_PI)
      , smoother(smoothValue)
      , wrapper{0, 2 * M_PI}
  {
  }

  /**
   * @brief Construct a Roll detector with custom wrapping range.
   *
   * @param wrapMin Minimum value of the output wrap range.
   * @param wrapMax Maximum value of the output wrap range.
   */
  Roll(double wrapMin, double wrapMax)
      : unwrapper(-M_PI, M_PI)
      , smoother(50)
      , wrapper{wrapMin, wrapMax}
  {
  }

  /**
   * @brief Construct a Roll detector with custom smoothing and wrap range.
   *
   * @param smoothValue Number of previous values that the smoother averages.
   * @param wrapMin Minimum value of the output wrap range.
   * @param wrapMax Maximum value of the output wrap range.
   */
  Roll(double smoothValue, double wrapMin, double wrapMax)
      : unwrapper(-M_PI, M_PI)
      , smoother(smoothValue)
      , wrapper{wrapMin, wrapMax}
  {
  }

  /**
   * @brief Calculate the current roll angle from IMU data.
   *
   * @param accel Accelerometer values in Gs.
   * @param gyro Gyroscope values in degrees per second.
   * @param mag Magnetometer values in Gauss.
   * @param period_sec Time since the previous sample in seconds.
   * @return Roll angle in radians, normally in the range [-PI, PI].
   */
  double roll(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec)
  {
    orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
    orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period_sec);
    orientation.setMagnetometerValues(mag.x, mag.y, mag.z);
    orientation.update(0.01);
    return orientation.euler.roll;
  }

  /**
   * @brief Unwrap a roll value to avoid discontinuities.
   *
   * This is useful when consecutive roll values should appear as a continuous
   * increase or decrease instead of jumping at the wrap boundary.
   *
   * @param reading Roll value to unwrap.
   * @return Unwrapped roll value.
   */
  double unwrap(double reading) { return unwrapper.unwrap(reading); }

  /**
   * @brief Smooth a roll value by averaging recent samples.
   *
   * The default smooth window is 50 samples.
   * @param reading Roll value to smooth.
   * @return Smoothed roll value.
   */
  double smooth(double reading) { return smoother.smooth(reading); }

  /**
   * @brief Wrap a roll value into the configured range.
   *
   * The default wrapping range is [0, 2 * PI].
   * @param reading Roll value to wrap.
   * @return Wrapped roll value.
   */
  double wrap(double reading) { return wrapper.wrap(reading); }

  /**
   * @brief Reset unwrap state.
   *
   * Clears the internal unwrapping history so subsequent values are treated
   * as a new sequence.
   */
  void clear_unwrap() { unwrapper.clear(); }

  /**
   * @brief Clear the smoothing history.
   *
   * After this call, the smoother starts fresh without previously accumulated values.
   */
  void clear_smooth() { smoother.clear(); }
};
}
