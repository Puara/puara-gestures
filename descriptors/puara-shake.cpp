//****************************************************************************//
// Puara Gestures - Jab (.cpp)                                               //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//

#include "puara-shake.h"

namespace puara_gestures {

    double Shake::update(double reading) {

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

    int Shake::update(Value reading) {
        Shake::update(reading.value);
        return 1;
    }

    int Shake::update() {
        Shake::update(tied_value->value);
        return 1;
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

    int Shake::tie(Value* new_tie) {
        tied_value = new_tie;
    }

}
