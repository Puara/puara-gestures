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
     * The frequency for the leaky integrator can be defined during
     * instantiation, and can be changed through integrator.frequency
     */
    class Shake {
        public:
            Shake() : tied_value(nullptr) {}
            Shake(int frequency = 10) : integrator(0, 0, 0.6, frequency, 0), tied_value(nullptr) {}
            Shake(Value* tied) : tied_value(tied) {}
            
            utils::LeakyIntegrator integrator;
            double fast_leak = 0.6;
            double slow_leak = 0.3;
            double update(double reading);
            int  update(Value reading);
            int  update();
            double frequency();
            double frequency(double freq);
            double current_value();
            int tie(Value* new_tie);
        private:
            utils::CircularBuffer buffer;
            Value* tied_value;
    };

}

#endif
