#pragma once

#include <puara/utils/blobDetector.h>

#include "brushRub.h"

namespace puara_gestures
{
/**
 * @class TouchArrayGestureDetector
 * @brief Detects touch gestures on a 1D touch sensor array.
 *
 * This class computes average touch values for the entire array and for the top,
 * middle and bottom regions. It also detects contiguous touch blobs and tracks
 * simple brush/rub motion for each blob.
 *
 * Example usage:
 * @code{.cpp}
 * #include <puara/descriptors/touchArrayGestureDetector.h>
 *
 * constexpr int maxNumBlobs = 4;
 * constexpr int touchSizeEdge = 4;
 * constexpr int touchSize = 16;
 * int touchArray[touchSize] = {0};
 *
 * // Fill the touch array with 1 for touched stripes and 0 for untouched stripes.
 * touchArray[0] = 1;
 * touchArray[5] = 1;
 * touchArray[6] = 1;
 * touchArray[8] = 1;
 * touchArray[9] = 1;
 * touchArray[10] = 1;
 * touchArray[14] = 1;
 * touchArray[15] = 1;
 *
 * puara_gestures::TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> detector;
 * detector.update(touchArray, touchSize);
 *
 * float total = detector.totalTouchAverage;
 * float top = detector.topTouchAverage;
 * float middle = detector.middleTouchAverage;
 * float bottom = detector.bottomTouchAverage;
 *
 * // 'totalBrush' and 'totalRub' are updated automatically after each update().
 * float brush = detector.totalBrush;
 * float rub = detector.totalRub;
 * @endcode
 *
 * The detector expects a flat integer array of size `touchSize`, where each
 * element is either 1 (touch present) or 0 (no touch). After `update()` it
 * computes region averages and gesture motion values.
 *
 * @tparam maxNumBlobs The maximum number of touch blobs that can be detected.
 * @tparam touchSizeEdge The number of stripes reserved for the top and bottom regions.
 */
template <int maxNumBlobs, int touchSizeEdge>
class TouchArrayGestureDetector
{
public:
  //these four values are the average amount of touch for the entire touch array, as well as the top, middle and bottom parts.
  float totalTouchAverage{};  // f, 0--1
  float topTouchAverage{};    // f, 0--1
  float middleTouchAverage{}; // f, 0--1
  float bottomTouchAverage{}; // f, 0--1

  /** total amount of "brushing" gesture for all detected blobs on the touch array, in ~cm/s (distance between stripes = ~1.5cm) */
  float totalBrush{};

  /** total amount of "rubbing" gesture for all detected blobs on the touch array, in ~cm/s (distance between stripes = ~1.5cm) */
  float totalRub{};

  /** Expects an array of ints representing whether each discrete stripe in a touch array
   *  is currently touched (==1) or not (==0), as well as the size of the touch array.
   */
  void update(int* touchArray, int touchSize)
  {
    // Update the "amount of touch" for the entire touch sensor, as well as the top, middle and bottom parts.
    // All normalized between 0 and 1.
    totalTouchAverage = utils::arrayAverage(touchArray, 0, touchSize);
    topTouchAverage = utils::arrayAverage(touchArray, 0, touchSizeEdge);
    middleTouchAverage = utils::arrayAverage(touchArray, (0 + touchSizeEdge), (touchSize - touchSizeEdge));
    bottomTouchAverage = utils::arrayAverage(touchArray, (touchSize - touchSizeEdge), touchSize);

    //detect blobs on the touch array
    blobDetector.detect1D(touchArray, touchSize);

    //based on the start position of detected blobs, update the brush and rub detector
    for(int i = 0; i < maxNumBlobs; ++i)
      brushRubDetector[i].update(blobDetector.blobStartPos[i]);

    //and finally update the total brush and rub values
    updateTotalBrushAndRub();
  }

private:
  BlobDetector<maxNumBlobs> blobDetector;
  BrushRubDetector brushRubDetector[maxNumBlobs];

  void updateTotalBrushAndRub()
  {
    // Calculate total brush and rub values
    double brushes[maxNumBlobs];
    double rubs[maxNumBlobs];
    for(int i = 0; i < maxNumBlobs; ++i)
    {
      brushes[i] = brushRubDetector[i].brush.value;
      rubs[i] = brushRubDetector[i].rub.value;
    }

    totalBrush = utils::arrayAverageWithoutZero(brushes, maxNumBlobs);
    totalRub = utils::arrayAverageWithoutZero(rubs, maxNumBlobs);
  }
};
}
