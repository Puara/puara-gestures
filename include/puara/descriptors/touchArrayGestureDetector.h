/**
* @file touchArrayGestureDetector.h
* @brief Detects brush and rub features from touch array data using leaky integration. 
* @see https://github.com/Puara/puara-gestures                                        
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca                      
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org 
* @author Edu Meneses (2024) - https://www.edumeneses.com                                
*/
#pragma once

#include <puara/utils/blobDetector.h>
#include "brushRub.h"

namespace puara_gestures
{
/**
 * @class TouchArrayGestureDetector
 * @brief Detects touch gestures on a 1D touch sensor array.
 *
 * @details This class computes average touch values for the entire array and 
 * for the top, middle and bottom regions. It also detects contiguous touch 
 * blobs and tracks simple brush/rub motion for each blob.
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
 * @details
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
  /**
   * @brief Average touch coverage over the entire touch array.
   *
   * Value is normalized between 0 and 1.
   */
  float totalTouchAverage{};

  /**
   * @brief Average touch coverage in the top region of the touch array.
   */
  float topTouchAverage{};

  /**
   * @brief Average touch coverage in the middle region of the touch array.
   */
  float middleTouchAverage{};

  /**
   * @brief Average touch coverage in the bottom region of the touch array.
   */
  float bottomTouchAverage{};

  /**
   * @brief Total amount of brush gesture motion across all detected blobs.
   *
   * Value is reported in approximate cm/s based on the array stripe spacing.
   */
  float totalBrush{};

  /**
   * @brief Total amount of rub gesture motion across all detected blobs.
   *
   * Value is reported in approximate cm/s based on the array stripe spacing.
   */
  float totalRub{};

  /**
   * @brief Update the touch array detector with a new touch sample.
   *
   * This method computes average touch values for the full array and each
   * region, detects contiguous blobs, and updates brush/rub motion values.
   *
   * @param touchArray Array of touch values where 1 means touched and 0 means not touched.
   * @param touchSize Number of elements in the touch array.
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

  /**
   * @brief Recalculate the aggregated brush and rub totals for active blobs.
   *
   * This helper ignores zero-valued blobs so that only active motion
   * contributes to the final totals.
   */
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