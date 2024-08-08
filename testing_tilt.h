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


class testing_tilt {
    public:

    void test() {
        // declare tilt
        puara_gestures::Tilt test;

        // set up common path
        std::string common = "/Users/maggieneedham/Downloads/InternshipSAT/puara-gestures/testing_files/";

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

        // read in tilt data
        std::string tilt_path = common +  "pitch_raw.csv";
        std::ifstream tilt_file(tilt_path);
        std::string tilt_line;

        // utils::TimeDifference timeDiff;
        while (std::getline(accl_file, accl_line, '\n') && std::getline(gyro_file, gyro_line, '\n') && std::getline(timestamp_file, timestamp_line, '\n') && std::getline(mag_file, mag_line, '\n') && std::getline(tilt_file, tilt_line, '\n')) {
            if (accl_line.empty() || gyro_line.empty() || timestamp_line.empty() || mag_line.empty() || tilt_line.empty() ) {
                break;
            }

            // get and set coordinates in orientation object
            // get accl coordinate
            puara_gestures::Coord3D accl;
            accl = utils::readinRawCSV(accl_line);

            // get gyro coordinates
            puara_gestures::Coord3D gyro;
            gyro = utils::readinRawCSV(gyro_line);

            // get timestamp value
            double timestamp = utils::readinRawSingleValue(timestamp_line);

            // get mag coordinates
            puara_gestures::Coord3D mag;
            mag = utils::readinRawCSV(mag_line);

            // get tilt
            double tilt =  utils::readinRawSingleValue(tilt_line);

            double puara_tilt = test.tilt(accl, gyro, mag, timestamp);

            std::cout << "Puara Function = " << puara_tilt;
            std::cout << "; tilt value = " << tilt;
            std::cout << "; Difference = " << std::fabs(puara_tilt - tilt) << "\n";
            // std::cout << "; unwrapped = " << test.unwrap(ypr.z);
            // std::cout << "; wrapped = " << test.wrap(test.unwrap(ypr.z), 0, 6.28) << "\n";
        }
        test.clear_unwrap();
        test.clear_smooth();
    };
};
};