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
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/MatrixFunctions>

#include "puara-structs.h"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <deque>
#include <math.h>
#include <iomanip>
#include <iostream>


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

            double integrate(double reading, double custom_old_value, double custom_leak, int custom_frequency, unsigned long long& custom_timer){
                auto currentTimePoint = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint.time_since_epoch());
                unsigned long long current_time = duration.count();
                if (custom_frequency <= 0) {
                    current_value = reading + (custom_old_value * custom_leak);
                } else if ((current_time/1000LL)  - (1000 / frequency) < custom_timer) {  
                    current_value = reading + custom_old_value;
                } else {
                    current_value = reading + (custom_old_value * custom_leak);
                    custom_timer = (current_time/1000LL);
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

            int mField = 234; // 1000 by default

            void setSoftIronMatrix(const Eigen::MatrixXd& newSoftIronMatrix) {
                softIronMatrix = newSoftIronMatrix;
            }

            void setHardIronBias(const Eigen::VectorXd& newHardIronBias) {
                hardIronBias = newHardIronBias;
            }

            void setMField(int newMField) {
                mField = newMField;
            }

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
            *  
            */ 
            int recordRawMagData(const Coord3D& magData) { // The user needs to call this a minimum amount of time (suggest a min size of dataset)
                rawMagData.push_back(magData);

                return rawMagData.size(); 
            }
            
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

            void generateMagnetometerMatrices() {
                
                // Mfield instructions?? or just in the doc ?? and say "We are using mfield..."
                // User instructions for calculating 

                // Record imu data and store it in rawMagData

                // Either record for x seconds or until user presses a key
                
                Eigen::MatrixXd s(rawMagData.size(),3);

                for (int i = 0; i < rawMagData.size(); ++i) {
                    s(i, 0) = rawMagData[i].x;
                    s(i, 1) = rawMagData[i].y;
                    s(i, 2) = rawMagData[i].z;
                }

                Eigen::MatrixXd M;
                Eigen::VectorXd n;
                double d;
                
                std::tie(M,n,d) = ellipsoid_fit(s.transpose());

                Eigen::MatrixXd M_1 = M.inverse();
                hardIronBias = - (M_1 * n);

                softIronMatrix = (mField / std::sqrt((n.transpose() * (M_1 * n) -d)) * M.sqrt());

                std::cout << "hardIronBias " << std::endl << hardIronBias << std::endl;
                std::cout << "softIronMatrix " << std::endl << softIronMatrix << std::endl;

            }
    };

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

    //TODO cartesian to polar and vice versa

}
}

#endif
