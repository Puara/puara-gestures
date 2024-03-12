//****************************************************************************//
// Puara Gestures - Utilities (.h)                                            //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#ifndef PUARA_UTILS_H
#define PUARA_UTILS_H

#include <chrono>
#include <deque>

namespace puara_gestures::utils {

    /**
     *  Simple leaky integrator implementation
     */ 
    class LeakyIntegrator {
        public:
            double current_value;
            double old_value;
            double leak;
            int frequency; // leaking frequency (Hz)
            unsigned long long timer;

            LeakyIntegrator(
                double currentValue = 0, 
                double oldValue = 0, 
                double leakValue = 0.5, 
                int freq = 100, 
                unsigned long long timerValue = 0
                ) : current_value(currentValue), old_value(oldValue), leak(leakValue), 
                    frequency(freq), timer(timerValue) {}

            double integrate(double reading);
            double integrate(double reading, double leak);
    };

    /**
     *  Simple class to renge values according to min and max (in and out)
     *  established values
     */ 
    class MapRange {
        public:
            double current_in = 0;
            double inMin = 0;
            double inMax = 0;
            double outMin = 0;
            double outMax = 0;
            double range (double in);
            float range (float in);
            int range (int in);
    };

    /**
     *  Simple circular buffer
     *  This was created to ensure compatibility with older ESP SoCs
     */ 
    class CircularBuffer {
        public:
            int size = 10;
            std::deque<double> buffer;
            double add(double element);
    };

    /**
     *  Simple function to get the current elapsed time in microseconds
     */ 
    long long getCurrentTimeMicroseconds();

    /**
     * Function used to reduce feature arrays into single values
     * 
     * E.g., brush uses it to reduce multiBrush instances
     */
    double arrayAverageZero (double * Array, int ArraySize);

    /**
     * Legacy function used to calculate 1D blob detection in older 
     * digital musical instruments
     */ 
    void bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift);
}

#endif
