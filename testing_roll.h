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

    int test() {
        // declare roll and unfolder objects
        puara_gestures::Roll test(50);
        puara_gestures::utils::Unfold unfolder;
        unfolder.clear();

        // file names
        std::list<std::string> filesAll = {"clock_-1.57_0.txt", "clock_-3.14_-1.57.txt", "clock_0_1.57.txt", "clock_1.57_3.14.txt", "counter_-1.57_-3.14.txt", "counter_0_-1.57.txt", "counter_1.57_0.txt", "counter_3.14_1.57.txt", "jab.txt", "lasso_clock.txt", "lasso_counter.txt", "roll_-1.57_1.57.txt", "roll_1.57_-1.57.txt", "roll_multiple_clock.txt", "roll_multiple_counter.txt", "shake.txt", "stat_-1.57.txt", "stat_0.txt", "stat_1.57.txt", "stat_3.14.txt"};
        std::list<std::string> files = {"roll_multiple_clock.txt"};

        // creating new raw files if not already made
        for (std::string s : files) {

            std::cout<< "In " << s << " file.\n";

            // set up common path
            std::string common = "/Users/maggieneedham/Downloads/InternshipSAT/puara-gestures/standalone/build/roll_lateral test data/";

            // read in raw data
            std::string raw_path = common + "raw_updated/" + s;
            std::ifstream raw_file(raw_path);
            std::string raw_line;

            // read in unwrapped data
            std::string unwrapped_path = common + "unwrapped/" + s;
            std::ifstream unwrapped_file(unwrapped_path);
            std::string unwrapped_line;

            // read in smoothed data
            std::string smoothed_path = common +  "smoothed/" + s;
            std::ifstream smoothed_file(smoothed_path);
            std::string smoothed_line;

            // read in final data
            std::string final_path = common + "final/" + s;
            std::ifstream final_file(final_path);
            std::string final_line;

            int line_count = 0;

            // loop through lines -- all files should be same length (160 values)
            while (std::getline(raw_file, raw_line, '\n')) {
                // increment line count
                line_count += 1;

                // get x, y, and z
                puara_gestures::Coord3D cart;
                cart = puara_gestures::utils::readinRaw(raw_line);

                // find azimuth
                double azimuth = puara_gestures::utils::getAzimuth(cart);
                std::cout << "angle = " << azimuth << ", ";

                // if first line, set prev angle to be the same as current
                if (line_count == 1) {
                    test.set_prev_angle(azimuth);
                }

                // "unfold" value
                double unfolded = unfolder.unfold(azimuth);
                std::cout << "unfolded = " << unfolded << ".\n";

                // "unwrap" value
                double unwrapped_puara = test.unwrap(azimuth);
                // std::cout << "puara unwrapped = " << unwrapped_puara << ", ";;

                // smooth unwrapped value
                double smoothed_puara = test.smooth(unfolded);
                std::cout << "smoothed = " << smoothed_puara << ", ";

                // process value
                double final_puara = test.fold(smoothed_puara, 0, 3.14);
                std::cout << "puara final = " << final_puara << ".\n";

                // get values from max patch
                std::getline(unwrapped_file, unwrapped_line, ' ');
                // std::cout<< "max unwrapped = " << unwrapped_line << ".";
                double unwrapped_max = std::stod(unwrapped_line);

                std::getline(smoothed_file, smoothed_line, ' ');
                // std::cout<< "max smoothed = " << smoothed_line << ", ";
                double smoothed_max = std::stod(smoothed_line);

                std::getline(final_file, final_line, ' ');
                // std::cout<< "max final = " << final_line << ".\n";
                double final_max = std::stod(final_line);

                // evaluate
                bool unwrapped_success = (unwrapped_max - unwrapped_puara) < 0.01 && (unwrapped_max - unwrapped_puara) > - 0.01;
                if (unwrapped_success) {
                    // std::cout << "Unwrapped S.\n\n";
                } else {
                    // std::cout << "Unwrapped F.\n\n";
                }

                bool smoothed_success = (smoothed_max - smoothed_puara) < 0.01 && (smoothed_max - smoothed_puara) > - 0.01;
                // if (smoothed_success) {
                //     std::cout << "Smoothed S.\n\n";
                // } else {
                //     std::cout << "Smoothed F. \n\n";
                // }

                bool final_success = (final_max - final_puara) < 0.01 && (final_max - final_puara) > - 0.01;
                // if (final_success) {
                //     std::cout << "Final S. \n\n";
                // } else {
                //     std::cout << "Final F.\n\n";
                // }
            }
            test.clear_unwrap();
            test.clear_smooth_list();
        }
        return 1;
    };
};
};