//********************************************************************************//
// Puara Gestures - Roll (.h)                                                     //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
// Maggie Needham (2024)                                                          //
//********************************************************************************//

#ifndef PUARA_TILT_H
#define PUARA_TILT_H

#include "puara-utils.h"
#include "puara-structs.h"
#include "IMU_Sensor_Fusion/imu_orientation.h"

#include <algorithm>
#include <list>
#include <cmath>

namespace puara_gestures {

    /**
     * @brief This class measures tilt gestures using 3DoF info from an accelerometer,
     * gyroscope, and magnetometer
     */
    class Tilt {
    public:
        IMU_Orientation orientation;
        utils::Smooth smoother;

        /**
         * @brief Default constructor for Tilt
         *
         * @return Sets "smoother" object to average the previous 50 objects
         */
        Tilt(): smoother(50) {}

        /**
         * @brief Constructor for Tilt
         *
         * @param smoothValue number of previous values that "smoother" object averages
         */
        Tilt(double smoothValue) : smoother(smoothValue) {}

        /**
         * @brief Calculates tilt (aka "pitch") measurement
         *
         * @param accel Measured in G's
         * @param gyro Measured in degrees/sec
         * @param mag Measured in Gauss
         * @param period_sec Measured in seconds
         *
         * @return Output range of [- PI /2, PI /2]
         */
        double tilt(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec) {
            orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
            orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period_sec);
            orientation.setMagnetometerValues(mag.x, mag.y, mag.z);
            orientation.update(0.01);
            return orientation.euler.tilt;
        }

        /**
         * @brief Option to "smooth" value stream by taking the average of a given
         * number of previous values
         * Set to 50 in default Tilt constructor
         */
        double smooth(double reading) {
            return smoother.smooth(reading);
        }

        /**
         * @brief Clears list of all previous inputs
         */
        void clear_smooth() {
            smoother.clear();
        }
    };
}

#endif
