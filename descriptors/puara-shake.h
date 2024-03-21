//****************************************************************************//
// Puara Gestures - Jab (.h)                                                  //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#ifndef PUARA_SHAKE_H
#define PUARA_SHAKE_H

#include "../puara_gestures.h"
#include "puara-utils.h"

#include <algorithm>


namespace puara_gestures {

    /**
     * This class creates jab gestures using 1DoF info
     * 
     * It expects 1 axis of a accelerometer, but can be used
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
            double update(double reading);
            int update(Coord1D reading);
            int update();
            double frequency();
            double frequency(double freq);
            double current_value();
            int tie(Coord1D* new_tie);
        private:
            double* tied_value;
    };

    /**
     * This class creates jab gestures using 2DoF info
     * 
     * It expects 2 axis of a accelerometer, but can be used
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
            int update(double readingX, double readingY);
            int update(Coord2D reading);
            int update();
            double frequency(double freq);
            Coord2D current_value();
    };

    /**
     * This class creates jab gestures using 3DoF info
     * 
     * It expects 3 axis of a accelerometer, but can be used
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
            int update(double readingX, double readingY, double readingZ);
            int update(Coord3D reading);
            int update();
            double frequency(double freq);
            Coord3D current_value();
    };

}

#endif
