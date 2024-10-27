//********************************************************************************//
// Puara Gestures - Jab (.h)                                                      //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//
#pragma once

#include "puara-structs.h"
#include "puara-utils.h"

#include <algorithm>
#include <deque>

namespace puara_gestures
{

/**
 * @brief This class creates jab gestures using 1DoF info.
 * 
 * It expects 1 axis of a accelerometer in m/s^2, but can be used
 * with any double or float.
 */
class Jab
{
public:
  int threshold{};

  Jab()
      : threshold(5)
      , tied_value(nullptr)
      , minmax(10)
  {
  }
  explicit Jab(double* tied)
      : threshold(5)
      , tied_value(tied)
      , minmax(10)
  {
  }
  explicit Jab(Coord1D* tied)
      : threshold(5)
      , tied_value(&(tied->x))
      , minmax(10)
  {
  }

  double update(double reading)
  {
    minmax.update(reading);
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
    if(tied_value != nullptr)
    {
      Jab::update(*tied_value);
      return 1;
    }
    else
    {
      return 0;
    }
  }

  double current_value() { return value; }

  int tie(Coord1D* new_tie)
  {
    tied_value = &(new_tie->x);
    return 1;
  }

private:
  double* tied_value{};
  double value = 0;
  puara_gestures::utils::RollingMinMax<double> minmax;
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
  Jab2D()
      : x()
      , y()
  {
  }
  Jab2D(Coord2D* tied)
      : x(&(tied->x))
      , y(&(tied->y))
  {
  }

  Jab x, y;
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
  Coord2D current_value()
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
  Jab3D() { }
  Jab3D(Coord3D* tied)
      : x(&(tied->x))
      , y(&(tied->y))
      , z(&(tied->z))
  {
  }

  Jab x, y, z;
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
  Coord3D current_value()
  {
    Coord3D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    answer.z = z.current_value();
    return answer;
  }
};
}
