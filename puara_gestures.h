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
#include "IMU_Sensor_Fusion/imu_orientation.h"

// Calibration parameter input structure
struct calibrationParameters {
    // Accelerometer Parameters
    float accel_zerog[3];

    // Gyroscope Parameters
    float gyro_zerorate[3];
    
    // Magnetometer Parameters
    float sx[3];
    float sy[3];
    float sz[3];
    float h[3];
};

class PuaraGestures {
    
    private:

        // Intertial measurements
        const int BUFFER_SIZE = 5;
        float accelX;
        float accelY;
        float accelZ;
        std::deque<float> gyroBuffers[3]; // Need buffer to compute shake/jab
        std::deque<float> acclBuffers[3]; // Need buffer to compute shake/jab
        float gyroX;
        float gyroY;
        float gyroZ;
        float magX;
        float magY;
        float magZ;
        // Shake and Jab
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
        void updateJabShake();
        void updateJabShakeAccl();
        // Orientation
        const float DECLINATION = -14.14; // Declination at Montreal on 2020-03-12
        IMU_Orientation orientation;
        void updateOrientation();

        // touch array
        int touchSizeEdge = 4;  // amount of touch stripes for top and bottom portions (arbitrary)
        float touchAverage (float * touchArrayStrips, int firstStrip, int lastStrip);
        float touchAverage (int * touchArrayStrips, int firstStrip, int lastStrip);
        int lastState_blobPos[4];
            int maxBlobs = 4;   // max amount of blobs to be detected
            int blobAmount;     // amount of detected blobs
            int blobCenter[4];  // shows the "center" (index) of each blob (former blobArray)
            int blobPos[4];     // starting position (index) of each blob
            float blobSize[4];  // "size" (amount of stripes) of each blob
        void blobDetection1D (int *discrete_touch, int touchSize);
        const int leakyBrushFreq = 100; // leaking frequency (Hz)
        unsigned long leakyBrushTimer = 0;
        const int leakyRubFreq = 100;
        unsigned long leakyRubTimer = 0;
        int brushCounter[4];
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
        unsigned int buttonThreshold = 1;

    public:
        int jabXThreshold = 5;
        int jabYThreshold = 5;
        int jabZThreshold = 5;
        float leakyIntegrator (float reading, float old_value, float leak, int frequency, unsigned long& timer);
        
        // Inertial measurement updates (accelerometer, gyroscope, magnetometer)
        /* IMPORTANT NOTE- axes signs should be updated following below:
        *
        * Top view
        *  ___________
        * |    Y      |
        * |    ^      |
        * |    |      |
        * |    o--->X |
        * |   Z^      |
        * |___________|
        * 
        */
        void setAccelerometerValues(float accelX, float accelY, float accelZ); // in m/sec^2
        void setGyroscopeValues(float gyroX, float gyroY, float gyroZ); // in degrees/sec
        void setMagnetometerValues(float magX, float magY, float magZ); // in Gauss
        void updateInertialGestures(); // Updates shake, jab and orientation

        // General inertial sensor signals
        float getAccelX();
        float getAccelY();
        float getAccelZ();
        float getGyroX();
        float getGyroY();
        float getGyroZ();
        float getMagX();
        float getMagY();
        float getMagZ();
        float getYaw();
        float getPitch();
        float getRoll();

        // Shake and Jab
        float getShakeX();
        float getShakeY();
        float getShakeZ();
        float getJabX();
        float getJabY();
        float getJabZ();

        // Orientation quaternion and euler values
        IMU_Orientation::Quaternion getOrientationQuaternion();
        IMU_Orientation::Euler getOrientationEuler();

        // Calibration Methods
        void calibrateMagnetometer(float magX, float magY, float magZ);
        void calibrateAccelerometer(float accelX, float accelY, float accelZ);
        void calibrateGyroscope(float gyroX, float gyroY, float gyroZ);
        void setCalibrationParameters(calibrationParameters calParams);

        // Magnetometer Calibration Variables
        float sx[3] = {0.333, 0.333, 0.333};
        float sy[3] = {0.333, 0.333, 0.333};
        float sz[3] = {0.333, 0.333, 0.333};
        float h[3] = {0,0,0};
        float magCal[3];

        // Accelerometer Calibration variables
        float accel_zerog[3] = {0,0,0};
        float accelCal[3];

        /// Gyroscope Calibration variables
        float gyro_zerorate[3] = {0,0,0};
        float gyroCal[3];

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
        void updateTrigButton(int buttonValue);
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
