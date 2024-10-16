#pragma once

#include <cmath>

#include <algorithm>

namespace puara_gestures
{

class Touch
{
public:
  void updateTouchArray(int* discrete_touch, int touchSize);
  float touchAll = 0;    // f, 0--1
  float touchTop = 0;    // f, 0--1
  float touchMiddle = 0; // f, 0--1
  float touchBottom = 0; // f, 0--1
  float brush = 0;       // f, 0--? (~cm/s)
  float multiBrush[4]; // ffff, 0--? (~cm/s)
  float rub;           // f, 0--? (~cm/s)
  float multiRub[4];   // ffff, 0--? (~cm/s)

  // touch array
  int touchSizeEdge
      = 4; // amount of touch stripes for top and bottom portions (arbitrary)
  float touchAverage(float* touchArrayStrips, int firstStrip, int lastStrip);
  float touchAverage(int* touchArrayStrips, int firstStrip, int lastStrip);
  int lastState_blobPos[4];
  int maxBlobs = 4;  // max amount of blobs to be detected
  int blobAmount;    // amount of detected blobs
  int blobCenter[4]; // shows the "center" (index) of each blob (former blobArray)
  int blobPos[4];    // starting position (index) of each blob
  float blobSize[4]; // "size" (amount of stripes) of each blob
  void blobDetection1D(int* discrete_touch, int touchSize);
  const int leakyBrushFreq = 100; // leaking frequency (Hz)
  unsigned long leakyBrushTimer = 0;
  const int leakyRubFreq = 100;
  unsigned long leakyRubTimer = 0;
  int brushCounter[4];
  float arrayAverageZero(float* Array, int ArraySize);
  void bitShiftArrayL(int* origArray, int* shiftedArray, int arraySize, int shift);

  float leakyIntegrator(
      float reading, float old_value, float leak, int frequency, unsigned long& timer);

  /* Expects an array of discrete touch values (int, 0 or 1) and
   * the size of the array
   */
  void updateTouchArray(int* discrete_touch, int touchSize)
  { // raw_touch

    // touchAll: get the "amount of touch" for the entire touch sensor
    // normalized between 0 and 1
    touchAll = touchAverage(discrete_touch, 0, touchSize);

    // touchTop: get the "amount of touch" for the top part of the capsense
    // normalized between 0 and 1
    touchTop = touchAverage(discrete_touch, 0, touchSizeEdge);

    // touchMiddle: get the "amount of touch" for the central part of the capsense
    // normalized between 0 and 1
    touchMiddle
        = touchAverage(discrete_touch, (0 + touchSizeEdge), (touchSize - touchSizeEdge));

    // touchBottom: get the "amount of touch" for the botton part of the capsense
    // normalized between 0 and 1
    touchBottom = touchAverage(discrete_touch, (touchSize - touchSizeEdge), touchSize);

    // Save last blob detection state before reading new data
    for(int i = 0; i < (sizeof(blobPos) / sizeof(blobPos[0])); ++i)
    {
      lastState_blobPos[i] = blobPos[i];
    }

    // 1D blob detection: used for brush
    blobDetection1D(discrete_touch, touchSize);

    // brush: direction and intensity of capsense brush motion
    // rub: intensity of rub motion
    // in ~cm/s (distance between stripes = ~1.5cm)
    for(int i = 0; i < (sizeof(blobPos) / sizeof(blobPos[0])); ++i)
    {
      float movement = blobPos[i] - lastState_blobPos[i];
      if(blobPos[i] == -1)
      {
        multiBrush[i] = 0;
        multiRub[i] = 0;
        brushCounter[i] = 0;
      }
      else if(movement == 0)
      {
        if(brushCounter[i] < 10)
        {
          brushCounter[i]++;
          // wait some time before dropping the rub/brush values
        }
        else if(multiBrush[i] < 0.001)
        {
          multiBrush[i] = 0;
          multiRub[i] = 0;
        }
        else
        {
          multiBrush[i] = leakyIntegrator(
              movement * 0.15, multiBrush[i], 0.7, leakyBrushFreq, leakyBrushTimer);
          multiRub[i] = leakyIntegrator(
              abs(movement * 0.15), multiRub[i], 0.7, leakyRubFreq, leakyRubTimer);
        }
      }
      else if(abs(movement) > 1)
      {
        multiBrush[i]
            = leakyIntegrator(0, multiBrush[i], 0.6, leakyBrushFreq, leakyBrushTimer);
      }
      else
      {
        multiBrush[i] = leakyIntegrator(
            movement * 0.15, multiBrush[i], 0.8, leakyBrushFreq, leakyBrushTimer);
        multiRub[i] = leakyIntegrator(
            abs(movement) * 0.15, multiRub[i], 0.99, leakyRubFreq, leakyRubTimer);
        brushCounter[i] = 0;
      }
    }
    brush = arrayAverageZero(multiBrush, 4);
    rub = arrayAverageZero(multiRub, 4);
  }

  float touchAverage(float* touchArrayStrips, int firstStrip, int lastStrip)
  {
    int sum = 0;
    for(int i = firstStrip; i < lastStrip - 1; ++i)
      sum += touchArrayStrips[i];

    return ((float)sum) / (lastStrip - firstStrip);
  }

  float touchAverage(int* touchArrayStrips, int firstStrip, int lastStrip)
  {
    int sum = 0;
    for(int i = firstStrip; i < lastStrip; i++)
      sum += (float)touchArrayStrips[i];

    return ((float)sum) / (lastStrip - firstStrip);
  }

  void blobDetection1D(int* discrete_touch, int touchSize)
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
