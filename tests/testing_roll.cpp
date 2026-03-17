//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#include <fstream>
#include <puara/gestures.h>
#include "rapidcsv.h"

using namespace puara_gestures;

int main()
{
  // declare roll
  Roll test;
  utils::Threshold thresh;

  // Load CSV with header row (labels in first line)
  // (use a relative path from the repo root where this is typically run)
  const std::string path = "tests/imu_data_roll.csv";

  rapidcsv::Document doc = rapidcsv::Document(path, rapidcsv::LabelParams(0, -1));

/*  // Uncomment to output to CSV
  std::ofstream out("tests/imu_data_roll_out.csv");
  out << "timestamp,t-stick_roll,puara_roll,diff_tstick-puara,smoothed,in_threshold\n";
*/
  const size_t rowCount = doc.GetRowCount();

  for (size_t r = 0; r < rowCount; ++r)
  {
    double timestamp = doc.GetCell<double>("timestamp", r);

    Coord3D accl;
    accl.x = doc.GetCell<double>("accl_x", r);
    accl.y = doc.GetCell<double>("accl_y", r);
    accl.z = doc.GetCell<double>("accl_z", r);

    Coord3D gyro;
    gyro.x = doc.GetCell<double>("gyro_x", r);
    gyro.y = doc.GetCell<double>("gyro_y", r);
    gyro.z = doc.GetCell<double>("gyro_z", r);


    Coord3D mag;
    mag.x = doc.GetCell<double>("mag_x", r);
    mag.y = doc.GetCell<double>("mag_y", r);
    mag.z = doc.GetCell<double>("mag_z", r);

    double roll = doc.GetCell<double>("roll", r);

    double puara_roll = test.roll(accl, gyro, mag, timestamp);

    double diff = std::fabs(puara_roll - roll);
    double smoothed = test.smooth(puara_roll);
    double in_threshold = thresh.update(smoothed);
    double wrapped = test.wrap(in_threshold);

/* // Uncomment to output to CSV
    out << timestamp << ","
        << roll << "," << puara_roll << "," << diff << ","
        << smoothed << "," << in_threshold << "," << wrapped << "\n";
*/
  }

  return 0;
}
