#pragma once

#include <vector>

namespace puara_gestures
{

struct Coord1D
{
  double x;
};

struct Coord2D
{
  double x, y;
};

struct Coord3D
{
  double x, y, z;
};

struct Simple_Orientation
{
  double roll, tilt, magnitude;
};

struct Spherical
{
  double azimuth;
  double& phi = azimuth;
  double& yaw = azimuth;
  double elevation;
  double& theta = elevation;
  double& pitch = elevation;
  double distance;
  double& r = distance;
  double roll;
};

struct Quaternion
{
  double w, x, y, z;
};

struct Imu6Axis
{
  Coord3D accl, gyro;
};

struct Imu9Axis
{
  Coord3D accl, gyro, magn;
};

struct DiscreteArray
{
  std::vector<int> arr;
  DiscreteArray(int n)
      : arr(std::vector<int>(n))
  {
  }
};

template <typename T>
struct MinMax
{
  T min;
  T max;
};

}
