#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidcsv.h>

#include <puara/gestures.h>

#include <cmath>
#include <filesystem>

using namespace puara_gestures;

static double readCsvDouble(const rapidcsv::Document& doc, const std::string& column, size_t row)
{
  return doc.GetCell<double>(column, row);
}

static std::filesystem::path getTestDataPath(std::string_view filename)
{
  const auto sourceDir = std::filesystem::path(__FILE__).parent_path();
  return sourceDir / "data" / filename;
}

TEST_CASE("Roll descriptor follows reference CSV values", "[descriptors][roll]")
{
  const auto path = getTestDataPath("imu_data_roll.csv");
  REQUIRE(std::filesystem::exists(path));
  rapidcsv::Document doc(path.string(), rapidcsv::LabelParams(0, -1));
  const size_t rowCount = doc.GetRowCount();

  Roll test;
  double maxDiff = 0.0;

  for (size_t r = 0; r < rowCount; ++r)
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

    const double expectedRoll = readCsvDouble(doc, "roll", r);
    const double measuredRoll = test.roll(accl, gyro, mag, timestamp);
    const double diff = std::fabs(measuredRoll - expectedRoll);

    if (r < 3) {
      std::cerr << "DEBUG roll row=" << r
                << " ts=" << timestamp
                << " accl=" << accl.x << "," << accl.y << "," << accl.z
                << " gyro=" << gyro.x << "," << gyro.y << "," << gyro.z
                << " mag=" << mag.x << "," << mag.y << "," << mag.z
                << " expected=" << expectedRoll
                << " measured=" << measuredRoll << "\n";
    }

    INFO("row=" << r << " timestamp=" << timestamp << " expected=" << expectedRoll << " measured=" << measuredRoll << " diff=" << diff);
    CHECK(diff < 0.35);
    maxDiff = std::max(maxDiff, diff);
  }

  CHECK(maxDiff < 0.35);
}

TEST_CASE("Tilt descriptor follows reference CSV values", "[descriptors][tilt]")
{
  const auto path = getTestDataPath("imu_data_tilt.csv");
  REQUIRE(std::filesystem::exists(path));
  rapidcsv::Document doc(path.string(), rapidcsv::LabelParams(0, -1));
  const size_t rowCount = doc.GetRowCount();

  Tilt test;
  double maxDiff = 0.0;

  for (size_t r = 0; r < rowCount; ++r)
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

    INFO("row=" << r << " timestamp=" << timestamp << " expected=" << expectedTilt << " measured=" << measuredTilt << " diff=" << diff);
    CHECK(diff < 0.35);
    maxDiff = std::max(maxDiff, diff);
  }

  CHECK(maxDiff < 0.35);
}

TEST_CASE("Touch descriptor computes expected averages and gesture values", "[descriptors][touch]")
{
  constexpr int maxNumBlobs = 4;
  constexpr int touchSizeEdge = 4;
  constexpr int touchSize = 16;

  TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> touchArrayGD;
  int touchArray[touchSize] = {0};

  touchArray[0] = 1;
  touchArray[5] = 1;
  touchArray[6] = 1;
  touchArray[8] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[14] = 1;
  touchArray[15] = 1;

  touchArrayGD.update(touchArray, touchSize);

  CHECK(touchArrayGD.totalTouchAverage == Catch::Approx(0.5).margin(1e-3));
  CHECK(touchArrayGD.topTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.middleTouchAverage == Catch::Approx(0.625).margin(1e-3));
  CHECK(touchArrayGD.bottomTouchAverage == Catch::Approx(0.5).margin(1e-3));
  CHECK(touchArrayGD.totalBrush >= 0.0f);
  CHECK(touchArrayGD.totalRub >= 0.0f);

  std::fill(std::begin(touchArray), std::end(touchArray), 0);
  touchArray[1] = 1;
  touchArray[6] = 1;
  touchArray[7] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[11] = 1;
  touchArray[15] = 1;

  touchArrayGD.update(touchArray, touchSize);

  CHECK(touchArrayGD.totalTouchAverage == Catch::Approx(0.4375).margin(1e-3));
  CHECK(touchArrayGD.topTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.middleTouchAverage == Catch::Approx(0.625).margin(1e-3));
  CHECK(touchArrayGD.bottomTouchAverage == Catch::Approx(0.25).margin(1e-3));
  CHECK(touchArrayGD.totalBrush >= 0.0f);
  CHECK(touchArrayGD.totalRub >= 0.0f);
}
