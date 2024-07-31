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
            Roll(double smooth) : smoothsize(smooth) {}

            double smoothsize;
            utils::Unwrap unwrapper;
            std::list<double> smoother =  {};


            /**
             * undo modulating effect of turning coordinates into spherical form
             * useful for smoothing effect
             */
            double unwrap(double reading) {
                return unwrapper.unwrap(reading);
            }

            /**
             * Sets the "accum" value of unwrapper back to 0
             */
            void clear_unwrap() {
                unwrapper.accum = 0;
            }

            /**
             * takes the average of the last n inputs, where n is a size set in the constructor
             * optional, leads to smoother readings
             */
            double smooth(double reading) {
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
            void clear_smooth_list() {
                smoother.clear();
            }

            /**
             * "Fold" angle again
             */
            double fold(double reading, double min, double max) {
                if ( min <= reading && reading <= max ) {
                    return reading;
                } else {
                    double remainder = std::fmod(reading, (max - min));
                    return max - (min + std::fabs(remainder));
                }
            }

            /**
             * Process the angle for midi format (?) -- from "unwrapped"
             */
            double wrap(double reading) {
                double value = std::fmod(reading, 6.28);
                if (value < 0 ) {
                    value += 6.28;
                }
                return (value * 0.159171);
            }

            /**
             * sets the previous angle for unwrapper
             */
            void set_prev_angle(double reading) {
                unwrapper.prev_angle = reading;
            }

    };

}

#endif
