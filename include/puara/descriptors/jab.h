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
 * @brief Simple 1D jab detector.
 *
 * Use Jab to watch one acceleration axis and turn a quick change into a
 * compact score. This is not a full gesture recognizer; it is a low-cost
 * feature extractor that reports the size of a recent jab movement.
 *
 * Example:
 *   double accel = 0.0;
 *   puara_gestures::Jab jab(&accel);
 *   jab.threshold = 3; // choose how strong the motion must be
 *
 *   // in your sensor loop:
 *   accel = sensor.read();
 *   jab.update();
 *   double score = jab.current_value();
 *
 *   // score grows when acceleration swings exceed the threshold.
 *
 * You can also use update(data) directly if you do not want to tie an external
 * variable. `Jab2D` and `Jab3D` follow the same pattern for 2D and 3D axes.
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
 * @brief Simple 2D jab detector.
 *
 * Jab2D combines two Jab detectors to report jab-like motion on X and Y.
 * It is useful when your sensor has two acceleration axes and you want a
 * compact per-axis motion score instead of raw accelerometer values.
 *
 * Example:
 *   puara_gestures::Coord2D accel{0.0, 0.0};
 *   puara_gestures::Jab2D jab2d(&accel);
 *   jab2d.threshold(2);
 *
 *   // in your loop:
 *   accel.x = sensor.readX();
 *   accel.y = sensor.readY();
 *   jab2d.update();
 *   auto result = jab2d.current_value();
 *   // result.x and result.y contain the latest jab scores.
 */
class Jab2D
{
public:
  Jab x{}, y{};

  Jab2D() = default;
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

  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    return new_threshold;
  }
};

/**
 * @brief Simple 3D jab detector.
 *
 * Jab3D combines three Jab detectors to watch X, Y and Z separately.
 * It is useful when you want a lightweight motion score for full 3D
 * accelerometer motion.
 *
 * Example:
 *   puara_gestures::Coord3D accel{0.0, 0.0, 0.0};
 *   puara_gestures::Jab3D jab3d(&accel);
 *   jab3d.threshold(2);
 *
 *   // in your loop:
 *   accel.x = sensor.readX();
 *   accel.y = sensor.readY();
 *   accel.z = sensor.readZ();
 *   jab3d.update();
 *   auto result = jab3d.current_value();
 *   // result.x, result.y and result.z hold jab intensities.
 */
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

  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    z.threshold = new_threshold;
    return new_threshold;
  };
};
}
