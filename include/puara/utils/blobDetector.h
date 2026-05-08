/**
* @file blobDetector.h
* @brief Detects contiguous runs of `1` values in a 1D binary array.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
* @author Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org
* @author Edu Meneses (2024) - https://www.edumeneses.com
*/
#pragma once

namespace puara_gestures
{

/**
 * @class BlobDetector
 * @brief Detects contiguous runs of `1` values in a 1D binary array.
 *
 * @details 
 * This class records the start index, length, and center index for each
 * contiguous group of `1` values, up to `maxNumBlobs` results.
 * Additional blobs beyond the limit are ignored.
 * 
 * Example:
 * @code{.cpp}
 * puara_gestures::BlobDetector<4> detector;
 * int data[] = {1, 1, 0, 1, 1, 1, 0};
 * detector.detect1D(data, 7);
 *
 * // detector.blobCount == 2
 * // detector.blobStartPos[0] == 0
 * // detector.blobSize[0] == 2
 * // detector.blobStartPos[1] == 3
 * // detector.blobSize[1] == 3
 * @endcode
 *
 * @tparam maxNumBlobs Maximum number of blobs the detector will store.
 */
template <int maxNumBlobs>
class BlobDetector
{
public:
  /**
   * @brief The start index of detected blobs.
   *
   * This stores the first array index for each detected blob.
   */
  int blobStartPos[maxNumBlobs]{};

  /**
   * @brief Start index of blobs from the previous detect1D call.
   */
  int prevBlobStartPos[maxNumBlobs]{};

  /**
   * @brief Size of each detected blob.
   *
   * This is the count of consecutive `1` values in the blob.
   */
  int blobSize[maxNumBlobs]{};

  /**
   * @brief Center index of each detected blob.
   *
   * This is computed as the midpoint of the blob.
   */
  float blobCenter[maxNumBlobs]{};

  /**
   * @brief Number of detected blobs.
   */
  int blobCount{};

  /**
   * @brief Detect contiguous regions (blobs) of `1`s in a 1D binary array.
   *
   * This function identifies blobs in the input binary array `touchArray` and
   * calculates their start positions, sizes, and center indices.
   *
   * @param touchArray Pointer to the 1D binary array representing touch data.
   *        Each element should be 0 or 1.
   * @param touchArraySize Number of elements in the touch array.
   * @note Additional blobs beyond `maxNumBlobs` are ignored.
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
