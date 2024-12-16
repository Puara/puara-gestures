#pragma once
namespace puara_gestures
{

struct BlobDetector
{
  /** The maximum number of blobs that the algorithm should detect. */
  static const int maxNumBlobs = 4;

  /** The start index of detected blobs. */
  int blobStartPos[maxNumBlobs]{};

  /** The cached start index of detected blobs, from the previous time detect1D
    * was called.
    */
  int lastState_blobPos [maxNumBlobs]{};

  /** "size" (amount of stripes) of each blob */
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
    * @param size The size of the `touchArray`.
    * @return A vector of integers representing the movement of each blob's start position since the 
    *         last invocation of `detect1D`. The size of the returned vector is `maxNumBlobs`.
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
  std::vector<int> detect1D(const int* const touchArray, const int size)
  {

    for(int i = 0; i < maxNumBlobs; i++)
    {
      //cache the last blobStartPos before clearing it
      lastState_blobPos[i] = blobStartPos[i];
      blobStartPos[i] = 0;
    }

    for(int stripe = 0; stripe < size;)
    {
      if(touchArray[stripe] == 1)
      {
        //start the blob
        blobStartPos[blobAmount] = stripe;

        //continue the blob until we no longer have 1s
        int sizeCounter = 1;
        while(touchArray[stripe + sizeCounter] == 1 && (stripe + sizeCounter) <= size)
          sizeCounter++;

        blobSize[blobAmount] = sizeCounter;
        blobCenter[blobAmount] = stripe + (sizeCounter - 1.f) / 2.f;
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
    std::vector<int> movement(maxNumBlobs, 0);
    for(int i = 0; i < maxNumBlobs; ++i)
      movement[i] = blobStartPos[i] - lastState_blobPos[i];

    return movement;
  }
};
}
