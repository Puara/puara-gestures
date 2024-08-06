//********************************************************************************//
// Puara Gestures - Roll (.h)                                                      //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//
#ifndef PUARA_ROLL_H
#define PUARA_ROLL_H

#include "puara-utils.h"
#include "puara-structs.h"
#include "IMU_Sensor_Fusion/imu_orientation.h"

#include <algorithm>
#include <iostream>
#include <list>


namespace puara_gestures {

    /**
     * This class creates roll gestures using 3DoF info
     *
     * It expects 3 axis of a accelerometer in m/s^2, but can be used
     * with any double or float
     *
     */
    class Roll {
        public:
            Roll(){};

            utils::Unwrap uw;
            IMU_Orientation orientation;
            std::list<double> smoother =  {};

            /**
             * Takes in accelerometer, gyroscope, magnometer values
             */
            double update(Coord3D accel, Coord3D gyro, Coord3D mag, double period) {

                orientation.setAccelerometerValues(accel.x, accel.y, accel.z);
                orientation.setGyroscopeDegreeValues(gyro.x, gyro.y, gyro.z, period);
                orientation.setMagnetometerValues(mag.x, mag.y, mag.z);

                orientation.update(0.01);

                return  orientation.euler.roll;
            }

            double unwrap(double reading) {
                return uw.update(reading);
            }

            /**
             * Resets "accum" and "prev_angle"
             */
            void clear_unwrap() {
                uw.clear();
            }

            /**
             * takes the average of the last n inputs, where n is a size set in the constructor
             * optional, leads to smoother readings
             */
            double smooth(double reading, double smoothsize) {
                // add current value to list
                smoother.push_front(reading);
                // keep list at desired size
                while(smoother.size() > smoothsize) {
                    smoother.pop_back();
                }
                // find average of list
                double total = 0;
                for (double i : smoother) {
                    total += i;
                }
                return (total / smoother.size());
            }

            /**
             * Clears list of all previous and current inputs
             */
            void clear_smooth() {
                smoother.clear();
            }

            /**
             *
             */
            double wrap(double reading, double min, double max) {
                double d = max - min;
                if (min <= reading && reading <= max) {
                    return reading;
                } else if (reading >= max ) {
                    return min +  (reading - std::fmod(min, d));
                }
                return max - (min - std::fmod(reading, d));
            }

    };

}

#endif
