//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//


#ifndef PUARA_UTILS_H
#define PUARA_UTILS_H

#include "puara-structs.h"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <deque>
#include <math.h>

namespace puara_gestures {
    
namespace utils {

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

            /**
             * @brief Call integrator
             * 
             * @param reading new value to add into the integrator
             * @param custom_leak between 0 and 1
             * @param time in microseconds
             * @return double 
             */
            LeakyIntegrator(
                double currentValue = 0, 
                double oldValue = 0, 
                double leakValue = 0.5, 
                int freq = 100, 
                unsigned long long timerValue = 0
                ) : current_value(currentValue), old_value(oldValue), leak(leakValue), 
                    frequency(freq), timer(timerValue) {}

            double integrate(double reading, float custom_leak, unsigned long time) {
                double leakValue = custom_leak;
                if (frequency <= 0) {
                    current_value = reading + (old_value * leakValue);
                } else if ((time/1000LL)  - (1000 / frequency) < timer) {  
                    current_value = reading + old_value;
                } else {
                    current_value = reading + (old_value * leakValue);
                    timer = (time/1000LL);
                }
                return current_value;
            }

            double integrate(double reading, double leak) {
                return LeakyIntegrator::integrate (reading, leak, getCurrentTimeMicroseconds());
            }

            double integrate(double reading, double leak, long long time) {
                return LeakyIntegrator::integrate (reading, leak);
            }
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
            double range (double in) {
                current_in = in;
                if (outMin != outMax) {
                    return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
                } else {
                    return in;
                }
            }

            float range (float in) {
                double casted_in = static_cast<double>(in);
                return static_cast<float>(range(casted_in));
            }

            int range (int in) {
                double casted_in = static_cast<double>(in);
                return static_cast<int>(range(casted_in));
            }
    };

    /**
     *  Simple circular buffer. 
     *  This was created to ensure compatibility with older ESP SoCs
     */ 
    class CircularBuffer {
        public:
            int size = 10;
            std::deque<double> buffer;
            double add(double element) {
                buffer.push_front(element);
                if (buffer.size() > size) {
                    buffer.pop_back();
                }
                return element;
            }
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
            puara_gestures::MinMax<T> update(T value) {
                puara_gestures::MinMax<T> ret{.min = value, .max = value};
                buf.push_back(value);
                for(const T value : buf) {
                    if(value < ret.min) ret.min = value;
                    if(value > ret.max) ret.max = value;
                }
                current_value = ret;
                return ret;
            }

        private:
            boost::circular_buffer<T> buf;
    };

    /**
     *  @brief Simple function to get the current elapsed time in microseconds.
     */ 
    long long getCurrentTimeMicroseconds() {
        auto currentTimePoint = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint.time_since_epoch());
        return duration.count();
    }

    /**
     * @brief Function used to reduce feature arrays into single values. 
     * E.g., brush uses it to reduce multiBrush instances
     */
    double arrayAverageZero (double * Array, int ArraySize) {
        double sum = 0;
        int count = 0;
        double output = 0;
        for (int i = 0; i < ArraySize; ++i) {
            if (Array[i] != 0) {
                sum += Array[i];
                count++;
            }
        }
        if (count > 0) {
            output = sum / count; 
        }
        return output;
    }

    /**
     * @brief Legacy function used to calculate 1D blob detection in older 
     * digital musical instruments
     */ 
    void bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift) {
        for (int i=0; i < arraySize; ++i) {
            shiftedArray[i] = origArray[i];
        }
        for (int k=0; k < shift; ++k) {
            for (int i=0; i < arraySize; ++i) {
                if ( i == (arraySize-1)) {
                    shiftedArray[i] = (shiftedArray[i] << 1);
                }
                else {
                    shiftedArray[i] = (shiftedArray[i] << 1) | (shiftedArray[i+1] >> 7);
                }
            }
        }
    }
}

namespace convert {

    /**
     * @brief Convert g's to m/s^2
     * 
     */
    double g_to_ms2(double reading) {
        return reading * 9.80665;
    }

    /**
     * @brief Convert m/s^2 to g's
     * 
     */
    double ms2_to_g(double reading) {
        return reading / 9.80665;
    }

    /**
     * @brief Convert DPS to radians per second
     * 
     */
    double dps_to_rads(double reading) {
        return reading * M_PI / 180;
    }

    /**
     * @brief Convert radians per second to DPS
     * 
     */
    double rads_to_dps(double reading) {
        return reading * 180 / M_PI;
    }

    /**
     * @brief Convert Gauss to uTesla
     * 
     */
    double gauss_to_utesla(double reading) {
        return reading / 10000;
    }

    /**
     * @brief Convert uTesla to Gauss 
     * 
     */
    double utesla_to_gauss(double reading) {
        return reading * 10000;
    }

}
}

#endif
