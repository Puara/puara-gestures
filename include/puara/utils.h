//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <puara/structs.h>
#include <puara/utils/circularbuffer.h>
#include <puara/utils/leakyintegrator.h>
#include <puara/utils/maprange.h>
#include <puara/utils/rollingminmax.h>
#include <puara/utils/smooth.h>
#include <puara/utils/threshold.h>
#include <puara/utils/wrap.h>

#include <cmath>

#include <chrono>

namespace puara_gestures::utils
{

/**
 *  @brief Simple function to get the current elapsed time in microseconds.
 */
inline unsigned long long getCurrentTimeMicroseconds()
{
  auto currentTimePoint = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      currentTimePoint.time_since_epoch());
  return duration.count();
}

/**
 * @brief Computes the average of elements in an array over a specified range.
 *
 * @tparam T The type of the elements in the array. Must be an arithmetic type (e.g., int, float, double).
 * @param array Pointer to the array of elements.
 * @param start The starting index of the range (inclusive). Must be >= 0.
 * @param end The ending index of the range (exclusive). Must be >= start.
 * @return The average of the elements in the specified range as a float. Returns 0.0 if the range is invalid or empty.
 */
template <typename T>
float arrayAverage(const T* array, int start, int end)
{
  static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type!");
  assert(start >= 0 && end >= start);

  float sum{};
  for(int i = start; i < end; ++i)
    sum += static_cast<float>(array[i]);

  const auto count = end - start;
  return (count > 0) ? (sum / count) : 0.0;
}

/**
 * @brief Function used to reduce feature arrays into single values.
 * E.g., brush uses it to reduce multiBrush instances. Any value in
 * the passed Array that is == 0 is ignored in the average calculation.
 */
inline double arrayAverageZero(double* Array, int ArraySize)
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
 * @brief Function used to reduce feature arrays into single values.
 * E.g., brush uses it to reduce multiBrush instances. Any value in
 * the passed Array that is == 0 is ignored in the average calculation.
 */
template <typename T>
inline double arrayAverageZero(T* Array, int ArraySize)
{
  static_assert(std::is_same<decltype(T::value), float>::value, "T must have a public float value member");

  double sum = 0.0;
  int count = 0;
  double output = 0.0;

  for(int i = 0; i < ArraySize; ++i)
  {
    if(Array[i].value != 0.0f)
    {
      sum += Array[i].value;
      count++;
    }
  }

  if(count > 0)
    output = sum / count;

  return output;
}

/**
 * @brief Legacy function used to calculate 1D blob detection in older
 * digital musical instruments
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
 * @brief Convert polar coordinates to cartesian
 * 
 */
inline Coord3D polar_to_cartesian(Spherical polarCoords)
{
  Coord3D cartesianCoords;

  cartesianCoords.x = polarCoords.r * cos(polarCoords.theta) * cos(polarCoords.phi);
  cartesianCoords.y = polarCoords.r * cos(polarCoords.theta) * sin(polarCoords.phi);
  cartesianCoords.z = polarCoords.r * sin(polarCoords.theta);

  return cartesianCoords;
}

/**
 * @brief Convert cartesian coordinates to polar
 * 
 */
inline Spherical cartesian_to_polar(Coord3D cartesianCoords)
{
  Spherical polarCoords;

  polarCoords.r = sqrt(
      cartesianCoords.x * cartesianCoords.x + cartesianCoords.y * cartesianCoords.y
      + cartesianCoords.z * cartesianCoords.z);

  polarCoords.theta = atan2(
      cartesianCoords.z, sqrt(
                             cartesianCoords.x * cartesianCoords.x
                             + cartesianCoords.y * cartesianCoords.y));

  polarCoords.phi = atan2(cartesianCoords.y, cartesianCoords.x);

  return polarCoords;
}

}
}
