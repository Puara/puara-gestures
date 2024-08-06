//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#include "puara_gestures.h"

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/osc.hpp>

#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

namespace puara_gestures {


class testing_roll {
    public:

    void test() {

        // declare roll and unfolder objects
        Roll test;
        IMU_Orientation orientation;

        // file names
        std::list<std::string> files = {"jab.txt", "lasso.txt", "roll_clock.txt", "roll_counter.txt", "stat_down.txt", "stat_left.txt", "stat_right.txt", "stat_up.txt", "tilt_back.txt", "tilt_down.txt", "tilt_front.txt", "tilt_roll_back.txt", "tilt_roll_front.txt", "tilt_up.txt", "yaw_clock.txt", "yaw_counter.txt"};
        std::list<std::string> selected = {"roll_clock.txt", "roll_counter.txt", "stat_down.txt", "stat_left.txt", "stat_right.txt", "stat_up.txt" };
        std::list<std::string> onefile = {"roll_clock.txt"};
        for (std::string s : onefile) {

            std::cout << "\n----------------------------------------------------------------------\n";
            std::cout << s << "\n";
            std::cout << "----------------------------------------------------------------------\n";

            // set up common path
            std::string common = "/Users/maggieneedham/Downloads/InternshipSAT/puara-gestures/standalone/build/roll_lateral_imu data/updated/";

            // read in accl data
            std::string accl_path = common + "accl/" + s;
            std::ifstream accl_file(accl_path);
            std::string accl_line;

            // read in gyro data
            std::string gyro_path = common + "gyro/" + s;
            std::ifstream gyro_file(gyro_path);
            std::string gyro_line;

            // read in mag data
            std::string mag_path = common +  "mag/" + s;
            std::ifstream mag_file(mag_path);
            std::string mag_line;

            // read in ypr data
            std::string ypr_path = common +  "ypr/" + s;
            std::ifstream ypr_file(ypr_path);
            std::string ypr_line;

            // utils::TimeDifference timeDiff;
            while (std::getline(accl_file, accl_line, '\n') && std::getline(gyro_file, gyro_line, '\n') && std::getline(mag_file, mag_line, '\n') && std::getline(ypr_file, ypr_line, '\n')) {
                if ( accl_line.empty() || gyro_line.empty() || mag_line.empty() || ypr_line.empty() ) {
                    break;
                }

                // get and set coordinates in orientation object
                // get accl coordinate
                puara_gestures::Coord3D accl;
                accl = utils::readinRaw(accl_line);
                accl.x = accl.x / 9.80665;
                accl.y = accl.y / 9.80665;
                accl.z = accl.z / 9.80665;

                // get gyro coordinates
                puara_gestures::Coord3D gyro;
                gyro = utils::readinRaw(gyro_line);
                gyro.x =  gyro.x * 180 / M_PI;
                gyro.y =  gyro.y * 180 / M_PI;
                gyro.z =  gyro.z * 180 / M_PI;

                // get mag coordinates
                puara_gestures::Coord3D mag;
                mag = utils::readinRaw(mag_line);
                // divide by 10,000 (undoing processing in max)
                mag.x = mag.x;
                mag.y = mag.y;
                mag.z = mag.z;

                // calibrate magnetometer

                float sx[3] = {0.333, 0.333, 0.333};
                float sy[3] = {0.333, 0.333, 0.333};
                float sz[3] = {0.333, 0.333, 0.333};

                float h[3] = {0,0,0};

                mag.x = sx[0]*(mag.x-h[0]) + sx[1]*(mag.x-h[0]) + sx[2]*(mag.x-h[0]);
                mag.y = sy[0]*(mag.y-h[1]) + sy[1]*(mag.y-h[1]) + sy[2]*(mag.y-h[1]);
                mag.z = sz[0]*(mag.z-h[2]) + sz[1]*(mag.z-h[2]) + sz[2]*(mag.z-h[2]);

                // get yaw, pitch, and roll
                puara_gestures::Coord3D ypr;
                ypr = utils::readinRaw(ypr_line);

                long then = utils::getCurrentTimeMicroseconds();
                long now = utils::getCurrentTimeMicroseconds();
                std::cout << "Puara = " << test.update(accl, gyro, mag, ((then - now))* 0.000001) << "; ";
                std::cout << "Max = " << ypr.z << "\n";
                // std::cout << "; unwrapped = " << test.unwrap(ypr.z);
                // std::cout << "; wrapped = " << test.wrap(test.unwrap(ypr.z), 0, 6.28) << "\n";
            }
            test.clear_unwrap();
            test.clear_smooth();
        }
    };
};
};