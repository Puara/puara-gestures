#ifndef PUARA_STRUCTS_H
#define PUARA_STRUCTS_H

#include <vector>

namespace puara_gestures {

    struct Coord1D {
        double x;
    };

    struct Coord2D {
        double x, y;
    };

    struct Coord3D {
        double x, y, z;
    };

    struct Spherical {
        double azimuth; 
        double& phi = azimuth;
        double elevation;
        double& theta = elevation;
        double distance;
        double& r = distance;
    };

    struct Quaternion {
        double w, x, y, z;
    };

    struct Imu6Axis {
        Coord3D accl, gyro;
    };

    struct Imu9Axis {
        Coord3D accl, gyro, magn;
    };
    
    struct DiscreteArray {
        std::vector<int> arr;
        DiscreteArray(int n) : arr(std::vector<int>(n)) {}
    };

    template <typename T>
    struct MinMax {
        T min;
        T max;
    };

}

#endif