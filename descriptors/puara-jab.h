//********************************************************************************//
// Puara Gestures - Jab (.h)                                                      //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//


#ifndef PUARA_JAB_H
#define PUARA_JAB_H

#include "puara-utils.h"

#include <deque>
#include <algorithm>


namespace puara_gestures {

    /**
     * @brief This class creates jab gestures using 1DoF info.
     * It expects 1 axis of a accelerometer in m/s^2, but can be used
     * with any double or float.
     */
    class Jab {
        public:
            Jab() : tied_value(nullptr), threshold(5), minmax(10) {}
            Jab(double* tied) : tied_value(tied), threshold(5), minmax(10) {}
            Jab(Coord1D* tied) : tied_value(&(tied->x)), threshold(5), minmax(10) {}
            
            int threshold;
            double update(double reading);
            int update(Coord1D reading);
            int update();
            double current_value();
            int tie(Coord1D* new_tie);
        private:
            double value;
            puara_gestures::utils::RollingMinMax<double> minmax;
            double* tied_value;
    };

    /**
     * @brief This class creates jab gestures using 2DoF info.
     * It expects 2 axis of a accelerometer in m/s^2, but can be used
     * with any double or float.
     */
    class Jab2D {
        public:
            Jab2D() : x(), y() {}
            Jab2D(Coord2D* tied) : x(&(tied->x)), y(&(tied->y)) {}

            Jab x, y;
            int update(double readingX, double readingY);
            int update(Coord2D reading);
            int update();
            double frequency(double freq);
            Coord2D current_value();
    };

    /**
     * @brief This class creates jab gestures using 3DoF info.
     * It expects 3 axis of a accelerometer in m/s^2, but can be used
     * with any double or float.
     */
    class Jab3D {
        public:
            Jab3D() {}
            Jab3D(Coord3D* tied) : x(&(tied->x)), y(&(tied->y)), z(&(tied->z)) {}

            Jab x, y, z;
            int update(double readingX, double readingY, double readingZ);
            int update(Coord3D reading);
            int update();
            double frequency(double freq);
            Coord3D current_value();
    };
}

#endif
