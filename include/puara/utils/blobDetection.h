
namespace puara_gestures
{

struct BlobDetection
{
  int maxBlobs = 4;    // max amount of blobs to be detected
  int blobAmount{};    // amount of detected blobs
  int blobCenter[4]{}; // shows the "center" (index) of each blob (former blobArray)
  float blobSize[4]{}; // "size" (amount of stripes) of each blob

  void blobDetection1D(int* discrete_touch, int touchSize, int* blobPos)
  {
    blobAmount = 0;
    int sizeCounter = 0;
    int stripe = 0;
    for(int i = 0; i < 4; i++)
    {
      blobCenter[i] = 0;
      blobPos[i] = 0;
      blobSize[i] = 0;
    }

    for(; stripe < touchSize; stripe++)
    {
      if(blobAmount < maxBlobs)
      {
        if(discrete_touch[stripe] == 1)
        { // check for beggining of blob...
          sizeCounter = 1;
          blobPos[blobAmount] = stripe;
          while(discrete_touch[stripe + sizeCounter] == 1)
          { // then keep checking for end
            sizeCounter++;
          }
          blobSize[blobAmount] = sizeCounter;
          blobCenter[blobAmount] = stripe + (sizeCounter / 2);
          stripe += sizeCounter + 1; // skip stripes already read
          blobAmount++;
        }
      }
    }
  }
};
}