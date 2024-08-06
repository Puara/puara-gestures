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

    void changeRawFile(std::string filepath, std::string s) {
        std::ifstream inputFile(filepath);
        std::string line;
        // keep track of lines
        int linecount = 0;
        // start a new file
        std::ofstream newfile;
        newfile.open(s);

        while (std::getline(inputFile, line, 'n')) {
            linecount += 1;
            std::string toWrite = linecount == 1 ? line + "\n" : line.substr(1) + "\n";
            newfile << toWrite;
        }
        newfile.close();
        inputFile.close();
    };

    Coord3D readinRaw(std::string line) {
        Coord3D cart;
        // separate out by spaces into three numbers, turn into doubles
        int first_space = line.find_first_of(" ");
        int second_space = line.substr(first_space +1).find_first_of(" ") + first_space + 1;
        double x = std::stod(line.substr(0, first_space));
        double y = std::stod(line.substr(first_space + 1, second_space));
        double z = std::stod(line.substr(second_space + 1, line.size()));
        // add three doubles to a Coord3D
        cart.x = x;
        cart.y = y;
        cart.z = z;
        // return Coord3D
        return cart;
    };


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
                double prevAngle = std::nan("0"), // set to unattainable value to be able to determine if angle is first
                double accum = 0,
                double min = - M_PI,
                double max = M_PI
                ) : prev_angle(prevAngle), accum(accum), min(min),
                    max(max), range(max - min) {}

            double update(double reading) {
                if (isnan(prev_angle)) {
                    // then we know this is the first angle sent to unwrapped
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

            void clear() {
                accum = 0;
                prev_angle = std::nan("0");
            }

    };

    /**
     * @brief Undoes the modulating effects of a spherical angle representation
     * Suitable for an angle measurement that, after reaching the maximum, decreases to the minimum
     * instead of switching directly to the minimum (a "folding" measurement).
     */
    class Unfold {
        public:
            std::list<double> rolls;
            double min;
            double max;
            double allowance;

            Unfold(
                double min = 0,
                double max = M_PI,
                double allowance = 0.055
                ) : min(min), max(max), allowance(allowance) {}

            /**
             * Evaluates the "unfolded" value -- takes the number of turns into account to generate non-modulated value
             */
            double unfold (double reading) {
                double unfolded;

                // if the list is empty (not passed over min or max), return value
                if (rolls.size() == 0) {
                    unfolded = reading;
                }

                // find if length of list is even in order to calculate new value
                bool isEven = rolls.size() % 2 == 0;

                // find the number of full rolls -- (0, 1) or (1, 0) pairs
                double fullRolls = isEven ? rolls.size() / 2 : (rolls.size() - 1) / 2 ;

                // if the first value is 0, the list just has (0, 1) pairs
                // decreases value
                if (rolls.front() == 0) {
                    double sum = fullRolls * -2 * max;
                    unfolded =  isEven ? sum + reading : sum - reading;
                }
                // if the first value is 1, the list just has (1, 0) pairs
                // increases value
                if (rolls.front() == 1) {
                    double sum = isEven ? fullRolls * 2 * max : (fullRolls + 1) * 2 * max;
                    unfolded = isEven ? sum + reading : sum - reading;
                }

                // update the list after finding value so that a value close to 0 or 1 just
                // changes the status of the list but isn't evaluated according to new list
                // if close to minimum
                if (reading < (min + allowance)) {
                    // add a 0 to indicate it passed min
                    rolls.push_back(0);
                }
                // if close to maximum
                if (reading > (max - allowance)) {
                    //add a 1 to indicate it passed max
                    rolls.push_back(1);
                }
                // if the device has passed over the min twice or the max twice,
                // revert to the previous state by removing these values from the list
                popPairs();

                return unfolded;
            }

            /**
             * If the device passes through the min/max twice in a row, erases from history to
             * revert back to previous state
             */
            void popPairs() {
                if (rolls.size() > 1) {
                    // comparing last two values
                    auto rit = rolls.rbegin();
                    double last = *rit;
                    ++rit;
                    // if the last two are the same, remove from list
                    if (last == *rit) {
                        rolls.pop_back();
                        rolls.pop_back();
                    }
                }
            }

            /**
             * Clears list memory
             */
            void clear() {
                rolls.clear();
            }

            /**
             * Printing function for testing
             */
            void printList() {
                std::cout << "list = ";
                for (auto i : rolls) {
                    std::cout << i << ", ";
                }
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
