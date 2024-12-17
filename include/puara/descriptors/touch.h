#pragma once

#include <puara/utils.h>
#include <puara/utils/blobDetector.h>
#include <puara/utils/leakyintegrator.h>

#include <cmath>

namespace puara_gestures
{

class Touch
{
public:
  static const int maxNumBlobs = BlobDetector::maxNumBlobs;
  float touchAll = 0.0f;    // f, 0--1
  float touchTop = 0.0f;    // f, 0--1
  float touchMiddle = 0.0f; // f, 0--1
  float touchBottom = 0.0f; // f, 0--1
  float brush = 0.0f;       // f, 0--? (~cm/s)
  double multiBrush[maxNumBlobs]{}; // ffff, 0--? (~cm/s)
  float rub{};              // f, 0--? (~cm/s)
  double multiRub[maxNumBlobs]{}; // ffff, 0--? (~cm/s)

  // touch array
  int touchSizeEdge = 4; // amount of touch stripes for top and bottom portions (arbitrary)

  BlobDetector blobDetector;
  int brushCounter[maxNumBlobs]{};

  // Arrays of LeakyIntegrator instances
  utils::LeakyIntegrator multiBrushIntegrator[maxNumBlobs];
  utils::LeakyIntegrator multiRubIntegrator[maxNumBlobs];

  Touch()
  {
    for(int i = 0; i < maxNumBlobs; ++i)
    {
      multiBrushIntegrator[i] = utils::LeakyIntegrator(0.0f, 0.0f, 0.7f, 100, 0);
      multiRubIntegrator[i] = utils::LeakyIntegrator(0.0f, 0.0f, 0.7f, 100, 0);
    }
  }

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

     // 1D blob detection: used for brush
    const auto movement = blobDetector.detect1D(discrete_touch, touchSize);

    // brush: direction and intensity of capsense brush motion
    // rub: intensity of rub motion
    // in ~cm/s (distance between stripes = ~1.5cm)
    for(int i = 0; i < maxNumBlobs; ++i)
    {
      if(movement[i] == 0)
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
          //   multiBrush[i] = multiBrushIntegrator[i].integrate(
          //       movement * 0.15, multiBrush[i], 0.7, leakyBrushFreq, leakyBrushTimer);

          //   multiRub[i] = multiRubIntegrator[i].integrate(
          //       (std::abs(movement * 0.15)), multiRub[i], 0.7, leakyRubFreq,
          //       leakyRubTimer);
          //
          multiBrush[i] = multiBrushIntegrator[i].integrate(movement[i] * 0.15);
          multiRub[i] = multiRubIntegrator[i].integrate(std::abs(movement[i] * 0.15));
        }
      }
      else if(std::abs(movement[i]) > 1)
      {
        // multiBrush[i] = multiBrushIntegrator[i].integrate(
        //     0, multiBrush[i], 0.6, leakyBrushFreq, leakyBrushTimer);

        multiBrush[i] = multiBrushIntegrator[i].integrate(0);
      }
      else
      {
        // multiBrush[i] = multiBrushIntegrator[i].integrate(
        //     movement * 0.15, multiBrush[i], 0.8, leakyBrushFreq, leakyBrushTimer);
        // multiRub[i] = multiRubIntegrator[i].integrate(
        //     (std::abs(movement * 0.15)) * 0.15, multiRub[i], 0.99, leakyRubFreq,
        //     leakyRubTimer);

        multiBrush[i] = multiBrushIntegrator[i].integrate(movement[i] * 0.15);
        multiRub[i] = multiRubIntegrator[i].integrate((std::abs(movement[i] * 0.15)));

        brushCounter[i] = 0;
      }
    }
    brush = utils::arrayAverageZero(multiBrush, maxNumBlobs);
    rub = utils::arrayAverageZero(multiRub, maxNumBlobs);
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
};
}
