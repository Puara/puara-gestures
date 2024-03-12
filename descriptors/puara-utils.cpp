//****************************************************************************//
// Puara Gestures - Utilities (.cpp)                                          //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//


#include "puara-utils.h"

namespace puara_gestures::utils {


    double LeakyIntegrator::integrate (double reading, double custom_leak) {
        double leakValue = custom_leak;
        if (frequency <= 0) {
            current_value = reading + (old_value * leakValue);
        } else if ((getCurrentTimeMicroseconds()/1000LL)  - (1000 / frequency) < timer) {  
            current_value = reading + old_value;
        } else {
            current_value = reading + (old_value * leakValue);
            timer = (getCurrentTimeMicroseconds()/1000LL);
        }
        return current_value;
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