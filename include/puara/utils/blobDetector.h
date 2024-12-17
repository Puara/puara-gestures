#pragma once

#include <boost/container/small_vector.hpp>

namespace puara_gestures
{

/**
 * @struct BlobDetector
 * @brief A structure for detecting contiguous regions (blobs) of `1`s in binary arrays.
 *
 * The `BlobDetector` identifies contiguous blobs in a binary input array where elements are either
 * `0` or `1`.
 * For each blob, it computes:
 * - The start position (`blobStartPos`)
 * - The size in terms of consecutive `1`s (`blobSize`)
 * - The center index (`blobCenter`)
 * - Movement compared to the previous detection (`lastState_blobPos`)
 * @warning Most of these values are reset when a detection function, e.g., detect1D(), is called,
 * and calculated during the detection funtion calls -- so their value is only really valid right
 * after such a call. There is no locking mechanism on any of these values so it is on the client
 * to ensure there is no race conditions.
 *
 * @details
 * The struct maintains internal arrays to store information about detected blobs, including their
 * start positions, sizes, and centers. It also tracks blob positions from the previous invocation
 * of the detection function to compute movement values.
 *
 * ## Key Functionalities:
 * - **Blob Detection:** Detects contiguous regions of `1`s in a binary input array.
 * - **Movement Calculation:** Computes the positional changes (movement) of blobs compared to
 *   their last positions.
 * - **Data Management:** Maintains internal state between function calls to facilitate movement
 *   tracking.
 *
 * @note
 * - The number of blobs processed is limited by `maxNumBlobs` (default is `4`).
 * - If the input contains more blobs than `maxNumBlobs`, the additional blobs are ignored.
 */
struct BlobDetector
{
  /** The maximum number of blobs that the algorithm should detect. */
  static constexpr int maxNumBlobs = 4;

  /** The start index of detected blobs. */
  int blobStartPos[maxNumBlobs]{};

  /** The cached start index of detected blobs, from the previous time detect1D
    * was called.
    */
  int lastState_blobPos[maxNumBlobs]{};

  /** size (amount of stripes) of each blob */
  int blobSize[maxNumBlobs]{};

  /** shows the "center"(index)of each blob */
  float blobCenter[maxNumBlobs]{};

  /** amount of detected blobs */
  int blobAmount{};

  /**
    * @brief Detects contiguous regions (blobs) of `1`s in a 1D binary array and computes their movement.
    *
    * This function identifies blobs in the input binary array `touchArray`, calculates their start
    * positions, sizes, and centers, and returns the movement of the blobs compared to their positions
    * from the previous function call.
    *
    * @param touchArray Pointer to the 1D binary array representing touch data. Each element is expected to be 0 or 1.
    * @param touchArraySize The size of the `touchArray`, representing the number of touch sensor in the array.
    *        This is expected to be larger than maxNumBlobs, which by definition will be a portion of the number
    *        of sensors, or at most equal to the number of sensors.
    * @return A small_vector of integers representing the movement of each blob's start position since the
    *         last invocation of `detect1D`.
    *
    * @note
    * - The function updates the global variables `blobStartPos`, `blobSize`, `blobCenter`,
    *   and `lastState_blobPos`.
    * - The number of blobs detected is limited by `maxNumBlobs`.
    * - If the number of blobs exceeds `maxNumBlobs`, additional blobs are ignored.
    *
    * @warning
    * - Ensure that `touchArray` has at least `size` elements to avoid out-of-bounds access.
    * - The function relies on external global variables (`blobStartPos`, `blobSize`, `blobCenter`,
    *   `lastState_blobPos`, `maxNumBlobs`, `blobAmount`). Ensure they are initialized appropriately
    *   before calling the function.
    */
  boost::container::small_vector<int, maxNumBlobs>
  detect1D(const int* const touchArray, const int touchArraySize)
  {
    blobAmount = 0;
    for(int i = 0; i < maxNumBlobs; i++)
    {
      //cache the last blobStartPos before clearing it
      lastState_blobPos[i] = blobStartPos[i];
      blobStartPos[i] = 0;
      blobSize[i] = 0;
      blobCenter[i] = 0;
    }

    for(int stripe = 0; stripe < touchArraySize;)
    {
      if(touchArray[stripe] == 1)
      {
        //start the blob
        blobStartPos[blobAmount] = stripe;

        //continue the blob until we no longer have 1s
        int sizeCounter = 1;
        while((stripe + sizeCounter) <= touchArraySize
              && touchArray[stripe + sizeCounter] == 1)
        {
          sizeCounter++;
        }

        blobSize[blobAmount] = sizeCounter;
        blobCenter[blobAmount] = stripe + (sizeCounter - 1.0) / 2.0;
        stripe += sizeCounter;

        if(++blobAmount >= maxNumBlobs)
          break;
      }
      else
      {
        ++stripe;
      }
    }

    //return the movement since the last time detect1D was called
    boost::container::small_vector<int, maxNumBlobs> movement(maxNumBlobs, 0);
    for(int i = 0; i < maxNumBlobs; ++i)
      movement[i] = blobStartPos[i] - lastState_blobPos[i];

    return movement;
  }
};
}
