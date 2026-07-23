#include "test_helpers.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>

using namespace puara_gestures;

TEST_CASE("Roll descriptor follows reference CSV values", "[descriptors][roll]")
{
  const auto path = getTestDataPath("imu_data_roll.csv");
  REQUIRE(std::filesystem::exists(path));
  rapidcsv::Document doc(path.string(), rapidcsv::LabelParams(0, -1));
  const size_t rowCount = doc.GetRowCount();

  Roll test;
  Imu9Axis imu_data;
  double maxDiff = 0.0;

  for(size_t r = 0; r < rowCount; ++r)
  {
    const double timestamp = readCsvDouble(doc, "timestamp", r);
    imu_data.accl.x = readCsvDouble(doc, "accl_x", r);
    imu_data.accl.y = readCsvDouble(doc, "accl_y", r);
    imu_data.accl.z = readCsvDouble(doc, "accl_z", r);
    imu_data.gyro.x = readCsvDouble(doc, "gyro_x", r);
    imu_data.gyro.y = readCsvDouble(doc, "gyro_y", r);
    imu_data.gyro.z = readCsvDouble(doc, "gyro_z", r);
    imu_data.magn.x = readCsvDouble(doc, "mag_x", r);
    imu_data.magn.y = readCsvDouble(doc, "mag_y", r);
    imu_data.magn.z = readCsvDouble(doc, "mag_z", r);

    const double expectedRoll = readCsvDouble(doc, "roll", r);
    const double measuredRoll
        = test.roll(imu_data.accl, imu_data.gyro, imu_data.magn, timestamp);
    const double diff = std::fabs(measuredRoll - expectedRoll);

    if(r < 3)
    {
      std::cerr << "DEBUG roll row=" << r << " ts=" << timestamp
                << " accl=" << imu_data.accl.x << "," << imu_data.accl.y << ","
                << imu_data.accl.z << " gyro=" << imu_data.gyro.x << ","
                << imu_data.gyro.y << "," << imu_data.gyro.z
                << " mag=" << imu_data.magn.x << "," << imu_data.magn.y << ","
                << imu_data.magn.z << " expected=" << expectedRoll
                << " measured=" << measuredRoll << "\n";
    }

    INFO(
        "row=" << r << " timestamp=" << timestamp << " expected=" << expectedRoll
               << " measured=" << measuredRoll << " diff=" << diff);
    CHECK(diff < 0.35);
    maxDiff = std::max(maxDiff, diff);
  }

  CHECK(maxDiff < 0.35);
}
