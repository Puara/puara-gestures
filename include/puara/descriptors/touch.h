#pragma once

#include <puara/utils/blobDetector.h>

#include "touchFeatures.h"

namespace puara_gestures
{
class Touch1D
{
public:
  Brush brush;
  Rub rub;

  void update(int movement)
  {
    brush.update(movement);
    rub.update(movement);
  }
};

//======================================================================

class Touch2D
{
public:
  static constexpr int maxNumBlobs = BlobDetector::maxNumBlobs;
  static constexpr auto touchSizeEdge{4}; // amount of stripes for top and bottom portions (arbitrary)

  float touchAll{};    // f, 0--1
  float touchTop{};    // f, 0--1
  float touchMiddle{}; // f, 0--1
  float touchBottom{}; // f, 0--1

  /** brush: direction and intensity of capsense brush motion in ~cm/s (distance between stripes = ~1.5cm) */
  float brush{};

  /** rub: intensity of rub motion in ~cm/s (distance between stripes = ~1.5cm) */
  float rub{};

  /* Expects an array of discrete touch values (int, 0 or 1) and
   * the size of the array
   */
  void updateTouchArray(int* discrete_touch, int touchSize)
  {
    // Update the "amount of touch" for the entire touch sensor, as well as the top, middle and bottom parts.
    // All normalized between 0 and 1.
    touchAll = utils::arrayAverage(discrete_touch, 0, touchSize);
    touchTop = utils::arrayAverage(discrete_touch, 0, touchSizeEdge);
    touchMiddle = utils::arrayAverage(discrete_touch, (0 + touchSizeEdge), (touchSize - touchSizeEdge));
    touchBottom = utils::arrayAverage(discrete_touch, (touchSize - touchSizeEdge), touchSize);

    const auto blobMovements{blobDetector.detect1D(discrete_touch, touchSize)};
    for(int i = 0; i < maxNumBlobs; ++i)
      touches[i].update(blobMovements[i]);

    updateBrushAndRub();
  }

private:
  BlobDetector blobDetector;
  Touch1D touches[maxNumBlobs];

  void updateBrushAndRub()
  {
    // Calculate total brush and rub values
    double brushes[maxNumBlobs];
    double rubs[maxNumBlobs];
    for(int i = 0; i < maxNumBlobs; ++i)
    {
      brushes[i] = touches[i].brush.value;
      rubs[i] = touches[i].rub.value;
    }

    brush = utils::arrayAverageZero(brushes, maxNumBlobs);
    rub = utils::arrayAverageZero(rubs, maxNumBlobs);
  }
};
}
