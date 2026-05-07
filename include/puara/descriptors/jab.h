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
 * @code
 * double accel = 0.0;
 * puara_gestures::Jab jab(&accel);
 * jab.threshold = 3; // choose how strong the motion must be
 *
 * // in your sensor loop:
 * accel = sensor.read();
 * jab.update();
 * double score = jab.current_value();
 * @endcode
 *
 * You can also use `update(data)` directly if you do not want to tie an
 * external variable. `Jab2D` and `Jab3D` follow the same pattern for 2D and 3D axes.
 */
class Jab
{
public:
  /**
   * @brief Threshold for detected jab motion.
   *
   * A jab is reported when the difference between the monitored maximum and
   * minimum values exceeds this threshold.
   */
  int threshold{};

  /**
   * @brief Default constructor.
   *
   * Initializes the threshold to 5 and leaves the tied data pointer empty.
   */
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

  /**
   * @brief Update the jab detector using a direct input sample.
   *
   * The detector tracks recent minimum and maximum values and produces a
   * compact intensity score when the axis motion exceeds the threshold.
   *
   * @param data Current value on the monitored axis.
   * @return Computed jab score after update.
   */
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

  /**
   * @brief Update the detector from a `Coord1D` sample.
   * @param reading The sampled coordinate containing the X axis value.
   * @return 1 when the update is processed.
   */
  int update(Coord1D reading)
  {
    Jab::update(reading.x);
    return 1;
  }

  /**
   * @brief Update the detector using the tied external input value.
   *
   * @return 1 if the tied value exists and the update was processed;
   *         0 otherwise.
   */
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

  /**
   * @brief Get the current jab score.
   * @return The latest computed jab intensity value.
   */
  double current_value() const { return value; }

  /**
   * @brief Connect the detector to an external `Coord1D` source.
   * @param new_tie Pointer to the external coordinate struct.
   * @return 1 when the tie succeeds.
   */
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
 * @code
 * puara_gestures::Coord2D accel{0.0, 0.0};
 * puara_gestures::Jab2D jab2d(&accel);
 * jab2d.threshold(2);
 *
 * // in your loop:
 * accel.x = sensor.readX();
 * accel.y = sensor.readY();
 * jab2d.update();
 * auto result = jab2d.current_value();
 * // result.x and result.y contain the latest jab scores.
 * @endcode
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

  /**
   * @brief Update both X and Y jab detectors using direct input values.
   * @param readingX X-axis acceleration reading.
   * @param readingY Y-axis acceleration reading.
   * @return 1 when the update is processed.
   */
  int update(double readingX, double readingY)
  {
    x.update(readingX);
    y.update(readingY);
    return 1;
  }

  /**
   * @brief Update both X and Y jab detectors from a coordinate sample.
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
   * @brief Update both X and Y detectors using tied external input values.
   * @return 1 when the update is processed.
   */
  int update()
  {
    x.update();
    y.update();
    return 1;
  }

  /**
   * @brief Adjust the detector frequency or smoothing behavior.
   * @param freq Frequency value to apply.
   * @return The frequency value that was set.
   */
  double frequency(double freq);

  /**
   * @brief Get the current per-axis jab score.
   * @return The current 2D jab values for X and Y.
   */
  Coord2D current_value() const
  {
    Coord2D answer;
    answer.x = x.current_value();
    answer.y = y.current_value();
    return answer;
  }

  /**
   * @brief Set a common threshold for both X and Y detectors.
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
 * @brief Simple 3D jab detector.
 *
 * Jab3D combines three Jab detectors to watch X, Y and Z separately.
 * It is useful when you want a lightweight motion score for full 3D
 * accelerometer motion.
 *
 * Example:
 * @code
 * puara_gestures::Coord3D accel{0.0, 0.0, 0.0};
 * puara_gestures::Jab3D jab3d(&accel);
 * jab3d.threshold(2);
 *
 * // in your loop:
 * accel.x = sensor.readX();
 * accel.y = sensor.readY();
 * accel.z = sensor.readZ();
 * jab3d.update();
 * auto result = jab3d.current_value();
 * // result.x, result.y and result.z hold jab intensities.
 * @endcode
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

  /**
   * @brief Update the detectors using direct X/Y/Z input values.
   * @param readingX X-axis acceleration reading.
   * @param readingY Y-axis acceleration reading.
   * @param readingZ Z-axis acceleration reading.
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
   * @brief Update the detectors from a 3D coordinate sample.
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
   * @brief Update the detectors using tied external input values.
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
   * @brief Adjust the detector frequency or smoothing behavior.
   * @param freq Frequency value to apply.
   * @return The frequency value that was set.
   */
  double frequency(double freq);

  /**
   * @brief Get the current 3D jab score.
   * @return The latest computed jab values for X, Y, and Z.
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
   * @brief Set a common threshold for X, Y, and Z jab detectors.
   * @param new_threshold Threshold value to apply.
   * @return The threshold value that was set.
   */
  double threshold(double new_threshold)
  {
    x.threshold = new_threshold;
    y.threshold = new_threshold;
    z.threshold = new_threshold;
    return new_threshold;
  };
};
}