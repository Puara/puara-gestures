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

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <list>

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

            LeakyIntegrator(
                double currentValue = 0,
                double oldValue = 0,
                double leakValue = 0.5,
                int freq = 100,
                unsigned long long timerValue = 0
                ) : current_value(currentValue), old_value(oldValue), leak(leakValue),
                    frequency(freq), timer(timerValue) {}

            /**
             * @brief Call integrator
             *
             * @param reading new value to add into the integrator
             * @param custom_leak between 0 and 1
             * @param time in microseconds
             * @return double
             */
            double integrate(double reading, double custom_old_value, double custom_leak, int custom_frequency, unsigned long long& custom_timer){
                auto currentTimePoint = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint.time_since_epoch());
                unsigned long long current_time = duration.count();

                if (custom_frequency <= 0) {
                    current_value = reading + (custom_old_value * custom_leak);
                } else if ((current_time/1000LL)  - (1000 / frequency) < custom_timer) {
                    current_value = reading + old_value;
                } else {
                    current_value = reading + (custom_old_value * custom_leak);
                    timer = (current_time/1000LL);
                }
                return current_value;
            }


            double integrate(double reading, double leak, unsigned long long& time) {
                return LeakyIntegrator::integrate(reading, old_value, leak, frequency, time);
            }

            double integrate(double reading, double custom_leak) {
                return LeakyIntegrator::integrate(reading, old_value, custom_leak, frequency, timer);
            }

            double integrate(double reading) {
                return LeakyIntegrator::integrate(reading, old_value, leak, frequency, timer);
            }


    };

    /**
     * @brief Undoes the modulating effects of a spherical angle representation
     */
    class Unwrap {
        public:
            double prev_angle;
            double accum;
            double min;
            double max;
            double range;

            Unwrap(
                double prevAngle = std::nan("0"),
                double accum = 0,
                double min = - M_PI,
                double max = M_PI
                ) : prev_angle(prevAngle), accum(accum), min(min),
                    max(max), range(max - min) {}

            double update(double reading) {
                // check if prev_angle is still at the initialized value;
                // this indicates that the current is the first value to be sent to unwrap
                if (isnan(prev_angle)) {
                    prev_angle = reading;
                }
                double diff = reading - prev_angle;
                prev_angle = reading;
                if (diff  > (range / 2)) {
                    accum -= 1;
                }
                if (diff < (range * -1 / 2)) {
                    accum += 1;
                }
                return reading + accum * range;
            }

            /**
             * @brief resets the unwrap object
             */
            void clear() {
                accum = 0;
                prev_angle = std::nan("0");
            }
    };

    /**
     * @brief Wraps a value around a given minimum and maximum.
     * Algorithm from Jean-Michael Celerier. Authoring interactive media : a logical & temporal approach.
     * Computation and Language [cs.CL]. Université de Bordeaux, 2018. English. ffNNT : 2018BORD0037ff. fftel-01947309
     */
    class Wrap {
        public:
            double min;
            double max;

            Wrap(
                double Min = 0,
                double Max = 2 * M_PI
            ) : min(Min), max(Max) {}

            double update(double reading) {
                double diff  = std::fabs(max - min);
                if (min <= reading && reading <= max) {
                    return reading;
                }
                else if (reading >= max) {
                    return min + (reading - std::fmod(min, diff));
                }
                return max - (min - std::fmod(reading, diff));
            }
    };

    /**
     * @brief "Smoothing" algorithm takes the average of a given number of previous inputs
     */
    class Smooth {
        public:
            std::list<double> list;
            double size;

            Smooth(
                std::list<double> List = {},
                double Size = 50
                ) : list(List), size(Size) {}

            double update(double reading) {
                list.push_front(reading);
                // keep list at desired size
                while (list.size() > size) {
                    list.pop_back();
                }
                // find average of list
                double total = 0;
                for (double i : list){
                    total += i;
                }
                return (total / size);
            }

            void clear() {
                list.clear();
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
     * Finds time difference between now and when function was last called
     */
    // class TimeDifference {
    //     public:
    //         static long then;
    //         std::chrono::duration<double> elapsed;

    //         TimeDifference (
    //             std::chrono::time_point<std::chrono::steady_clock> Then =  std::chrono::steady_clock::now()
    //             ) : then(Then), now(Now){}

    //         // auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    //         double update() {
    //             long = std::chrono::steady_clock::now();
    //             elapsed = then - now;
    //             then = now;
    //             std::cout << "elapsed.count () = " << elapsed.count() << "\n";
    //             return elapsed.count();
    //         }
    // };


    /**
     *  @brief Simple function to get the current elapsed time in microseconds.
     */
    unsigned long long getCurrentTimeMicroseconds() {
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


    /**
     * @brief Function used to obtain the azimuth of a Spherical coordinate from a Cartesian 3D coordinate
     * This will generate a value from 0 to 3.14
     */
    double getAzimuth(Coord3D reading) {
        return acos(reading.z / (sqrt(pow(reading.x, 2) + pow(reading.y, 2) + pow(reading.z, 2) )));
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

    /**
     *  @brief Convert Cartesian to Polar coordinates
     */
    Coord2D car_to_pol (Coord2D reading) {
        Coord2D pol;
        pol.x = sqrt (pow(reading.x, 2) + pow(reading.y, 2));
        pol.y = atan(reading.y / reading.x );
        return pol;
    }

    /**
     *  @brief Convert Cartesian to Polar coordinates
     */
    Coord2D car_to_pol (double reading_x, double reading_y) {
        Coord2D pol;
        pol.x = sqrt (pow(reading_x, 2) + pow(reading_y, 2));
        pol.y = atan(reading_y / reading_x );
        return pol;
    }


    /**
     *  @brief Convert Cartesian to Spherical coordinates as in Max Patch
     */
    Spherical car_to_sphere (Coord3D reading) {
        Spherical sphere;
        Coord2D pol = car_to_pol(reading.y, reading.z);

        Coord2D pol2 = car_to_pol(pol.x, reading.x);

        sphere.distance = pol2.x;

        sphere.azimuth = pol.y;

        sphere.elevation = pol2.y;

        return sphere;
    }
}
}

#endif
