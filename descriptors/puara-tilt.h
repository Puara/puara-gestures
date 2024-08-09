//********************************************************************************//
// Puara Gestures - Roll (.h)                                                      //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//
#ifndef PUARA_TILT_H
#define PUARA_TILT_H

#include "puara-utils.h"
#include "puara-structs.h"
#include "IMU_Sensor_Fusion/imu_orientation.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <math.h>

namespace puara_gestures {

    /**
     * This class creates tilt gestures using 3DoF info
     * Requires info from accelerometer, gyroscope, and magnetometer
     */
    class Tilt {
    public:
        Tilt() {};

        IMU_Orientation orientation;
        utils::Unwrap unwrapper;
        utils::Smooth smoother;
        utils::Wrap wrapper;

        /**
         * Tilt/"pitch" measurement
         * Range: [- PI/2, PI/2]
         */
        double tilt(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec) {
            update(accel, gyro, mag, period_sec);
            return orientation.euler.tilt;
        }

        void update(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec) {
            orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
            orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period_sec);
            orientation.setMagnetometerValues(mag.x, mag.y, mag.z);
            orientation.update(0.01);
        }


        /**
         * @brief Option to "unwrap" value so that consecutive rolls register as increases or decreases,
         * depending on direction, rather than "wrapping" around a given range.
         */
        double unwrap(double reading) {
            return unwrapper.update(reading);
        }
        /**
         * @brief Option to "unwrap" value so that consecutive rolls register as increases or decreases,
         * depending on direction, rather than "wrapping" around a given range.
         * @param min minimum value of the inputs to unwrap function
         * @param max maximum value of the inputs to unwrap function
         */
        double unwrap(double reading, double min, double max) {
            unwrapper.min = (M_PI / - 2);
            unwrapper.max = (M_PI / 2);
            unwrapper.range = std::fabs(max - min);
            return unwrapper.update(reading);
        }
        /**
         * @brief Option to "smooth" value
         */
        double smooth(double reading) {
            return smoother.update(reading);
        }
        /**
         * @brief Option to "wrap" values again to limit to a [- PI, PI] range.
         */
        double wrap(double reading) {
            return wrapper.update(reading);
        }
        /**
         * @brief Option to "wrap" values again to limit to a [- PI, PI] range.
         * @param min minimum value of desired range
         * @param max maximum value of desired range
         */
        double wrap(double reading, double min, double max) {
            wrapper.min = max;
            wrapper.max = max;
            return wrapper.update(reading);
        }
        /**
         * @brief Resets "accum" and "prev_angle"
         */
        void clear_unwrap() {
            unwrapper.clear();
        }
        /**
         * @brief Clears list of all previous and current inputs
         */
        void clear_smooth() {
            smoother.clear();
        }
    };
}

#endif
