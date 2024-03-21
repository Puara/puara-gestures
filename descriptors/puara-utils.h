//****************************************************************************//
// Puara Gestures - Utilities (.h)                                            //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#ifndef PUARA_UTILS_H
#define PUARA_UTILS_H

#include "../puara_gestures.h"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <deque>

namespace puara_gestures::utils {

    /**
     *  @brief Simple leaky integrator implementation.
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
            double integrate(double reading, double leak, long long time);
    };

    /**
     *  @brief Simple class to renge values according to min and max (in and out)
     *  established values.
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
     *  Simple circular buffer. 
     *  This was created to ensure compatibility with older ESP SoCs
     */ 
    class CircularBuffer {
        public:
            int size = 10;
            std::deque<double> buffer;
            double add(double element);
    };

    template <typename T>
    /**
     * @brief Calculates the minimum and maximum values of the last N updates. 
     * The default N value is 10, modifiable during initialization.
     * Ported from https://github.com/celtera/avendish/blob/56b89e52e367c67213be0c313d2ed3b9fb1aac19/examples/Ports/Puara/Jab.hpp#L15
     */
    class RollingMinMax {
        public:
            RollingMinMax(size_t buffer_size=10) : buf(buffer_size) {}

            puara_gestures::MinMax<T> current_value;
            puara_gestures::MinMax<T> update(T value);
        private:
            boost::circular_buffer<T> buf;
    };

    /**
     *  @brief Simple function to get the current elapsed time in microseconds.
     */ 
    long long getCurrentTimeMicroseconds();

    /**
     * @brief Function used to reduce feature arrays into single values. 
     * E.g., brush uses it to reduce multiBrush instances
     */
    double arrayAverageZero (double * Array, int ArraySize);

    /**
     * @brief Legacy function used to calculate 1D blob detection in older 
     * digital musical instruments
     */ 
    void bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift);
}

#endif
