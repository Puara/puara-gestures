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

        // for (std::string s : onefile) {

        //     std::cout << "\n----------------------------------------------------------------------\n";
        //     std::cout << s << "\n";
        //     std::cout << "----------------------------------------------------------------------\n";

            // set up common path
            std::string common = "../../test_files/t-stick/";

            // read in accl data
            std::string accl_path = common + "accel_raw.csv";
            std::ifstream accl_file(accl_path);
            std::string accl_line;

            // read in gyro data
            std::string gyro_path = common + "gyro_raw.csv";;
            std::ifstream gyro_file(gyro_path);
            std::string gyro_line;

            // read in timestamp data
            std::string timestamp_path = common + "timestamp_raw.csv";
            std::ifstream timestamp_file(timestamp_path);
            std::string timestamp_line;

            // read in mag data
            std::string mag_path = common +  "mag_raw.csv";
            std::ifstream mag_file(mag_path);
            std::string mag_line;

            // read in roll data
            std::string roll_path = common +  "roll_raw.csv";
            std::ifstream roll_file(roll_path);
            std::string roll_line;

            // utils::TimeDifference timeDiff;
            while (std::getline(accl_file, accl_line, '\n') && std::getline(gyro_file, gyro_line, '\n') && std::getline(timestamp_file, timestamp_line, '\n') && std::getline(mag_file, mag_line, '\n') && std::getline(roll_file, roll_line, '\n')) {
                if (accl_line.empty() || gyro_line.empty() || timestamp_line.empty() || mag_line.empty() || roll_line.empty() ) {
                    break;
                }

                // get and set coordinates in orientation object
                // get accl coordinate
                puara_gestures::Coord3D accl;
                accl = utils::readinRawCSV(accl_line);
                std::cout << "READ IN FROM SPARKFUN: \naccl.x = " << accl.x << ", accl.y = " << accl.y << ", accl.z = " << accl.z << "\n";
                // accl.x = 1;
                // accl.y = 1;
                // accl.z = 1;

                // get gyro coordinates
                puara_gestures::Coord3D gyro;
                gyro = utils::readinRawCSV(gyro_line);
                std::cout << "gyro.x = " << gyro.x << ", gyro.y = " << gyro.y << ", gyro.z = " << gyro.z << "\n";
                // gyro.x =  1;
                // gyro.y =  1;
                // gyro.z =  1;

                // get timestamp value
                double timestamp = utils::readinRawSingleValue(timestamp_line);

                // get mag coordinates
                puara_gestures::Coord3D mag;
                mag = utils::readinRawCSV(mag_line);
                std::cout << "mag.x = " << mag.x << ", mag.y = " << mag.y << ", mag.z = " << mag.z << "\n";
                // mag.x = 1;
                // mag.y = 1;
                // mag.z = 1;

                // get roll
                double roll =  utils::readinRawSingleValue(roll_line);

                double puara_roll = test.update(accl, gyro, mag, timestamp);

                std::cout << "ORIENTATION VALUES POST-UPDATE:\n";
                test.printOrientationInfo();


                std::cout << "RESULTS:\n";
                std::cout << "Puara Function = " << puara_roll;
                std::cout << "; Roll value = " << roll;
                std::cout << "; Difference = " << std::fabs(puara_roll - roll) << "\n";
                // std::cout << "; unwrapped = " << test.unwrap(ypr.z);
                // std::cout << "; wrapped = " << test.wrap(test.unwrap(ypr.z), 0, 6.28) << "\n";
            }
            test.clear_unwrap();
            test.clear_smooth();
        //}
    };
};
};