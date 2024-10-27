//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#include <puara/gestures.h>
#include <puara/utils.h>

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
  // declare roll
  Roll test;
  utils::Threshold thresh;

  // set up common path
  std::string common = PUARA_TESTS_DIR "/roll/";

  // read in accl data
  std::string accl_path = common + "accel_raw.csv";
  std::ifstream accl_file(accl_path);
  std::string accl_line;

  // read in gyro data
  std::string gyro_path = common + "gyro_raw.csv";
  ;
  std::ifstream gyro_file(gyro_path);
  std::string gyro_line;

  // read in timestamp data
  std::string timestamp_path = common + "timestamp_raw.csv";
  std::ifstream timestamp_file(timestamp_path);
  std::string timestamp_line;

  // read in mag data
  std::string mag_path = common + "mag_raw.csv";
  std::ifstream mag_file(mag_path);
  std::string mag_line;

  // read in roll data
  std::string roll_path = common + "roll_raw.csv";
  std::ifstream roll_file(roll_path);
  std::string roll_line;

  while(std::getline(accl_file, accl_line, '\n')
        && std::getline(gyro_file, gyro_line, '\n')
        && std::getline(timestamp_file, timestamp_line, '\n')
        && std::getline(mag_file, mag_line, '\n')
        && std::getline(roll_file, roll_line, '\n'))
  {
    if(accl_line.empty() || gyro_line.empty() || timestamp_line.empty()
       || mag_line.empty() || roll_line.empty())
    {
      break;
    }

    Coord3D accl;
    accl = readinRawCSV(accl_line);

    Coord3D gyro;
    gyro = readinRawCSV(gyro_line);

    double timestamp = readinRawSingleValue(timestamp_line);

    Coord3D mag;
    mag = readinRawCSV(mag_line);

    double roll = readinRawSingleValue(roll_line);

    double puara_roll = test.roll(accl, gyro, mag, timestamp);

    std::cout << "Roll Value from Puara = " << puara_roll << ", ";
    std::cout << "Roll Value from T-Stick = " << roll << ". ";
    std::cout << "Diff = " << (std::fabs(puara_roll - roll)) << ".\n";
    double unwrapped = test.unwrap(roll);
    std::cout << "Unwrapped = " << unwrapped << ", ";
    double smoothed = test.smooth(unwrapped);
    std::cout << "Smoothed = " << smoothed << ", ";
    double threshold = thresh.update(smoothed);
    std::cout << "In Threshold = " << threshold << ", ";
    double wrapped = test.wrap(threshold);
    std::cout << "Wrapped = " << wrapped << ".\n";
  }
}
