//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
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
  // declare tilt
  Tilt test;
  utils::Threshold thresh;

  // set up common path
  std::string common = PUARA_TESTS_DIR "/tilt/";

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

  // read in tilt data
  std::string tilt_path = common + "tilt_raw.csv";
  std::ifstream tilt_file(tilt_path);
  std::string tilt_line;

  while(std::getline(accl_file, accl_line, '\n')
        && std::getline(gyro_file, gyro_line, '\n')
        && std::getline(timestamp_file, timestamp_line, '\n')
        && std::getline(mag_file, mag_line, '\n')
        && std::getline(tilt_file, tilt_line, '\n'))
  {
    if(accl_line.empty() || gyro_line.empty() || timestamp_line.empty()
       || mag_line.empty() || tilt_line.empty())
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

    double tilt = readinRawSingleValue(tilt_line);

    double puara_tilt = test.tilt(accl, gyro, mag, timestamp);

    std::cout << "Tilt Value from Puara = " << puara_tilt << ", ";
    std::cout << "Tilt Value from T-Stick = " << tilt << ". ";
    std::cout << "Diff = " << (std::fabs(puara_tilt - tilt)) << ".\n";
    double smoothed = test.smooth(puara_tilt);
    std::cout << "Smoothed = " << smoothed << ", ";
    double threshold = thresh.update(smoothed);
    std::cout << "In Threshold = " << threshold << ", ";
  }
}
