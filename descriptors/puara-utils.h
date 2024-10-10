//********************************************************************************//
// Puara Gestures - Utilities (.h)                                                //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//


#ifndef PUARA_UTILS_H
#define PUARA_UTILS_H

#include <cmath>

#include <Eigen/Dense>
#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>

#include "puara-structs.h"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <deque>
#include <math.h>
#include <iomanip>

#include <iostream>

#include <cmath>
#include <numeric>
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
                old_value = current_value;
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
     * @brief Wrapper class undoes modular effects of angle measurements
     * Ensures that instead of "wrapping" (e.g. moving from - PI to PI),
     * measurements continue to decrease or increase
     */
    class Unwrap {
        public:
            double prev_angle;
            double accum;
            double range;
            bool empty;

            /**
             * @brief Constructor for Unwrap class
             *
             * Initaliazes accumulated at 0 to indicate the data has not "wrapped" yet
             * Initalizes empty at True
             * @param Min minimum input value
             * @param Max maximum input value
             */
            Unwrap (
                double Min,
                double Max
            ) : accum(0), range(Max - Min), empty(true) {}

            /**
             * @brief Calculates whether angle has "wrapped" based on previous angle
             * and updates accordingly
             */
            double unwrap(double reading) {
                double diff;

                // check if update hasn't been called before or the unwrap class
                // has been cleared
                if (empty) {
                    diff = 0;
                    empty = false;
                } else {
                    diff = reading - prev_angle;
                }
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
                empty = true;
            }
    };

    /**
     * @brief Wraps a value around a given minimum and maximum.
     * Algorithm from Jean-Michael Celerier. "Authoring interactive media : a logical
     * & temporal approach." Computation and Language [cs.CL]. Université de Bordeaux,
     * 2018. English. ffNNT : 2018BORD0037ff. fftel-01947309
     */
    class Wrap {
        public:
            double min;
            double max;

            /**
             *  @brief Constructor for Wrap class
             *
             * @param Min minimum value of desired range for "wrapper" object
             * @param Max maximum value of desired range for "wrapper" object
             */
            Wrap (
                double Min,
                double Max
            ) : min (Min), max (Max) {}

            double wrap(double reading) {
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
     * @brief "Smoothing" algorithm takes the average of a given number of previous
     * inputs. Can stabilize an erratic input stream.
     */
    class Smooth {
        public:
            std::list<double> list;
            double size;

            /**
             * @brief Constructor for Smooth class
             *
             * @param Size number of previous values that "smoother" object averages
             */
            Smooth(
                double Size
                ) : list(), size(Size){}

            /**
             * @brief Calls updateList then finds average of previous inputs
             */
            double smooth(double reading) {
                updateList(reading);
                double sumList = std::accumulate(list.begin(), list.end(), 0.0);
                return (sumList / list.size());
            }

            /**
             * @brief Updates list of previous inputs with current input
             */
            void updateList(double reading) {
                list.push_front(reading);
                // ensure list stays at desired size
                while (list.size() > size) {
                    list.pop_back();
                }
            }

            /**
             * @brief Clears list of all previous inputs
             */
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

    /**
     * Simple Threshold class to ensure a value doesn't exceed settable max and min values.
     */
    class Threshold {
        public:
            double min;
            double max;
            double current;

            Threshold(
                double Min = - 10.0,
                double Max = 10.0
                ) : min(Min), max(Max) {}

            double update(double reading) {
                current = reading;
                if (reading < min) {
                    return min;
                }
                if (reading > max) {
                    return max;
                }
                return reading;
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
     *  Calibrates the raw magnetometer values 
     */ 
    class Calibration {
        public:
            Imu9Axis myCalIMU;
            std::vector<Coord3D> rawMagData;
            Eigen::MatrixXd softIronMatrix;
            Eigen::VectorXd hardIronBias;

            Calibration() : softIronMatrix(3,3), hardIronBias(3) {
            softIronMatrix << 1,  1,  1, 
            1, 1,  1,
            1, 1,  1;
            
            hardIronBias << 0,0,0; 
            
            }

            /**
            *  The gravitation field is used to calculate the soft iron matrices and should be modified according to the follownig formula:
            *       1- Get the Total Field for your location from: 
            *          https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#igrfwmm
            *       2- Convert the Total Field value to Gauss (1nT = 10E-5G)
            *       3- Convert Total Field to Raw value Total Field, which is the
            *          Raw Gravitation Field we are searching for
            *          Read your magnetometer datasheet and find your gain value,
            *          Which should be the same of the collected raw points
            *
            *  Reference: https://github.com/nliaudat/magnetometer_calibration/blob/main/calibrate.py
            *  
            *
            */ 
            int gravitationField = 234; // should be 1000 by default, this was calculated for the LSM9DS1 in Montreal

            void applyMagnetometerCalibration(const Imu9Axis& myRawIMU) {
                myCalIMU.magn.x = softIronMatrix(0, 0) * (myRawIMU.magn.x - hardIronBias(0)) +
                                softIronMatrix(0, 1) * (myRawIMU.magn.y - hardIronBias(1)) +
                                softIronMatrix(0, 2) * (myRawIMU.magn.z - hardIronBias(2));

                myCalIMU.magn.y = softIronMatrix(1, 0) * (myRawIMU.magn.x - hardIronBias(0)) +
                                softIronMatrix(1, 1) * (myRawIMU.magn.y - hardIronBias(1)) +
                                softIronMatrix(1, 2) * (myRawIMU.magn.z - hardIronBias(2));

                myCalIMU.magn.z = softIronMatrix(2, 0) * (myRawIMU.magn.x - hardIronBias(0)) +
                                softIronMatrix(2, 1) * (myRawIMU.magn.y - hardIronBias(1)) +
                                softIronMatrix(2, 2) * (myRawIMU.magn.z - hardIronBias(2));
            }

            /**
            * Records the raw magnetometer data and saves it in a vector.
            * The user needs to call this a minimum amount of time in order to generate at least 1500 data points
            */  
            int recordRawMagData(const Coord3D& magData) { 
                rawMagData.push_back(magData);

                return rawMagData.size(); 
            } 
            
            /**
            * Fits an ellipsoid to 3D points by creating matrices from the input, solving for eigenvalues, 
            * and returning the ellipsoid's shape matrix M, center vector n, and scalar offset d
            */
            std::tuple<Eigen::MatrixXd, Eigen::VectorXd, double> ellipsoid_fit(const Eigen::MatrixXd& s) {
                
                // std::cout << "s " << std::endl << s << std::endl;

                Eigen::MatrixXd D(10,s.cols());

                D.row(0) = s.row(0).array().square();
                D.row(1) = s.row(1).array().square();
                D.row(2) = s.row(2).array().square();
                D.row(3) = 2.0 * s.row(1).array() * s.row(2).array();
                D.row(4) = 2.0 * s.row(0).array() * s.row(2).array();
                D.row(5) = 2.0 * s.row(0).array() * s.row(1).array();
                D.row(6) = 2.0 * s.row(0).array();
                D.row(7) = 2.0 * s.row(1).array();
                D.row(8) = 2.0 * s.row(2).array();
                D.row(9) = Eigen::MatrixXd::Ones(1, 105);

                // std::cout << std::fixed << std::setprecision(30);
    
                // std::cout << std::scientific << std::setprecision(8) << "D " << std::endl << D << std::endl;



                Eigen::MatrixXd S = D * D.transpose();

                Eigen::MatrixXd S_11 = S.block(0, 0, 6, 6);
                Eigen::MatrixXd S_12 = S.block(0, 6, 6, S.cols() - 6);
                Eigen::MatrixXd S_21 = S.block(6, 0, S.rows() - 6, 6);
                Eigen::MatrixXd S_22 = S.block(6, 6, S.rows() - 6, S.cols() - 6);

                // std::cout << "S " << std::endl << S << std::endl;
                                
                Eigen::MatrixXd C(6, 6);

                C << -1,  1,  1,  0,  0,  0,
                1, -1,  1,  0,  0,  0,
                1,  1, -1,  0,  0,  0,
                0,  0,  0, -4,  0,  0,
                0,  0,  0,  0, -4,  0,
                0,  0,  0,  0,  0, -4;
                
                Eigen::MatrixXd C_inv = C.inverse();
                
                Eigen::MatrixXd E =  C_inv * (S_11 - (S_12 * (S_22.inverse() * S_21)));

                Eigen::EigenSolver<Eigen::MatrixXd> solver(E);
                Eigen::VectorXd E_w = solver.eigenvalues().real();
                Eigen::MatrixXd E_v = solver.eigenvectors().real();

                int maxIndex = 0;
                E_w.maxCoeff(&maxIndex);
                Eigen::VectorXd v_1 = E_v.col(maxIndex);
                
                if (v_1(0) < 0) v_1 = -v_1;

                Eigen::VectorXd v_2 = (- S_22.inverse() * S_21) * v_1;
                
                Eigen::MatrixXd M(3,3);
                Eigen::VectorXd n(3);
                double d = v_2[3];

                M << v_1(0), v_1(5), v_1(4),
                v_1(5), v_1(1), v_1(3),
                v_1(4), v_1(3), v_1(2);

                n << v_2(0), v_2(1), v_2(2);

                return std::make_tuple(M, n, d);
            }

            /**
            * Generates magnetometer calibration matrices based on saved raw dataset by fitting an ellipsoid to a set of 3D coordinates, 
            * deriving the hard-iron bias and soft-iron matrix based on  the pre-defined gravitational field.
            */                                                                                                                                 
            int generateMagnetometerMatrices(std::vector<Coord3D> customRawMagData) {
                            
                if (customRawMagData.empty()) {
                    return 0;
                }

                Eigen::MatrixXd s(customRawMagData.size(),3);

                for (int i = 0; i < customRawMagData.size(); ++i) {
                    s(i, 0) = customRawMagData[i].x;
                    s(i, 1) = customRawMagData[i].y;
                    s(i, 2) = customRawMagData[i].z;
                }

                Eigen::MatrixXd M;
                Eigen::VectorXd n;
                double d;
                
                std::tie(M,n,d) = ellipsoid_fit(s.transpose());

                Eigen::MatrixXd M_1 = M.inverse();
                hardIronBias = - (M_1 * n);

                softIronMatrix = (gravitationField / std::sqrt((n.transpose() * (M_1 * n) -d)) * M.sqrt());

                return 1;
            }

            void generateMagnetometerMatrices() {
                generateMagnetometerMatrices(rawMagData);
            }
    };

    /**
     *  @brief Simple function to get the current elapsed time in microseconds.
     */
    inline unsigned long long getCurrentTimeMicroseconds() {
        auto currentTimePoint = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint.time_since_epoch());
        return duration.count();
    }

    /**
     * @brief Function used to reduce feature arrays into single values.
     * E.g., brush uses it to reduce multiBrush instances
     */
    inline double arrayAverageZero (double * Array, int ArraySize) {
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
    inline void bitShiftArrayL (int * origArray, int * shiftedArray, int arraySize, int shift) {
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
    inline double g_to_ms2(double reading) {
        return reading * 9.80665;
    }

    /**
     * @brief Convert m/s^2 to g's
     *
     */
    inline double ms2_to_g(double reading) {
        return reading / 9.80665;
    }

    /**
     * @brief Convert DPS to radians per second
     *
     */
    inline double dps_to_rads(double reading) {
        return reading * M_PI / 180;
    }

    /**
     * @brief Convert radians per second to DPS
     *
     */
    inline double rads_to_dps(double reading) {
        return reading * 180 / M_PI;
    }

    /**
     * @brief Convert Gauss to uTesla
     *
     */
    inline double gauss_to_utesla(double reading) {
        return reading / 10000;
    }

    /**
     * @brief Convert uTesla to Gauss
     *
     */
    inline double utesla_to_gauss(double reading) {
        return reading * 10000;
    }

    /**
     * @brief Convert polar coordinates to cartesian
     * 
    */
    Coord3D polar_to_cartesian(Spherical polarCoords) {
        Coord3D cartesianCoords;

        cartesianCoords.x = polarCoords.r * cos(polarCoords.theta) * cos(polarCoords.phi);
        cartesianCoords.y = polarCoords.r * cos(polarCoords.theta) * sin(polarCoords.phi);
        cartesianCoords.z = polarCoords.r * sin(polarCoords.theta);

        return cartesianCoords;
    }

    /**
     * @brief Convert cartesian coordinates to polar
     * 
    */
   Spherical cartesian_to_polar(Coord3D cartesianCoords) {
        Spherical polarCoords;

        polarCoords.r = sqrt(cartesianCoords.x * cartesianCoords.x +
                            cartesianCoords.y * cartesianCoords.y +
                            cartesianCoords.z * cartesianCoords.z);
        
        polarCoords.theta = atan2(cartesianCoords.z, 
                                sqrt(cartesianCoords.x * cartesianCoords.x + 
                                    cartesianCoords.y * cartesianCoords.y));

        polarCoords.phi = atan2(cartesianCoords.y, cartesianCoords.x);

        return polarCoords;
}

}
}

#endif
