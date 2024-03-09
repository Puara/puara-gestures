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

#include "IMU_Sensor_Fusion/imu_orientation.h"

namespace puara_gestures {

    struct Value {
        double value;
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

    DynamicArray(int n) : arr(std::vector<int>(n)) {}

    // No need for a destructor as std::vector handles memory management
};

    class SecondClass {
        private:
            const FirstClass& firstInstance;
            const ThirdClass& thirdInstance;

        public:
            SecondClass(const FirstClass& first, const ThirdClass& third) : firstInstance(first), thirdInstance(third) {}

            void printFirstValue() {
                std::cout << "Value in FirstClass: " << firstInstance.value << std::endl;
                std::cout << "Values in ThirdClass: " << thirdInstance.value1 << ", " << thirdInstance.value2 << std::endl;
            }
        };

}

#endif
