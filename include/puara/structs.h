#pragma once

#include <vector>

namespace puara_gestures
{

/**
 * @brief Basic coordinate and sensor data structures used throughout puara-gestures.
 *
 * These lightweight structs are intended for simple numeric storage and
 * fast passing by value between gesture algorithms. They provide a common
 * representation for 1D/2D/3D coordinates, IMU data, and spherical values.
 */

/**
 * @brief One-dimensional coordinate or sensor sample.
 */
struct Coord1D
{
  double x = 0.0;
};

/**
 * @brief Two-dimensional coordinate or sensor sample.
 */
struct Coord2D
{
  double x = 0.0, y = 0.0;
};

/**
 * @brief Three-dimensional coordinate or sensor sample.
 */
struct Coord3D
{
  double x = 0.0, y = 0.0, z = 0.0;
};

struct Simple_Orientation
{
  double roll = 0.0, tilt = 0.0, magnitude = 0.0;
};

/**
 * @brief Spherical coordinate representation.
 *
 * This struct avoids the traditional phi/theta naming ambiguity by using
 * azimuth (horizontal angle) and elevation (vertical angle).
 *
 * Aliases are provided so users can also refer to:
 *   - yaw  = azimuth
 *   - pitch = elevation
 *   - r = distance
 */
struct Spherical
{
  double azimuth = 0.0;
  double& yaw = azimuth;
  double elevation = 0.0;
  double& pitch = elevation;
  double distance = 0.0;
  double& r = distance;
  double roll = 0.0;
};

/**
 * @brief Quaternion representation for 3D rotation or orientation.
 */
struct Quaternion
{
  double w = 1.0, x = 0.0, y = 0.0, z = 0.0;
};

/**
 * @brief Six-axis IMU sample containing accelerometer and gyroscope data.
 */
struct Imu6Axis
{
  Coord3D accl, gyro;
};

/**
 * @brief Nine-axis IMU sample containing accelerometer, gyroscope, and magnetometer data.
 */
struct Imu9Axis
{
  Coord3D accl, gyro, magn;
};

/**
 * @brief Simple container for a fixed-sized integer array.
 *
 * Used by algorithms that expect a vector-like storage with a known size.
 */
struct DiscreteArray
{
  std::vector<int> arr;
  DiscreteArray(int n) : arr(std::vector<int>(n)) {}
};

template <typename T>
struct MinMax
{
  T min;
  T max;
};
}
