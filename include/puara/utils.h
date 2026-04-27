//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>
#include <puara/utils/blobDetector.h>
#include <puara/utils/calibration.h>
#include <puara/utils/chrono.h>
#include <puara/utils/circularbuffer.h>
#include <puara/utils/discretizer.h>
#include <puara/utils/includeEigen.h>
#include <puara/utils/leakyintegrator.h>
#include <puara/utils/maprange.h>
#include <puara/utils/rollingminmax.h>
#include <puara/utils/smooth.h>
#include <puara/utils/threshold.h>
#include <puara/utils/wrap.h>
#include <puara/utils/kalmanQuaternion.h>
#include <puara/utils/madgwickQuaternion.h>
#include <puara/utils/mahonyQuaternion.h>

#include <cmath>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502984
#endif
#ifndef M_PI_2
#define M_PI_2 1.5707963267948966
#endif

namespace puara_gestures::utils
{

/**
 * @brief Utility helpers for gesture and sensor feature extraction.
 *
 * This header exposes a small collection of lightweight, embedded-friendly
 * helper functions for averaging, filtering, coordinate conversion, and
 * other common math tasks used throughout the library.
 */

/**
 * @brief Computes the average of elements in an array over a specified range.
 *
 * This is useful for smoothing and normalization. If the requested range is
 * empty or invalid, the function returns 0.0.
 *
 * Example:
 *   const int values[] = {1, 2, 3, 4, 5};
 *   float avg = arrayAverage(values, 1, 4); // averages 2,3,4 => 3.0
 *
 * @tparam T The type of elements in the array (must be arithmetic).
 * @param array Pointer to the array of values.
 * @param start Inclusive start index of the range.
 * @param end Exclusive end index of the range.
 * @return The average over [start,end), or 0.0 when the range is empty or invalid.
 */
template <typename T>
float arrayAverage(const T* array, int start, int end)
{
  static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type!");
  assert(start >= 0 && end >= start);

  const float sum = std::accumulate(array + start, array + end, 0.0f);
  const auto count = end - start;
  return (count > 0) ? (sum / count) : 0.0;
}

/**
 * @brief Function used to reduce feature arrays into single values.
 * E.g., brush uses it to reduce multiBrush instances. Any value in
 * the passed Array that is == 0 is ignored in the average calculation.
 */
inline double arrayAverageWithoutZero(double* Array, int ArraySize)
{
  double sum = 0;
  int count = 0;
  double output = 0;
  for(int i = 0; i < ArraySize; ++i)
  {
    if(Array[i] != 0)
    {
      sum += Array[i];
      count++;
    }
  }
  if(count > 0)
  {
    output = sum / count;
  }
  return output;
}

/**
 * @brief Computes the average of non-zero elements in an array.
 *
 * This is useful when zeros represent missing data or inactive sensor
 * values that should not influence the average.
 *
 * Example:
 *   double values[] = {1.0, 0.0, 3.0, 0.0, 5.0};
 *   double avg = arrayAverageWithoutZero(values, 5); // 3.0
 *
 * @param Array Pointer to the array of values.
 * @param ArraySize Number of elements in the array.
 * @return Average of non-zero values, or 0.0 if no non-zero values exist.
 */

/**
 * @brief Legacy function used to calculate 1D blob detection in older
 * digital musical instruments.
 */
inline void bitShiftArrayL(int* origArray, int* shiftedArray, int arraySize, int shift)
{
  for(int i = 0; i < arraySize; ++i)
  {
    shiftedArray[i] = origArray[i];
  }
  for(int k = 0; k < shift; ++k)
  {
    for(int i = 0; i < arraySize; ++i)
    {
      if(i == (arraySize - 1))
      {
        shiftedArray[i] = (shiftedArray[i] << 1);
      }
      else
      {
        shiftedArray[i] = (shiftedArray[i] << 1) | (shiftedArray[i + 1] >> 7);
      }
    }
  }
}

namespace convert
{

/**
 * @brief Convert g's to m/s^2
 *
 */
inline double g_to_ms2(double reading)
{
  return reading * 9.80665;
}

/**
 * @brief Convert m/s^2 to g's
 *
 */
inline double ms2_to_g(double reading)
{
  return reading / 9.80665;
}

/**
 * @brief Convert DPS to radians per second
 *
 */
inline double dps_to_rads(double reading)
{
  return reading * M_PI / 180;
}

/**
 * @brief Convert radians per second to DPS
 *
 */
inline double rads_to_dps(double reading)
{
  return reading * 180 / M_PI;
}

/**
 * @brief Convert Gauss to uTesla
 *
 */
inline double gauss_to_utesla(double reading)
{
  return reading / 10000;
}

/**
 * @brief Convert uTesla to Gauss
 *
 */
inline double utesla_to_gauss(double reading)
{
  return reading * 10000;
}

/**
 * @brief Convert spheric coordinates to cartesian coordinates using 
 * the ISO convention and mathematical approach. Phi and Theta debate is 
 * replaced by azimuth and elevation to reduce confusion between conventions.
 * In these conventions, the default "pointing" direction is along the 
 * positive z-axis.
 * See : https://en.wikipedia.org/wiki/Spherical_coordinate_system
 */
inline Coord3D spheric_to_cartesian(Spherical polarCoords)
{
  Coord3D cartesianCoords;

  cartesianCoords.x
      = polarCoords.r * cos(polarCoords.azimuth) * sin(polarCoords.elevation);
  cartesianCoords.y
      = polarCoords.r * sin(polarCoords.elevation) * sin(polarCoords.azimuth);
  cartesianCoords.z = polarCoords.r * cos(polarCoords.elevation);

  return cartesianCoords;
}

/**
 * @brief Convert cartesian coordinates to spherical coordinates using 
 * the ISO convention and mathematical approach. Phi and Theta debate is 
 * replaced by azimuth and elevation to reduce confusion between conventions.
 * In these conventions, the default "pointing" direction is along the 
 * positive z-axis.
 * See : https://en.wikipedia.org/wiki/Spherical_coordinate_system
 */
inline Spherical cartesian_to_spheric(Coord3D cartesianCoords)
{
  Spherical sphericCoords;

  sphericCoords.r = sqrt(
      pow(cartesianCoords.x, 2) + pow(cartesianCoords.y, 2) + pow(cartesianCoords.z, 2));

  sphericCoords.elevation = acos(cartesianCoords.z / sphericCoords.r);

  sphericCoords.azimuth = atan2(cartesianCoords.y, cartesianCoords.x);

  return sphericCoords;
}

/**
 * @brief Convert rectangular coordinates to spherical coordinates using 
 * Matlab's phased convention where default "pointing" direction is along 
 * the positive x-axis rather than the positive z-axis.
 * See : https://www.mathworks.com/help/phased/ug/spherical-coordinates.html
 */
inline Spherical phased_cartesian_to_spheric(Coord3D cartesianCoords)
{
  Spherical sphericCoords;

  sphericCoords.r = sqrt(
      pow(cartesianCoords.x, 2) + pow(cartesianCoords.y, 2) + pow(cartesianCoords.z, 2));

  sphericCoords.elevation = atan2(
      cartesianCoords.z, sqrt(pow(cartesianCoords.x, 2) + pow(cartesianCoords.y, 2)));

  sphericCoords.azimuth = atan2(cartesianCoords.y, cartesianCoords.x);

  //sphericCoords.roll = 0; // should roll be left undefined after initiatlization ?

  return sphericCoords;
}

/**
 * @brief Convert spherical coordinates to rectangular coordinates using 
 * Matlab's phased convention where default "pointing" direction is along 
 * the positive x-axis rather than the positive z-axis.
 * See : https://www.mathworks.com/help/phased/ug/spherical-coordinates.html
 */
inline Coord3D phased_spheric_to_cartesian(Spherical sphericCoords)
{
  Coord3D cartesianCoords;

  cartesianCoords.x
      = sphericCoords.r * cos(sphericCoords.elevation) * cos(sphericCoords.azimuth);
  cartesianCoords.y
      = sphericCoords.r * cos(sphericCoords.elevation) * sin(sphericCoords.azimuth);
  cartesianCoords.z = sphericCoords.r * sin(sphericCoords.elevation);

  return cartesianCoords;
}

}
}
