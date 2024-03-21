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

    int Shake::update(Coord1D reading) {
        Shake::update(reading.x);
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
        tied_value = &(new_tie->x);
        return 1;
    }

    int Shake2D::update(double readingX, double readingY) {
        x.update(readingX);
        y.update(readingY);
        return 1;
    }

    int Shake2D::update(Coord2D reading) {
        x.update(reading.x);
        y.update(reading.y);
        return 1;
    }

    int Shake2D::update() {
        x.update();
        y.update();
        return 1;
    }

    double Shake2D::frequency (double freq) {
        x.frequency(freq);
        y.frequency(freq);
        return freq;
    }

    Coord2D Shake2D::current_value() {
        Coord2D answer;
        answer.x = x.current_value();
        answer.y = y.current_value();
        return answer;
    }

    int Shake3D::update(double readingX, double readingY, double readingZ) {
        x.update(readingX);
        y.update(readingY);
        z.update(readingZ);
        return 1;
    }

    int Shake3D::update(Coord3D reading) {
        x.update(reading.x);
        y.update(reading.y);
        z.update(reading.z);
        return 1;
    }

    int Shake3D::update() {
        x.update();
        y.update();
        z.update();
        return 1;
    }

    double Shake3D::frequency (double freq) {
        x.frequency(freq);
        y.frequency(freq);
        z.frequency(freq);
        return freq;
    }

    Coord3D Shake3D::current_value() {
        Coord3D answer;
        answer.x = x.current_value();
        answer.y = y.current_value();
        answer.z = z.current_value();
        return answer;
    }
}
