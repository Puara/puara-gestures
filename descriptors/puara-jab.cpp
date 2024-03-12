//****************************************************************************//
// Puara Gestures - Jab (.cpp)                                               //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//

#include "puara-jab.h"

namespace puara_gestures {

    double Jab::update(double reading) {
        
        buffer.add(reading);
        std::deque<double>::iterator min = std::min_element(buffer.buffer.begin(), buffer.buffer.end());
        std::deque<double>::iterator max = std::max_element(buffer.buffer.begin(), buffer.buffer.end());

        if (*max-*min > threshold) {
            if (*max >= 0 && *min >= 0) {
                value = *max - *min;
            } else if (*max < 0 && *min < 0) {
                value = *min - *max;
            } else {
                value = *max - *min; // for X, this line was originally value = *min - *max;
            }
        }

        return value;
    }

    double Jab::current_value() {
        return value;
    }

}
