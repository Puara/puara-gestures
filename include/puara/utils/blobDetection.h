#pragma once
namespace puara_gestures
{

class BlobDetection
{
public:
  static const int maxNumBlobs = 4; // max amount of blobs to be detected

#include <vector>

  std::vector<int> blobDetection1D(const int* const touchArray, const int size)
  {
    int blobCenter[maxNumBlobs]{}; // shows the "center" (index) of each blob
    float blobSize[maxNumBlobs]{}; // "size" (amount of stripes) of each blob
    int blobAmount{};              // amount of detected blobs
    int sizeCounter{};

    for(int i = 0; i < maxNumBlobs; i++)
    {
      //cache the last blobPos before clearing it
      lastState_blobPos[i] = blobPos[i];
      blobPos[i] = 0;
    }

    for(int stripe = 0; stripe < size; ++stripe)
    {
      if(touchArray[stripe] == 1)
      {
        sizeCounter = 1;
        blobPos[blobAmount] = stripe;

        while(touchArray[stripe + sizeCounter] == 1)
          sizeCounter++;

        blobSize[blobAmount] = sizeCounter;
        blobCenter[blobAmount] = stripe + (sizeCounter / 2);
        stripe += sizeCounter + 1;

        if(++blobAmount >= maxNumBlobs)
          break;
      }
    }

    //return the movement since the last time blobDetection1D was called
    std::vector<int> movement(blobAmount, 0);
    for(int i = 0; i < blobAmount; ++i)
      movement[i] = blobPos[i] - lastState_blobPos[i];

    return movement;
  }

private:
  int blobPos[maxNumBlobs]{};
  int lastState_blobPos[maxNumBlobs]{};
};
}
