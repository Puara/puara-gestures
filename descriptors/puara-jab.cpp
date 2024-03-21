//****************************************************************************//
// Puara Gestures - Jab (.cpp)                                               //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//

#include "puara-jab.h"

namespace puara_gestures {

    double Jab::update(double reading) {
        
        minmax.update(reading);
        double min = minmax.current_value.min;
        double max = minmax.current_value.max;

        if (max - min > threshold) {
            if (max >= 0 && min >= 0) {
                value = max - min;
            } else if (max < 0 && min < 0) {
                value = min - max;
            } else {
                value = max - min; // for X, this line was originally min - max
            }
        }
        return value;
    }

    double Jab::current_value() {
        return value;
    }

    int Jab::update(Coord1D reading) {
        Jab::update(reading.x);
        return 1;
    }

    int Jab::update() {
        if (tied_value != nullptr) {
            Jab::update(*tied_value);
            return 1;
        } else {
            return 0;
        }
    }

    int Jab::tie(Coord1D* new_tie) {
        tied_value = &(new_tie->x);
        return 1;
    }

    int Jab2D::update(double readingX, double readingY) {
        x.update(readingX);
        y.update(readingY);
        return 1;
    }

    int Jab2D::update(Coord2D reading) {
        x.update(reading.x);
        y.update(reading.y);
        return 1;
    }

    int Jab2D::update() {
        x.update();
        y.update();
        return 1;
    }

    Coord2D Jab2D::current_value() {
        Coord2D answer;
        answer.x = x.current_value();
        answer.y = y.current_value();
        return answer;
    }

    int Jab3D::update(double readingX, double readingY, double readingZ) {
        x.update(readingX);
        y.update(readingY);
        z.update(readingZ);
        return 1;
    }

    int Jab3D::update(Coord3D reading) {
        x.update(reading.x);
        y.update(reading.y);
        z.update(reading.z);
        return 1;
    }

    int Jab3D::update() {
        x.update();
        y.update();
        z.update();
        return 1;
    }

    Coord3D Jab3D::current_value() {
        Coord3D answer;
        answer.x = x.current_value();
        answer.y = y.current_value();
        answer.z = z.current_value();
        return answer;
    }
}
