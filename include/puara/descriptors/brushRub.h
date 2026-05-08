/**
* @file brushRub.h
* @brief Simple "brush" and "rub" touch features based on movement input.
* @details
* This file contains touch feature integration helpers used by descriptor
* classes that detect brush and rub motion.
* @defgroup puara_gestures_descriptors Gesture descriptors
* @ingroup puara_gestures
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

#include <cmath>
#include <puara/utils.h>
#include <puara/utils/leakyintegrator.h>


namespace puara_gestures
{

/**
 * @class ValueIntegrator
 * @brief Internal base class for brush/rub feature integration.
 *
 * @ingroup puara_gestures_descriptors
 * @details
 * ValueIntegrator is an implementation detail used by the public `Brush`
 * and `Rub` classes below. It provides the shared leaky-integrator logic
 * and handles tied input values, while the derived classes define how
 * movement is converted into a brush or rub feature.
 *
 * @note
 * This class is not intended to be used directly by most library users.
 * See `Brush` and `Rub` later in this file for the public touch feature API.
 */

class ValueIntegrator
{
public:
  ValueIntegrator() = default;
  ValueIntegrator(const ValueIntegrator&) noexcept = default;
  ValueIntegrator(ValueIntegrator&&) noexcept = default;
  ValueIntegrator& operator=(const ValueIntegrator&) noexcept = default;
  ValueIntegrator& operator=(ValueIntegrator&&) noexcept = default;

  /**
   * @brief The computed feature value (e.g., intensity or rate).
   * Initialized to 0. Measured in ~cm/s.
   */
  double value{};

  /**
   * @brief Pointer to external data that the feature is tied to.
   * @see Jab for example usage.
   */
  const double* tied_data{nullptr};

  /**
   * @brief Resets the feature value to its default state.
   */
  void reset() { value = 0; }

  double prevValue{};

  /**
   * @brief Update the feature with a new raw input value.
   * @param newValue The new input sample used to compute the integrated value.
   */
  void update(double newValue)
  {
    const auto delta = newValue - prevValue;
    prevValue = newValue;

    // No delta since the last update -> potentially reset the value
    if(delta == 0.0)
    {
      // Only reset the value once we've gotten 0 movements for 10 times
      if(++counter < 10.0)
        return;

      if(value < 0.001)
        reset();
      else
        integrate(delta);
    }
    // Large delta -> integrate with 0
    else if(std::abs(delta) > 1.0)
    {
      integrate(0.0);
    }
    // Goldilocks delta -> integrate and reset the counter
    else
    {
      integrate(delta);
      counter = 0;
    }
  }

  /**
   * @brief Updates the feature using the tied value.
   * Ensures that the tied value is not null and synchronizes the tied
   * value with the feature's updated value.
   * @return True if the update was successful; false if `tied_value` is null.
   */
  bool update()
  {
    if(tied_data == nullptr)
    {
      assert(false && "tied_value cannot be null!");
      return false;
    }

    ValueIntegrator::update(*tied_data);
    return true;
  }

  /**
   * @brief Ties the feature to an external value.
   * @param new_tie Pointer to the external value to tie the feature to.
   */
  void tie(double& new_tie)
  {
    tied_data = &new_tie;
  }

protected:
  ValueIntegrator(double* tied)
      : tied_data(tied)
  {
  }

  /**
   * @brief A leaky integrator for smoothing the feature values.
   */
  utils::LeakyIntegrator integrator{0.0f, 0.0f, 0.7f, 100, 0};

private:
  /**
   * @brief Abstract method to integrate input into the feature value.
   * @param input The input value to be integrated.
   */
  virtual void integrate(double input) = 0;

  /**
   * @brief A counter for tracking consecutive zero-movement updates.
   */
  int counter{};
};

//======================================================================

/**
 * @class Brush
 * @brief "Brush" touch feature that integrates directional movement input.
 * @ingroup puara_gestures_descriptors
 * @details The brush feature value increases with movement in a specific direction,
 * making it ideal for detecting directional gestures like swipes or strokes.
 *
 * @code
 * Brush brush;
 * brush.update(0.7);
 * double brushValue = brush.value; // Access the brush feature value
 * @endcode
 */
class Brush : public ValueIntegrator
{
public:
  Brush() = default;

  /**
   * @brief Constructs a Brush feature tied to an external value.
   * @param tied Pointer to the external value to tie the feature to.
   */
  explicit Brush(double* tied)
      : ValueIntegrator(tied)
  {
  }

private:
  /**
   * @brief Integrates movement input into the brush feature value.
   * Applies a scaling factor to the input and uses the leaky integrator.
   * @param movement The input movement to integrate.
   */
  void integrate(double movement) override
  {
    value = integrator.integrate(movement * .15);
  }
};

//======================================================================

/**
 * @class Rub
 * @brief "Rub" touch feature that integrates bidirectional movement input.
 * @ingroup puara_gestures_descriptors
 *
 * @details The rub feature value is based on the absolute value of movement.
 * This increases as the sensors are rubbed in any direction, making it ideal
 * for detecting bidirectional or multidirectional gestures.
 *
 * Example:
 * @code
 * Rub rub;
 * rub.update(0.7);
 * double rubValue = rub.value; // Access the rub feature value
 * @endcode
 */
class Rub : public ValueIntegrator
{
public:
  Rub() = default;

  /**
   * @brief Constructs a Rub feature tied to an external value.
   * @param tied Pointer to the external value to tie the feature to.
   */
  explicit Rub(double* tied)
      : ValueIntegrator(tied)
  {
  }

private:
  /**
   * @brief Integrates movement input into the rub feature value.
   * Takes the absolute value of the input before applying the leaky integrator.
   * @param movement The input movement to integrate.
   */
  void integrate(double movement) override
  {
    value = integrator.integrate(std::abs(movement * .15));
  }
};

//======================================================================

/**
 * @class BrushRubDetector
 * @brief Detector that updates both brush and rub features.
 * @ingroup puara_gestures_descriptors
 * @details BrushRubDetector is a convenience class that combines both `Brush` and `Rub` features.
 * It allows you to update both features simultaneously from a shared input value.
 * This is useful when you want to track both directional and bidirectional movement features together.
 *
 * Example:
 * @code
 * BrushRubDetector detector;
 * detector.update(0.5);
 * double brushValue = detector.brush.value; // Access the brush feature value
 * double rubValue = detector.rub.value;     // Access the rub feature value
 * @endcode
 */
class BrushRubDetector
{
public:
  Brush brush;
  Rub rub;

  /**
   * @brief Update both brush and rub features from a shared input.
   * @param newData The new input value used by each detector.
   */
  void update(double newData)
  {
    brush.update(newData);
    rub.update(newData);
  }
};

}