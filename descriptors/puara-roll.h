//********************************************************************************//
// Puara Gestures - Roll (.h)                                                     //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
// Maggie Needham (2024)                                                          //
//********************************************************************************//

#ifndef PUARA_ROLL_H
#define PUARA_ROLL_H

#include "puara-utils.h"
#include "puara-structs.h"
#include "IMU_Sensor_Fusion/imu_orientation.h"

#include <algorithm>
#include <list>
#include <cmath>

namespace puara_gestures {

    /**
     * @brief This class measures roll gestures using 3DoF info from an accelerometer,
     * gyroscope, and magnetometer
     */
    class Roll {
    public:

        IMU_Orientation orientation;
        utils::Unwrap unwrapper;
        utils::Smooth smoother;
        utils::Wrap wrapper;

        /**
         * @brief Default constructor for Roll
         *
         * @return Sets "unwrapper" object to undo the modulation effect around a
         * range of [- PI, PI] due to the output range of IMU Orientation "roll"
         * @return Sets "smoother" object to average the previous 50 objects
         * @return Sets "wrapper" object to limit values to a [0, 2 * PI] range
         */
        Roll()
            : unwrapper(- M_PI, M_PI),
              smoother(50),
              wrapper(0, 2 * M_PI) {}

        /**
         * @brief Constructor for Roll
         *
         * @param smoothValue number of previous values that "smoother" object averages
         */
        Roll(double smoothValue)
            : unwrapper(- M_PI, M_PI),
              smoother(smoothValue),
              wrapper(0, 2 * M_PI) {}

        /**
         * @brief Constructor for Roll
         *
         * @param wrapMin minimum value of desired range for "wrapper" object
         * @param wrapMax maximum value of desired range for "wrapper" object
         */
        Roll(double wrapMin, double wrapMax)
            : unwrapper(- M_PI, M_PI),
              smoother(50),
              wrapper(wrapMin, wrapMax) {}

        /**
         * @brief Constructor for Roll
         *
         * @param smoothValue number of previous values that "smoother" object averages
         * @param wrapMin minimum value of desired range for "wrapper" object
         * @param wrapMax maximum value of desired range for "wrapper" object
         */
        Roll(double smoothValue, double wrapMin, double wrapMax)
            : unwrapper(- M_PI, M_PI),
              smoother(smoothValue),
              wrapper(wrapMin, wrapMax) {}

        /**
         * @brief Calculates side-to-side measurement of roll
         *
         * @param accel Measured in G's
         * @param gyro Measured in degrees/sec
         * @param mag Measured in Gauss
         * @param period_sec Measured in seconds
         *
         * @return Output range of [- PI, PI]
         */
        double roll(Coord3D accel, Coord3D gyro, Coord3D mag, double period_sec) {
            orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
            orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period_sec);
            orientation.setMagnetometerValues(mag.x, mag.y, mag.z);
            orientation.update(0.01);
            return orientation.euler.roll;
        }

        /**
         * @brief Option to "unwrap" value so that consecutive rolls register as
         * increases or decreases depending on direction, rather than "wrapping"
         * around the given range
         */
        double unwrap(double reading) {
            return unwrapper.unwrap(reading);
        }

        /**
         * @brief Option to "smooth" value stream by taking the average of a given
         * number of previous values
         * Set to 50 in default Roll constructor
         */
        double smooth(double reading) {
            return smoother.smooth(reading);
        }

        /**
         * @brief Option to "wrap" values again to limit to a given range
         * Set to [0, 2 * PI] in default Roll constructor
         */
        double wrap(double reading) {
            return wrapper.wrap(reading);
        }

        /**
         * @brief Resets accumulated value, sets object as "empty"
         */
        void clear_unwrap() {
            unwrapper.clear();
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
