//********************************************************************************//
// Puara Gestures - Template (.h)                                                 //
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
 * @brief This is a template of a gestural descriptor.
 * 
 * It shows different instantiation practices to tie (connect) the class to a variable
 * or puara structure. This connection is usefull to facilitate the call for updating
 * the descriptor.
 * 
 */
class Template
{
public:
  /**
   * @brief We declare the internal parameters and variables here. The variable
   * value (private, at the end of this class) is used to store the last calculated 
   * descriptor value for future access.
   */
  int config_parameter{};

  /**
   * @brief Basic construct with no parameters.
   * 
   */
  Template() noexcept
      : config_parameter(1)
      , tied_data(nullptr)
  {
  }

  Template(const Template&) noexcept = default;
  Template(Template&&) noexcept = default;
  Template& operator=(const Template&) noexcept = default;
  Template& operator=(Template&&) noexcept = default;

  /**
   * @brief Constructor that ties the class to the external variable with type double.
   * 
   * @param tied 
   */
  explicit Template(double* tied)
      : config_parameter(1)
      , tied_data(tied)
  {
  }

  /**
   * @brief Constructor that ties the class to the external variable with the puara
   * type Coord1D. The struct member x is used as the tied variable.
   * 
   * @param tied 
   */
  explicit Template(Coord1D* tied)
      : config_parameter(1)
      , tied_data(&(tied->x))
  {
  }

  /**
   * @brief The update function usually calculates the descriptor (gesture), stores it 
   * on the internal variable (e.g., stored_value), and returns it.
   * 
   * In some cases, it is preferable to return 1 to indicate success, and use another 
   * function, e.g., current_value(), to access the value.
   * 
   */
  double update(double data)
  {
    /**
     * Add your code to extract/calculate the descriptor here.
     */
    value = data + data;

    return value;
  }

  /**
   * @brief This is a wrapper to allow the user to use a puara-specific structure
   * to calculate the descriptor.
   * 
   * @param reading (Coord1D) The puara-specific structure to be used.
   */
  double update(Coord1D reading)
  {
    return Template::update(reading.x);
  }

  /**
   * @brief This is another wrapper to allow the user to use the tied variable to
   * calculate the descriptor.
   * 
   */
  double update()
  {
    if (tied_data != nullptr)
    {
      return Template::update(*tied_data);
    }
    else
    {
      return 0;
    }
  }

  /**
   * @brief Get the current value of the descriptor.
   * 
   * @return double 
   */
  double current_value() const { return value; }

  /**
   * @brief This function allows the user to set a new tied variable for the class.
   * 
   * @param new_tie The new tied variable to be set.
   */
  int tie(Coord1D* new_tie)
  {
    tied_data = &(new_tie->x);
    return 1;
  }

  /**
   * @brief This function also allows the user to set a new tied variable for the class.
   * 
   * @param new_tie The new tied variable to be set.
   */
  int tie(double* new_tie)
  {
    tied_data = new_tie;
    return 1;
  }

/**
 * @brief private variables: the tied_data pointer and the internal value variable. 
 * 
 */
private:
  const double* tied_data{};
  double value = 0;
};
} // namespace puara_gestures

/**
 * Please check other descriptors such as Jab for examples on internal wrappers to 
 * allow multidimensional gestures based on their 1DoF counterparts.
 */

