//********************************************************************************//
// Puara Gestures - Projection (.h)                                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Zachary L'Heureux (2025)                                                          //
//********************************************************************************//
#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

namespace puara_gestures
{

/**
 * @brief Zach add description here
 * 
 * This class creates jab gestures using 1DoF info.
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
class Projection2D
{
public:
  int threshold{};

  Projection2D() noexcept
      : tied_data(nullptr)
  {
  }

  Projection2D(const Projection2D&) noexcept = default;
  Projection2D(Projection2D&&) noexcept = default;
  Projection2D& operator=(const Projection2D&) noexcept = default;
  Projection2D& operator=(Projection2D&&) noexcept = default;


  explicit Projection2D(Coord3D* tied)
      : threshold(5)
      , tied_x(&(tied->x))
      , tied_y(&(tied->y))
      , tied_z(&(tied->z))
  {
  }

  int update(double data_x, double data_y, double data_z){ // Zach adjust the function

    double xAngle = (offsetValue(data_x, xOffset, 0, 360));
    double yAngle = (offsetValue(data_y, yOffset, -180, 180));
    double zAngle = (offsetValue(data_z, zOffset, -90, 90));

    // Convert angles to radians and find X and Y positions
    float x = sin(yAngle*PI/180);
    float y = sin(zAngle*PI/180);
    xPosition = (x * (cos(xAngle*PI/180))) - (y * (sin(xAngle*PI/180)));
    yPosition = (x * (sin(xAngle*PI/180))) + (y * (cos(xAngle*PI/180)));
    
    Serial.print("X Position: ");
    Serial.println(xPosition);
    Serial.print("Y Position: ");
    Serial.println(yPosition);

    return 1;
  }

  int update(Coord3D reading)
  {
    Projection2D::update(reading.x, reading.y, reading.z);
    return 1;
  }

  int update()
  {
    if(tied_data != nullptr)
    {
      Projection2D::update(*tied_data);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without a tied_value?
      return 0;
    }
  }

  double current_value() const { return value; }

  int tie(Coord3D* new_tie)
  {
    tied_x = &(new_tie->x);
    tied_y = &(new_tie->y);
    tied_z = &(new_tie->z);
    return 1;
  }

private:
  const double* tied_data{};
  double value = 0;

  /** Keep track of the min and max values over the last 10 times Jab::update() was called. */
  puara_gestures::utils::RollingMinMax<double> minmax{};
};
}
