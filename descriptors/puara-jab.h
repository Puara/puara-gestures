//****************************************************************************//
// Puara Gestures - Jab (.h)                                                  //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#ifndef PUARA_JAB_H
#define PUARA_JAB_H

#include "puara-utils.h"

#include <deque>
#include <algorithm>


namespace puara_gestures {

    /**
     * This class creates jab gestures using 1DoF info
     * 
     * It expects 1 axis of a accelerometer, but can be used
     * with any double or float
     */
    class Jab {
        public:
            Jab(int threshold = 5) : threshold(threshold) {}
            int threshold;
            double value;
            double update(double reading);
            double current_value();
        private:
            utils::CircularBuffer buffer;
    };

}

#endif
