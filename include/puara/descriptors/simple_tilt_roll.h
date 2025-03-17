//********************************************************************************//
// Puara Gestures - Simple Tilt / Roll (.h)                                       //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2025) - https://www.edumeneses.com                                //
//********************************************************************************//
#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @brief This class creates a simpler tilt and roll extractor for IMUs that don't have
 * magnetometers.
 * 
 * It expects 6DoF IMU data: accelerometer X, Y, Z, and gytoscope X, Y, Z (all type double).
 *
 * This class can use `tied_data`, which is an external variable that users update on their own.
 * Users can then call update() without any argumments to extract the features from the `tied_data`.
* The usage should be:
* setup:
*   - user creates variable, e.g. sensor_data
* user instantiates the class, e.g. puara::Tilt_Roll my_tilt_roll(sensor_data)
* loop/task:
*   - user saves sensor value into sensor_data
*   - user/task calls my_tilt_roll.update()
*   - user accesses values with my_tilt_roll.current_tilt_value() or my_tilt_roll.current_roll_value()
 */
class Tilt_Roll
{
public:
  Tilt_Roll() noexcept
      : tied_x(nullptr)
      , tied_y(nullptr)
      , tied_z(nullptr)
  {
  }

  Tilt_Roll(const Tilt_Roll&) noexcept = default;
  Tilt_Roll(Tilt_Roll&&) noexcept = default;
  Tilt_Roll& operator=(const Tilt_Roll&) noexcept = default;
  Tilt_Roll& operator=(Tilt_Roll&&) noexcept = default;

  explicit Tilt_Roll(Coord3D* tied)
    : tied_x(&(tied->x))
    , tied_y(&(tied->y))
    , tied_z(&(tied->z))
  {
  }

  int update(double accelx, double accely, double accelz)
  {
    // calculate polar representation of accelerometer data
    roll = atan2(accelz, accely);
    magnitude = sqrt(pow(accelz, 2) + pow(accely, 2));
    tilt = atan2(accelx, magnitude);
    magnitude = sqrt(pow(accelx, 2) + pow(magnitude, 2));
    magnitude *= 0.00390625;

    return 1;
  }

  int update(Coord3D imu_data)
  {
    update(imu_data.x, imu_data.y, imu_data.z);
    return 1;
  }

  int update()
  {
    if(tied_x != nullptr || tied_y != nullptr || tied_z != nullptr)
    {
      update(*tied_x, *tied_y, *tied_z);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  Simple_Orientation current_value() const
  {
    return Simple_Orientation{roll, tilt, magnitude};
  }

  double current_roll_value() const
  {
    return roll;
  }

  double current_tilt_value() const
  {
    return tilt;
  }

  int tie(Coord3D* new_tie)
  {
    tied_x = &(new_tie->x);
    tied_y = &(new_tie->y);
    tied_z = &(new_tie->z);
    
    return 1;
  }

private:
  const double* tied_x;
  const double* tied_y;
  const double* tied_z;
  double roll = 0;
  double tilt = 0;
  double magnitude = 0;
};

} // namespace puara_gestures
