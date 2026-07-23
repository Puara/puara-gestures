#include "test_helpers.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <algorithm>
#include <cmath>
#include <filesystem>

using namespace puara_gestures;

TEST_CASE("Tilt descriptor follows reference CSV values", "[descriptors][tilt]")
{
  const auto path = getTestDataPath("imu_data_tilt.csv");
  REQUIRE(std::filesystem::exists(path));
  rapidcsv::Document doc(path.string(), rapidcsv::LabelParams(0, -1));
  const size_t rowCount = doc.GetRowCount();

  Tilt test;

  double maxDiff = 0.0;

  for(size_t r = 0; r < rowCount; ++r)
  {
    const double timestamp = readCsvDouble(doc, "timestamp", r);

    Coord3D accl{
        readCsvDouble(doc, "accl_x", r),
        readCsvDouble(doc, "accl_y", r),
        readCsvDouble(doc, "accl_z", r),
    };

    Coord3D gyro{
        readCsvDouble(doc, "gyro_x", r),
        readCsvDouble(doc, "gyro_y", r),
        readCsvDouble(doc, "gyro_z", r),
    };

    Coord3D mag{
        readCsvDouble(doc, "mag_x", r),
        readCsvDouble(doc, "mag_y", r),
        readCsvDouble(doc, "mag_z", r),
    };

    const double expectedTilt = readCsvDouble(doc, "tilt", r);
    const double measuredTilt = test.tilt(accl, gyro, mag, timestamp);
    const double diff = std::fabs(measuredTilt - expectedTilt);

    INFO(
        "row=" << r << " timestamp=" << timestamp << " expected=" << expectedTilt
               << " measured=" << measuredTilt << " diff=" << diff);
    CHECK(diff < 0.35);
    maxDiff = std::max(maxDiff, diff);
  }

  CHECK(maxDiff < 0.35);
}
