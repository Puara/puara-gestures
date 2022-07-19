//****************************************************************************//
// Puara Module Manager -  high-level gestural descriptors                    //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022) - https://www.edumeneses.com                            //
//****************************************************************************//


#ifndef PUARA_GESTURES_H
#define PUARA_GESTURES_H

#include <deque>
#include <cmath>
#include <algorithm>
#include "esp_timer.h"

class PuaraGestures {
    
    private:

        // Shake and Jab
        int queueAmount = 5;          // # of values stored
        std::deque<float> gyroXArray; // store last values
        std::deque<float> gyroYArray;
        std::deque<float> gyroZArray;
        int arraycounter = 0;
        const int leakyShakeFreq = 10;
        unsigned long leakyShakeTimerX;
        unsigned long leakyShakeTimerY;
        unsigned long leakyShakeTimerZ;
        float shakeX;
        float shakeY;
        float shakeZ;
        float jabX;
        float jabY;
        float jabZ;
        int jabThreshold = 10;

        // touch array
        int touchSizeEdge = 4;  // amount of touch stripes for top and bottom portions (arbitrary)
        float touchAverage (float * touchArrayStrips, int firstStrip, int lastStrip);
        float touchAverage (int * touchArrayStrips, int firstStrip, int lastStrip);
        float touchAverage (int * touchArrayStrips, int firstStrip, int lastStrip);
        int lastState_blobPos[4];
            int maxBlobs = 4;   // max amount of blobs to be detected
            int blobAmount;     // amount of detected blobs
            int blobCenter[4];  // shows the "center" (index) of each blob (former blobArray)
            int blobPos[4];     // starting position (index) of each blob
            float blobSize[4];  // "size" (amount of stripes) of each blob
        void blobDetection1D (int *discrete_touch, int touchSize);
        float leakyIntegrator (float reading, float old_value, float leak, int frequency, unsigned long& timer);
        const int leakyBrushFreq = 100; // leaking frequency (Hz)
        unsigned long leakyBrushTimer = 0;
        const int leakyRubFreq = 100;
        unsigned long leakyRubTimer = 0;
        int brushCounter[4];
        const int leakyShakeFreq = 10;
        unsigned long leakyShakeTimer[3];
        float arrayAverageZero (float * Array, int ArraySize);
        void bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift);
    
        // button
        unsigned int buttonCount;
        unsigned int buttonCountInterval = 200;
        int buttonValue;
        bool buttonPress = false;
        unsigned int buttonTap;
        unsigned int buttonDtap;
        unsigned int buttonTtap;
        bool buttonHold;
        unsigned int buttonHoldInterval = 5000;
        unsigned int buttonFilterPeriod = 10;
        long buttonTimer;
        unsigned long buttonPressTime;
        unsigned int buttonThreshold = 870;

    public:
        float leakyIntegrator (float reading, float old_value, float leak, int frequency, unsigned long& timer);
        
        // Shake and Jab
        void updateJabShake (float gyroX, float gyroY, float gyroZ); // in radians per second
        float getShakeX();
        float getShakeY();
        float getShakeZ();
        float getJabX();
        float getJabY();
        float getJabZ();
        
        // touch array
        void updateTouchArray (int *discrete_touch, int touchSize);
        float touchAll;         // f, 0--1
        float touchTop;         // f, 0--1
        float touchMiddle;      // f, 0--1
        float touchBottom;      // f, 0--1
        float brush;            // f, 0--? (~cm/s)
        float multiBrush[4];    // ffff, 0--? (~cm/s)
        float rub;              // f, 0--? (~cm/s)
        float multiRub[4];      // ffff, 0--? (~cm/s)

        // button
        void updateButton(int buttonValue);
        unsigned int getButtonCount();
        bool getButtonTouch();
        unsigned int getButtonPressTime();
        unsigned int getButtonValue();
        unsigned int getButtonTap();
        unsigned int getButtonDTap();
        unsigned int getButtonTTap();
        bool getButtonHold();
        unsigned int getButtonHoldInterval();
        unsigned int setButtonHoldInterval(int value);
        unsigned int getButtonThreshold();
        unsigned int setButtonThreshold(int value);
};

#endif