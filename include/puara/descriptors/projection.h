//********************************************************************************//
// Puara Gestures - Projection (.h)                                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Zachary L'Heureux (2025)                                                          //
//********************************************************************************//
#pragma once

#define _USE_MATH_DEFINES

#include <puara/structs.h>
#include <puara/utils.h>
#include <cmath>

namespace puara_gestures
{

/**
 * @brief This class calculates a 2D projection of a point based on rotation angles and radius from the origin.
 * 
 * Calculation is based on this math: https://farside.ph.utexas.edu/teaching/celestial/Celestial/node122.html
 * 
 * It expects a Coord3D struct containing euler angles in degrees, in order of yaw, pitch, roll (x,y,z).
 *
 * Projection can use `tied_data`, which is an external variable that users update on their own.
 * Users can then call update() without any arguments to extract the features from the `tied_data`.
* The usage should be:
* setup:
*   - user creates variable, e.g. sensor_data
* user instantiates the class, e.g. puara::Projection2D my_projection(sensor_data)
* loop/task:
*   - user saves sensor value into sensor_data
*   - user/task calls my_projection.update()
*   - user accesses projection value with my_projection.current_value()
 */
class Projection2D
{
public:
  int projectionRadius{};
  double xPosition = 0;
  double yPosition = 0;

  Projection2D() noexcept
      : tied_data(nullptr)
  {
  }

  Projection2D(const Projection2D&) noexcept = default;
  Projection2D(Projection2D&&) noexcept = default;
  Projection2D& operator=(const Projection2D&) noexcept = default;
  Projection2D& operator=(Projection2D&&) noexcept = default;


  explicit Projection2D(Coord3D* tied)
      : projectionRadius(5)
      , tied_x(&(tied->x))
      , tied_y(&(tied->y))
      , tied_z(&(tied->z))
  {
  }

  int update(double data_x, double data_y, double data_z){
    double xAngle = data_x;
    double yAngle = data_y;
    double zAngle = data_z;

    // Convert angles to radians and find X and Y positions from pitch and roll
    double xRadians = sin(yAngle*M_PI/180);
    double yRadians = sin(zAngle*M_PI/180);
    // Apply yaw rotation to offset the X and Y positions
    xPosition = projectionRadius * (xRadians * (cos(xAngle*M_PI/180))) - (yRadians * (sin(xAngle*M_PI/180)));
    yPosition = projectionRadius * (xRadians * (sin(xAngle*M_PI/180))) + (yRadians * (cos(xAngle*M_PI/180)));

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

};
}
