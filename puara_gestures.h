//****************************************************************************//
// Puara Gestures - Sensor fusion and  gestural descriptors algorithms        //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#ifndef PUARA_GESTURES_H
#define PUARA_GESTURES_H

#include <deque>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <vector>

// puara-gesture descriptors
#include "descriptors/puara-utils.h"
#include "descriptors/puara-jab.h"
#include "descriptors/puara-shake.h"
#include "descriptors/IMU_Sensor_Fusion/imu_orientation.h"


namespace puara_gestures {

    struct Coord1D {
        double X;
    };

    struct Coord2D {
        double X, Y;
    };

    struct Coord3D {
        double X, Y, Z;
    };

    struct Imu6Axis {
        struct accl {double X, Y, Z;};
        struct gyro {double X, Y, Z;};
    };

    struct Imu9Axis {
        struct accl {double X, Y, Z;};
        struct gyro {double X, Y, Z;};
        struct magn {double X, Y, Z;};
    };

    struct DiscreteArray {
        std::vector<int> arr;
        DiscreteArray(int n) : arr(std::vector<int>(n)) {}
    };

}

#endif
