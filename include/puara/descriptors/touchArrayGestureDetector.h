#pragma once

#include <puara/utils/blobDetector.h>

#include "brushRub.h"

namespace puara_gestures
{

// touchSizeEdge: number of stripes for top and bottom portions (arbitrary)
//maxNumBlobs: maximum number of "blobs" that the blob detector can detect at once on the touch array

/**
 * @class TouchArrayGestureDetector
 * @brief A class to detect gestures on a touch-array device.
 *
 * This class is designed to detect various gestures on a touch-array device by analyzing touch data.
 * It detects blobs (contiguous touched stripes in the touch array), and for each blob, it calculates
 * the amount of "brushing" and "rubbing" gestures using a BrushRubDetector.
 *
 * @tparam maxNumBlobs The maximum number of blobs that the blob detector can detect at once on the touch array.
 * @tparam touchSizeEdge The number of stripes for the top and bottom portions of the touch array.
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

    totalBrush = utils::arrayAverageZero(brushes, maxNumBlobs);
    totalRub = utils::arrayAverageZero(rubs, maxNumBlobs);
  }
};
}
