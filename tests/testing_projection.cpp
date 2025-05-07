//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Zachary L'Heureux (2025)                                                       //
//********************************************************************************//

#include <puara/gestures.h>

#include <cmath>

#include <fstream>
#include <iostream>

using namespace puara_gestures;
Coord3D readinRawCSV(std::string line)
{
  Coord3D cart;
  int first_space = line.find_first_of(",");
  int second_space = line.substr(first_space + 1).find_first_of(",") + first_space + 1;
  double x = std::stod(line.substr(0, first_space));
  double y = std::stod(line.substr(first_space + 1, second_space));
  double z = std::stod(line.substr(second_space + 1, line.size()));
  // add three doubles to a Coord3D
  cart.x = x;
  cart.y = y;
  cart.z = z;
  // return Coord3D
  return cart;
}

double readinRawSingleValue(std::string line)
{
  return std::stod(line);
}

int main()
{
    // declare projection
    Projection2D test;

    // set up common path
    std::string common = PUARA_TESTS_DIR "/projection/";

    // read in orientation data
    std::string orientation_path = common + "orientation_raw.csv";
    std::ifstream orientation_file(orientation_path);
    std::string orientation_line;

    while (std::getline(orientation_file, orientation_line))
    {
        if(orientation_line.empty())
        {
            break;
        }
        
        // read in orientation data
        Coord3D orientation = readinRawCSV(orientation_line);
        // update projection with orientation data
        test.update(orientation.x, orientation.y, orientation.z);
        // print projection value
        std::cout << "Projection: " << test.current_value() << std::endl;
    }
}