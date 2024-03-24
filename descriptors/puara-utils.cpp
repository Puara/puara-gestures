//********************************************************************************//
// Puara Gestures - Utilities (.cpp)                                              //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//


#include "puara-utils.h"

namespace puara_gestures {
    
namespace utils {

    double LeakyIntegrator::integrate (double reading, double custom_leak, long long time) {
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

    double LeakyIntegrator::integrate (double reading, double custom_leak) {
        return LeakyIntegrator::integrate (reading, leak, getCurrentTimeMicroseconds());
    }

    double LeakyIntegrator::integrate (double reading) {
        return LeakyIntegrator::integrate (reading, leak);
    }

    double MapRange::range (double in) {
        current_in = in;
        if (outMin != outMax) {
            return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
        } else {
            return in;
        }
    }

    double MapRange::range (double in) {
        current_in = in;
        if (outMin != outMax) {
            return (in - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
        } else {
            return in;
        }
    }

    float MapRange::range (float in) {
        double casted_in = static_cast<double>(in);
        return static_cast<float>(range(casted_in));
    }

    int MapRange::range (int in) {
        double casted_in = static_cast<double>(in);
        return static_cast<int>(range(casted_in));
    }

    double CircularBuffer::add(double element) {
        buffer.push_front(element);
        if (buffer.size() > size) {
            buffer.pop_back();
        }
        return element;
    }

    template <typename T>
    puara_gestures::MinMax<T> RollingMinMax<T>::update(T value) {
        puara_gestures::MinMax<T> ret{.min = value, .max = value};
        buf.push_back(value);
        for(const T value : buf) {
            if(value < ret.min) ret.min = value;
            if(value > ret.max) ret.max = value;
        }
        current_value = ret;
        return ret;
    }

    long long getCurrentTimeMicroseconds() {
        auto currentTimePoint = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint.time_since_epoch());
        return duration.count();
    }

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

    double g_to_ms2(double reading) {
        return reading * 9.80665;
    }

    double ms2_to_g(double reading) {
        return reading / 9.80665;
    }

    double dps_to_rads(double reading) {
        return reading * M_PI / 180;
    }

    double rads_to_dps(double reading) {
        return reading * 180 / M_PI;
    }

    double gauss_to_utesla(double reading) {
        return reading / 10000;
    }

    double utesla_to_gauss(double reading) {
        return reading * 10000;
    }
}

}