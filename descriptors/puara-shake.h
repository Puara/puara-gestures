//********************************************************************************//
// Puara Gestures - Jab (.h)                                                      //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//


#ifndef PUARA_SHAKE_H
#define PUARA_SHAKE_H

#include "puara-utils.h"

#include <algorithm>


namespace puara_gestures {

    /**
     * This class creates jab gestures using 1DoF info
     * 
     * It expects 1 axis of a accelerometer in m/s^2, but can be used
     * with any double or float
     * 
     * The frequency for the leaky integrator can be changed through 
     * integrator.frequency
     */
    class Shake {
        public:
            Shake() : tied_value(nullptr), integrator(0, 0, 0.6, 10, 0) {}
            Shake(double* tied) : tied_value(tied), integrator(0, 0, 0.6, 10, 0) {}
            Shake(Coord1D* tied) : tied_value(&(tied->x)), integrator(0, 0, 0.6, 10, 0) {}
            
            utils::LeakyIntegrator integrator;
            double fast_leak = 0.6;
            double slow_leak = 0.3;
            double update(double reading) {
                if (tied_value != nullptr) {
                *tied_value = reading;
                }

                double abs_reading = std::abs(reading);
                
                if (abs_reading > 0.1) {
                    integrator.integrate(abs_reading/10, fast_leak);
                } else {
                    integrator.integrate(0.0, slow_leak);
                    if (integrator.current_value < 0.01) {
                        integrator.current_value = 0;
                    }
                }
                return integrator.current_value;
            }

            int update(Coord1D reading) {
                Shake::update(reading.x);
                return 1;
            }

            int update() {
                if (tied_value != nullptr) {
                Shake::update(*tied_value);
                return 1;
                } else {
                    return 0;
                }
            }

            double frequency() {
                return integrator.frequency;
            }

            double frequency(double freq) {
                integrator.frequency = freq;
                return freq;
            }

            double current_value() {
                return integrator.current_value;
            }

            int tie(Coord1D* new_tie) {
                tied_value = &(new_tie->x);
                return 1;
            }

        private:
            double* tied_value;
    };

    /**
     * This class creates jab gestures using 2DoF info
     * 
     * It expects 2 axis of a accelerometer in m/s^2, but can be used
     * with any double or float
     * 
     * The frequency for the leaky integrator can be changed through 
     * integrator.frequency individually or for all axes with 
     * frequency(double freq)
     */
    class Shake2D {
        public:
            Shake2D() {}
            Shake2D(Coord2D* tied) : x(&(tied->x)), y(&(tied->y)) {}

            Shake x;
            Shake y;
            int update(double readingX, double readingY) {
                x.update(readingX);
                y.update(readingY);
                return 1;
            }

            int update(Coord2D reading) {
                x.update(reading.x);
                y.update(reading.y);
                return 1;
            }

            int update() {
                x.update();
                y.update();
                return 1;
            }

            double frequency(double freq) {
                x.frequency(freq);
                y.frequency(freq);
                return freq;
            }

            Coord2D current_value() {
                Coord2D answer;
                answer.x = x.current_value();
                answer.y = y.current_value();
                return answer;
            }
    };

    /**
     * This class creates jab gestures using 3DoF info
     * 
     * It expects 3 axis of a accelerometer in m/s^2, but can be used
     * with any double or float
     * 
     * The frequency for the leaky integrator can be changed through 
     * integrator.frequency individually or for all axes with 
     * frequency(double freq)
     */
    class Shake3D {
        public:
            Shake3D() {}
            Shake3D(Coord3D* tied) : x(&(tied->x)), y(&(tied->y)), z(&(tied->z)) {}

            Shake x, y, z;
            int update(double readingX, double readingY, double readingZ) {
                x.update(readingX);
                y.update(readingY);
                z.update(readingZ);
                return 1;
            }

            int update(Coord3D reading) {
                x.update(reading.x);
                y.update(reading.y);
                z.update(reading.z);
                return 1;
            }

            int update() {
                x.update();
                y.update();
                z.update();
                return 1;
            }

            double frequency(double freq) {
                x.frequency(freq);
                y.frequency(freq);
                z.frequency(freq);
                return freq;
            }
            Coord3D current_value() {
                Coord3D answer;
                answer.x = x.current_value();
                answer.y = y.current_value();
                answer.z = z.current_value();
                return answer;
            }
    };

}

#endif
