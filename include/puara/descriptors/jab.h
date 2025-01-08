//********************************************************************************//
// Puara Gestures - Jab (.h)                                                      //
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
 * @brief This class creates jab gestures using 1DoF info.
 * 
 * It expects 1 axis of a accelerometer in m/s^2, but can be used
 * with any double or float.
 *
 * Jab can use `tied_data`, which is an external variable that users update on their own.
 * Users can then call update() without any argumments to extract the features from the `tied_data`.
* The usage should be:
* setup:
*   - user creates variable, e.g. sensor_data
* user instantiates the class, e.g. puara::jab my_jab(sensor_data)
* loop/task:
*   - user saves sensor value into sensor_data
*   - user/task calls my_jab.update()
*   - user accesses jab value with my_jab.current_value()
 */
class Jab
{
public:
  int threshold{};

  Jab() noexcept
      : threshold(5)
      , tied_data(nullptr)
  {
  }

  Jab(const Jab&) noexcept = default;
  Jab(Jab&&) noexcept = default;
  Jab& operator=(const Jab&) noexcept = default;
  Jab& operator=(Jab&&) noexcept = default;

  explicit Jab(double* tied)
      : threshold(5)
      , tied_data(tied)
  {
  }

  explicit Jab(Coord1D* tied)
      : threshold(5)
      , tied_data(&(tied->x))
  {
  }

  double update(double data)
  {
    minmax.update(data);
    double min = minmax.current_value.min;
    double max = minmax.current_value.max;

    if(max - min > threshold)
    {
      if(max >= 0 && min >= 0)
      {
        value = max - min;
      }
      else if(max < 0 && min < 0)
      {
        value = min - max;
      }
      else
      {
        value = max - min; // for X, this line was originally min - max
      }
    }
    return value;
  }

  int update(Coord1D reading)
  {
    Jab::update(reading.x);
    return 1;
  }

  int update()
  {
    if(tied_data != nullptr)
    {
      Jab::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  double current_value() const { return value; }

  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

private:
  const double* tied_data{};
  double value = 0;

  /** Keep track of the min and max values over the last 10 times Jab::update() was called. */
  puara_gestures::utils::RollingMinMax<double> minmax{};
};

/**
 * @brief This class creates jab gestures using 2DoF info.
 * 
 * It expects 2 axis of a accelerometer in m/s^2, but can be used
 * with any double or float.
 */
class Jab2D
{
public:
  Jab x{}, y{};

  Jab2D() noexcept = default;
  Jab2D(const Jab2D&) noexcept = default;
  Jab2D(Jab2D&&) noexcept = default;
  Jab2D& operator=(const Jab2D&) noexcept = default;
  Jab2D& operator=(Jab2D&&) noexcept = default;

  explicit Jab2D(Coord2D* tied) noexcept
      : x(&(tied->x))
      , y(&(tied->y))
  {
  }

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

  double frequency(double freq);
  Coord2D current_value() const
  {
    Coord2D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    return answer;
  }
};

class Jab3D
{
public:
  Jab x{}, y{}, z{};

  Jab3D() = default;
  Jab3D(const Jab3D&) noexcept = default;
  Jab3D(Jab3D&&) noexcept = default;
  Jab3D& operator=(const Jab3D&) noexcept = default;
  Jab3D& operator=(Jab3D&&) noexcept = default;

  explicit Jab3D(Coord3D* tied)
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

  double frequency(double freq);
  Coord3D current_value() const
  {
    Coord3D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    answer.z = z.current_value();
    return answer;
  }
};
}
