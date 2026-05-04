#pragma once

#ifndef PUARA_GESTURES_BLOBDETECTOR_H
#define PUARA_GESTURES_BLOBDETECTOR_H

namespace puara_gestures
{

/**
 * @class BlobDetector
 * @brief Detects contiguous runs of `1` values in a 1D binary array.
 *
 * Example:
 *   puara_gestures::BlobDetector<4> detector;
 *   int data[] = {1, 1, 0, 1, 1, 1, 0};
 *   detector.detect1D(data, 7);
 *
 *   // detector.blobCount == 2
 *   // detector.blobStartPos[0] == 0
 *   // detector.blobSize[0] == 2
 *   // detector.blobStartPos[1] == 3
 *   // detector.blobSize[1] == 3
 *
 * This class records the start index, length, and center index for each
 * contiguous group of `1` values, up to `maxNumBlobs` results.
 * Additional blobs beyond the limit are ignored.
 *
 * @tparam maxNumBlobs Maximum number of blobs the detector will store.
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
      while((stripe + sizeCounter) < touchArraySize && touchArray[stripe + sizeCounter] == 1)
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
#endif // PUARA_GESTURES_BLOBDETECTOR_H
