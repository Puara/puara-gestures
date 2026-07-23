/**
* @file multitouch.h
* @brief Two-finger relational gestures: pinch/spread (scale) and twist (rotation) from two 2D points.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/


#pragma once

#include <puara/structs.h>
#include <puara/utils.h>

#include <cmath>

namespace puara_gestures
{
  /**
   * @class MultiTouch
   * @brief Relational two-finger gestures: pinch/spread, twist, and pan from
   * two touch positions.
   *
   * @details
   * Where @ref TouchArrayGestureDetector reports brush/rub features from a
   * sensor array, `MultiTouch` works from the *positions of two contacts* and
   * extracts the classic relational multi-touch gestures used on trackpads
   * and phones — pinch/spread (scale), two-finger twist (rotation), and the
   * moving midpoint (pan). These map naturally to expressive continuous
   * controls: scale to a filter cutoff or zoom, rotation to a parameter dial,
   * the centroid to an XY position.
   *
   * Feed it the two finger coordinates each frame:
   *   - `distance`      : current separation of the two points;
   *   - `center`        : their midpoint (pan / XY);
   *   - `angle`         : orientation of the line between them (radians);
   *   - `scale`         : `distance` relative to the reference (1.0 = same as
   *                       when the gesture was anchored; 2.0 = spread twice as
   *                       far apart; 0.5 = pinched to half);
   *   - `spread`        : signed change in distance from the reference;
   *   - `rotation`      : accumulated twist since the reference (radians,
   *                       unwrapped across the +/-pi boundary);
   *   - `distanceDelta` / `rotationDelta` : per-update change, i.e. pinch and
   *                       twist velocity.
   *
   * The first `update()` (or a call to `anchor()`) sets the reference that
   * `scale`/`spread`/`rotation` are measured against — call `anchor()` when a
   * new two-finger gesture begins (e.g. on second-finger-down). It keeps no
   * timing state and uses only doubles, so it behaves identically in an
   * Arduino `loop()`, a thread, or an ossia score process. Header-only, no
   * STL/Boost/allocation.
   *
   * Example usage with tied data:
   * @code
   * #include <puara/descriptors/multitouch.h>
   *
   * puara_gestures::Coord2D finger1{}, finger2{};
   * puara_gestures::MultiTouch touch(&finger1, &finger2);
   *
   * void loop() {
   *   finger1 = readTouch(0);
   *   finger2 = readTouch(1);
   *   touch.update();
   *   double zoom = touch.scale;      // pinch/spread factor
   *   double dial = touch.rotation;   // twist in radians
   * }
   * @endcode
   *
   * If you prefer not to use `tied_data`, call `touch.update(a, b)` directly.
   *
   * @ingroup puara_gestures_descriptors
   */
class MultiTouch
{
public:
  /**
   * @brief Default-construct a MultiTouch without tied input data.
   *
   * Use `update(a, b)` to provide the two touch points directly.
   */
  MultiTouch() noexcept = default;
  MultiTouch(const MultiTouch&) noexcept = default;
  MultiTouch(MultiTouch&&) noexcept = default;
  MultiTouch& operator=(const MultiTouch&) noexcept = default;
  MultiTouch& operator=(MultiTouch&&) noexcept = default;

  /**
   * @brief Construct tied to two external `Coord2D` touch points.
   * @param tiedA Pointer to the first touch position.
   * @param tiedB Pointer to the second touch position.
   */
  MultiTouch(Coord2D* tiedA, Coord2D* tiedB)
  : tied_a(tiedA)
  , tied_b(tiedB)
  {
  }

  // --- Outputs -------------------------------------------------------------

  /** @brief Current separation between the two points. */
  double distance = 0.0;
  /** @brief Midpoint of the two points (pan / XY). */
  Coord2D center{};
  /** @brief Orientation of the line between the points, radians. */
  double angle = 0.0;
  /** @brief Distance relative to the reference (1.0 = reference, 2.0 = twice as far). */
  double scale = 1.0;
  /** @brief Signed change in distance from the reference. */
  double spread = 0.0;
  /** @brief Accumulated twist since the reference, radians (unwrapped). */
  double rotation = 0.0;
  /** @brief Change in distance since the previous update (pinch velocity). */
  double distanceDelta = 0.0;
  /** @brief Change in angle since the previous update, radians (twist velocity). */
  double rotationDelta = 0.0;

  /**
   * @brief Update from two touch positions.
   * @param a First touch point.
   * @param b Second touch point.
   * @return The current separation `distance`.
   */
  double update(Coord2D a, Coord2D b)
  {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    distance = std::sqrt(dx * dx + dy * dy);
    center = Coord2D{(a.x + b.x) * 0.5, (a.y + b.y) * 0.5};
    angle = std::atan2(dy, dx);

    if(!anchored)
    {
      setAnchor();
    }
    else
    {
      distanceDelta = distance - lastDistance;
      rotationDelta = wrapPi(angle - lastAngle);
      rotation += rotationDelta;
      scale = (referenceDistance > epsilon) ? distance / referenceDistance : 1.0;
      spread = distance - referenceDistance;
    }
    lastDistance = distance;
    lastAngle = angle;
    return distance;
  }

  /**
   * @brief Update from the tied external touch points.
   * @return 1 when update succeeds; 0 if either tied input is missing.
   */
  int update()
  {
    if(tied_a != nullptr && tied_b != nullptr)
    {
      MultiTouch::update(*tied_a, *tied_b);
      return 1;
    }
    else
    {
      // should we assert here, it seems like an error to call update() without tied points?
      return 0;
    }
  }

  /**
   * @brief Anchor the reference for `scale`/`spread`/`rotation` to the current
   * geometry. Call this when a new two-finger gesture begins.
   */
  void anchor()
  {
    // Re-anchor to the most recent geometry.
    setAnchor();
  }

  /**
   * @brief Get the current pinch/spread scale.
   * @return `scale` (1.0 at the reference separation).
   */
  double current_value() const { return scale; }

  /**
   * @brief Connect to two external `Coord2D` sources.
   * @param a Pointer to the first touch position.
   * @param b Pointer to the second touch position.
   * @return 1 when the tie succeeds.
   */
  int tie(Coord2D* a, Coord2D* b)
  {
    tied_a = a;
    tied_b = b;
    return 1;
  }

  /** @brief Clear all state; the next update() re-anchors the reference. */
  void reset()
  {
    distance = 0.0;
    center = Coord2D{};
    angle = 0.0;
    scale = 1.0;
    spread = 0.0;
    rotation = 0.0;
    distanceDelta = 0.0;
    rotationDelta = 0.0;
    anchored = false;
  }

private:
  static constexpr double epsilon = 1e-9;

  /** @brief Wrap an angle difference to (-pi, pi]. */
  static double wrapPi(double a)
  {
    while(a > M_PI)
    {
      a -= 2.0 * M_PI;
    }
    while(a <= -M_PI)
    {
      a += 2.0 * M_PI;
    }
    return a;
  }

  /** @brief Set the reference geometry to the current values. */
  void setAnchor()
  {
    referenceDistance = distance;
    scale = 1.0;
    spread = 0.0;
    rotation = 0.0;
    distanceDelta = 0.0;
    rotationDelta = 0.0;
    lastDistance = distance;
    lastAngle = angle;
    anchored = true;
  }

  Coord2D* tied_a{};
  Coord2D* tied_b{};
  bool anchored = false;
  double referenceDistance = 0.0;
  double lastDistance = 0.0;
  double lastAngle = 0.0;
};
}
