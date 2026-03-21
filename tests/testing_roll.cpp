//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#include <puara/gestures.h>
#include "rapidcsv.h"

using namespace puara_gestures;

int main()
{
  // declare roll
  Roll test;
  Imu9Axis imu_data;
  utils::Threshold thresh;

  // Load CSV with header row (labels in first line)
  // (use a relative path from the repo root where this is typically run)
  const std::string path = "tests/data/imu_data_roll.csv";
  rapidcsv::Document doc = rapidcsv::Document(path, rapidcsv::LabelParams(0, -1));
  const size_t rowCount = doc.GetRowCount();

  for (size_t r = 0; r < rowCount; ++r)
  {
    double timestamp = doc.GetCell<double>("timestamp", r);

    imu_data.accl.x = doc.GetCell<double>("accl_x", r);
    imu_data.accl.y = doc.GetCell<double>("accl_y", r);
    imu_data.accl.z = doc.GetCell<double>("accl_z", r);

    imu_data.gyro.x = doc.GetCell<double>("gyro_x", r);
    imu_data.gyro.y = doc.GetCell<double>("gyro_y", r);
    imu_data.gyro.z = doc.GetCell<double>("gyro_z", r);

    imu_data.magn.x = doc.GetCell<double>("mag_x", r);
    imu_data.magn.y = doc.GetCell<double>("mag_y", r);
    imu_data.magn.z = doc.GetCell<double>("mag_z", r);

    double t_stick_roll = doc.GetCell<double>("roll", r);

    double puara_roll = test.roll(imu_data.accl, imu_data.gyro, imu_data.magn, timestamp);
    double diff = std::fabs(puara_roll - t_stick_roll);
    double smoothed = test.smooth(puara_roll);
    double in_threshold = thresh.update(smoothed);
    double wrapped = test.wrap(in_threshold);
    double unwrapped = test.unwrap(wrapped);

    std::cout << "timestamp : " << timestamp << ","
              << "t_stick-roll : " << t_stick_roll << ","
              << "puara_roll : " << puara_roll << ",\n"
              << "diff (puara-roll - t-stick-roll): " << diff << ","
              << "smoothed(puara-roll) : " << smoothed << ","
              << "in_threshold(smoothed) : " << in_threshold << ","
              << "wrapped :   " << wrapped << ","
              << "unwrapped : " << unwrapped << "\n\n";
  }

  std::cout << "Before clear_unwrap(), unwrap(0.5) value: " << test.unwrap(0.5) << "\n"; 
  test.clear_unwrap();
  std::cout << "After clear_unwrap(), unwrap(0.5) value: " << test.unwrap(0.5) << "\n"; 
  std::cout << "Before clear_smooth(), smooth(0.5) value: " << test.smooth(0.5) << "\n";
  test.clear_smooth();
  std::cout << "After clear_smooth, smooth(0.5) value: " << test.smooth(0.5) << "\n";

  return 0;
}
