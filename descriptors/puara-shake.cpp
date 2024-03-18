//****************************************************************************//
// Puara Gestures - Jab (.cpp)                                               //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//

#include "puara-shake.h"

namespace puara_gestures {

    double Shake::update(double reading) {

        if (tied_value != nullptr) {
            *tied_value = reading;
        }

        buffer.add(reading);

        double abs_reading = std::abs(buffer.buffer.front());
            
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

    int Shake::update(Coord1D reading) {
        Shake::update(reading.X);
        return 1;
    }

    int Shake::update() {
        if (tied_value != nullptr) {
            Shake::update(*tied_value);
            return 1;
        } else {
            return 0;
        }
    }

    double Shake::frequency () {
        return integrator.frequency;
    }

    double Shake::frequency (double freq) {
        integrator.frequency = freq;
        return freq;
    }

    double Shake::current_value() {
        return integrator.current_value;
    }

    int Shake::tie(Coord1D* new_tie) {
        tied_value = &(new_tie->X);
        return 1;
    }

    double Shake2D::frequency (double freq) {
        X.frequency(freq);
        Y.frequency(freq);
        return freq;
    }

}
