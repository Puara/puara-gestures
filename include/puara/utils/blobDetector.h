#pragma once

#include <boost/container/small_vector.hpp>

namespace puara_gestures
{

/**
 * @class BlobDetector
 * @brief A Class for detecting contiguous regions (blobs) of `1`s in binary arrays.
 *
 * The `BlobDetector` identifies contiguous blobs in a binary input array where elements are either
 * `0` or `1`.
 * For each blob, it computes:
 * - The start position (`blobStartPos`)
 * - The size in terms of consecutive `1`s (`blobSize`)
 * - The center index (`blobCenter`)
 *
 * @tparam maxNumBlobs The maximum number of blobs that the blob detector can detect at once.
 *
 * @warning Most of these values are reset when a detection function, e.g., detect1D(), is called,
 * and calculated during the detection funtion calls -- so their value is only really valid right
 * after such a call. There is no locking mechanism on any of these values so it is on the client
 * to ensure there is no race conditions.
 *
 * @note
 * - If the input contains more blobs than `maxNumBlobs`, the additional blobs are ignored.
 */
template <int maxNumBlobs>
class BlobDetector
{
public:
  /** The start index of detected blobs. */
  int blobStartPos[maxNumBlobs]{};

  /** The cached start index of detected blobs, from the previous time detect1D was called. */
  int prevBlobStartPos[maxNumBlobs]{};

  /** size (amount of stripes) of each blob */
  int blobSize[maxNumBlobs]{};

  /** shows the "center"(index)of each blob */
  float blobCenter[maxNumBlobs]{};

  /** number of detected blobs */
  int blobCount{};

  /**
    * @brief Detects contiguous regions (blobs) of `1`s in a 1D binary array.
    *
    * This function identifies blobs in the input binary array `touchArray` and calculates their start
    * positions, sizes, and centers.
    *
    * @param touchArray Pointer to the 1D binary array representing touch data. Each element is expected to be 0 or 1.
    *        An element that is `1` indicates a touch sensor is activated, while `0` indicates it is not.
    * @param touchArraySize The size of the `touchArray`, representing the number of touch sensor in the array.
    *        This is expected to be larger than maxNumBlobs, which by definition will be a portion of the number
    *        of sensors, or at most equal to the number of sensors.
    */
  void detect1D(const int* const touchArray, const int touchArraySize)
  {
    blobCount = 0;
    for(int i = 0; i < maxNumBlobs; i++)
    {
      //cache the last blobStartPos before clearing it
      prevBlobStartPos[i] = blobStartPos[i];
      blobStartPos[i] = 0;
      blobSize[i] = 0;
      blobCenter[i] = 0;
    }

    for(int stripe = 0; stripe < touchArraySize;)
    {
      //continue until we find a touched stripe
      if(touchArray[stripe] != 1)
      {
        ++stripe;
        continue;
      }

      //start the blob
      blobStartPos[blobCount] = stripe;

      //continue the blob until we no longer have 1s
      int sizeCounter = 1;
      while((stripe + sizeCounter) <= touchArraySize && touchArray[stripe + sizeCounter] == 1)
        ++sizeCounter;

      blobSize[blobCount] = sizeCounter;
      blobCenter[blobCount] = stripe + (sizeCounter - 1.0) / 2.0;
      stripe += sizeCounter;

      if(++blobCount >= maxNumBlobs)
        break;
    }
  }
};
}
