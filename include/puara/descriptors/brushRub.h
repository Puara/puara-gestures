#include <puara/utils.h>
#include <puara/utils/leakyintegrator.h>

#include <cmath>

namespace puara_gestures
{

/**
 * @brief Base class representing a generic touch feature.
 * This class provides a framework for interpreting touch features based on
 * movement input. It uses a leaky integrator for smoothing and allows derived
 * classes to define specific integration behavior.
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
 * @brief Derived class implementing the "brush" touch feature.
 * The feature value is a signed value that increases or
 * decreases based on the brush movement direction.
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
 * @brief Derived class implementing the "rub" touch feature.
 * The feature value is based on the absolute value of movement. This increases
 * as the sensors are rubbed in any direction, making it ideal for detecting
 * bidirectional or multidirectional gestures.
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

class BrushRubDetector
{
public:
  Brush brush;
  Rub rub;

  void update(double newData)
  {
    brush.update(newData);
    rub.update(newData);
  }
};

}
