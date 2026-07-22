#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <puara/gestures.h>

#include <cmath>
#include <rapidcsv.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <thread>

using namespace puara_gestures;

static double
readCsvDouble(const rapidcsv::Document& doc, const std::string& column, size_t row)
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

TEST_CASE(
    "Simple Tilt/Roll descriptor computes roll and tilt from accelerometer data",
    "[descriptors][simple_tilt_roll]")
{
  puara_gestures::Tilt_Roll simple;
  simple.update(0.0, 0.0, 1.0);

  CHECK(simple.current_roll_value() == Catch::Approx(M_PI_2).margin(1e-6));
  CHECK(simple.current_tilt_value() == Catch::Approx(0.0).margin(1e-6));

  simple.update(1.0, 0.0, 0.0);
  CHECK(simple.current_roll_value() == Catch::Approx(0.0).margin(1e-6));
  CHECK(simple.current_tilt_value() == Catch::Approx(M_PI_2).margin(1e-3));

  Coord3D imu_data{0.0, 0.0, 1.0};
  Tilt_Roll tied(&imu_data);
  tied.update();
  CHECK(tied.current_roll_value() == Catch::Approx(M_PI_2).margin(1e-6));
  CHECK(tied.current_tilt_value() == Catch::Approx(0.0).margin(1e-6));
}

TEST_CASE(
    "Jab descriptor detects motion range, ties external data, and supports 2D/3D",
    "[descriptors][jab]")
{
  puara_gestures::Jab jab;
  jab.threshold = 1;

  // Use the first x-axis values from imu_data_jab_shake.csv as realistic motion input.
  CHECK(jab.update(5.516652) == Catch::Approx(0.0).margin(1e-6));
  CHECK(jab.update(3.8303106) == Catch::Approx(1.6863414).margin(1e-6));
  CHECK(jab.update(4.2335014) == Catch::Approx(1.6863414).margin(1e-6));
  CHECK(jab.update(-0.5012963) == Catch::Approx(6.0179487).margin(1e-6));
  CHECK(jab.current_value() == Catch::Approx(6.0179487).margin(1e-6));

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Jab tiedJab(&tiedData);
  tiedJab.threshold = 1;
  tiedData.x = 0.0;
  tiedJab.update();
  tiedData.x = 4.0;
  tiedJab.update();
  CHECK(tiedJab.current_value() == Catch::Approx(4.0).margin(1e-6));

  puara_gestures::Jab2D jab2d;
  jab2d.threshold(1);
  jab2d.update(0.0, 0.0);
  jab2d.update(3.0, -3.0);
  auto values2d = jab2d.current_value();
  CHECK(values2d.x == Catch::Approx(3.0).margin(1e-6));
  CHECK(values2d.y == Catch::Approx(3.0).margin(1e-6));

  puara_gestures::Jab3D jab3d;
  jab3d.threshold(1);
  jab3d.update(0.0, 0.0, 0.0);
  jab3d.update(3.0, -3.0, 1.5);
  auto values3d = jab3d.current_value();
  CHECK(values3d.x == Catch::Approx(3.0).margin(1e-6));
  CHECK(values3d.y == Catch::Approx(3.0).margin(1e-6));
  CHECK(values3d.z == Catch::Approx(1.5).margin(1e-6));
}

TEST_CASE(
    "Shake descriptor integrates high movement and decays when motion stops",
    "[descriptors][shake]")
{
  puara_gestures::Shake shake;
  shake.threshold = 0.1;
  shake.fast_leak = 0.6;
  shake.slow_leak = 0.3;
  shake.frequency(0);

  CHECK(shake.update(10.0) == Catch::Approx(1.0).margin(1e-6));
  CHECK(shake.current_value() == Catch::Approx(1.0).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.3).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.09).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.027).margin(1e-6));
  CHECK(shake.update(0.0) == Catch::Approx(0.0).margin(1e-6));

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Shake tiedShake(&tiedData);
  tiedShake.threshold = 0.1;
  tiedShake.frequency(0);
  tiedData.x = 0.0;
  tiedShake.update();
  tiedData.x = 10.0;
  tiedShake.update();
  CHECK(tiedData.x == Catch::Approx(10.0).margin(1e-6));
  CHECK(tiedShake.current_value() == Catch::Approx(1.0).margin(1e-6));

  puara_gestures::Shake2D shake2d;
  shake2d.threshold(0.1);
  shake2d.frequency(0);
  shake2d.update(10.0, 5.0);
  auto values2d = shake2d.current_value();
  CHECK(values2d.x == Catch::Approx(1.0).margin(1e-6));
  CHECK(values2d.y == Catch::Approx(0.5).margin(1e-6));

  puara_gestures::Shake3D shake3d;
  shake3d.threshold(0.1);
  shake3d.frequency(0);
  shake3d.update(10.0, 5.0, 2.0);
  auto values3d = shake3d.current_value();
  CHECK(values3d.x == Catch::Approx(1.0).margin(1e-6));
  CHECK(values3d.y == Catch::Approx(0.5).margin(1e-6));
  CHECK(values3d.z == Catch::Approx(0.2).margin(1e-6));
}

TEST_CASE(
    "Touch descriptor computes expected averages and gesture values",
    "[descriptors][touch]")
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

TEST_CASE("Button descriptor tracks taps, press time, and hold", "[descriptors][button]")
{
  Button button;
  button.countInterval = 1;
  button.holdInterval = 1000;

  button.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  button.update(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  button.update(0);

  CHECK(button.tap == 1);
  CHECK(button.doubleTap == 0);
  CHECK(button.tripleTap == 0);
  CHECK(button.pressTime > 0);
  CHECK(button.count == 0);

  Button doubleTapButton;
  doubleTapButton.countInterval = 1;
  doubleTapButton.holdInterval = 1000;

  doubleTapButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);
  doubleTapButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  doubleTapButton.update(0);

  CHECK(doubleTapButton.tap == 0);
  CHECK(doubleTapButton.doubleTap == 1);
  CHECK(doubleTapButton.tripleTap == 0);

  Button holdButton;
  holdButton.countInterval = 1000;
  holdButton.holdInterval = 5;

  holdButton.update(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  holdButton.update(1);

  CHECK(holdButton.hold == true);
}

TEST_CASE("Ribbon reports position, slide velocity and direction",
          "[descriptors][ribbon]")
{
  Ribbon ribbon;

  // Touch down: onset, no velocity yet.
  ribbon.update(0.5, true, 0.0);
  CHECK(ribbon.touched == true);
  CHECK(ribbon.onset == true);
  CHECK(ribbon.position == Catch::Approx(0.5).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(0.0).margin(1e-9));

  // Slide up by 0.1 over 0.1 s -> velocity 1.0, direction +1.
  ribbon.update(0.6, true, 0.1);
  CHECK(ribbon.position == Catch::Approx(0.6).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(1.0).margin(1e-9));
  CHECK(ribbon.direction == 1);

  // Slide back down.
  ribbon.update(0.5, true, 0.1);
  CHECK(ribbon.direction == -1);
  CHECK(ribbon.velocity == Catch::Approx(-1.0).margin(1e-9));
}

TEST_CASE("Ribbon does not compute velocity across a re-touch",
          "[descriptors][ribbon]")
{
  Ribbon ribbon;

  ribbon.update(0.1, true, 0.0);  // touch near one end
  ribbon.update(0.1, false, 0.0); // lift
  CHECK(ribbon.release == true);

  // New touch far away: onset resets, no giant spurious velocity.
  ribbon.update(0.9, true, 0.1);
  CHECK(ribbon.onset == true);
  CHECK(ribbon.position == Catch::Approx(0.9).margin(1e-9));
  CHECK(ribbon.velocity == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("Ribbon detects a quick tap and rejects long/large touches",
          "[descriptors][ribbon]")
{
  Ribbon tapRibbon;
  tapRibbon.tapMaxTime = 200.0;
  tapRibbon.tapMaxTravel = 0.05;

  // Short touch that barely moves -> tap on release.
  tapRibbon.update(0.5, true, 0.0);
  tapRibbon.update(0.51, true, 0.05);  // +50 ms, travel 0.01
  tapRibbon.update(0.51, false, 0.0);
  CHECK(tapRibbon.release == true);
  CHECK(tapRibbon.tap == true);

  // A touch that travels too far is not a tap.
  Ribbon slideRibbon;
  slideRibbon.tapMaxTime = 200.0;
  slideRibbon.tapMaxTravel = 0.05;
  slideRibbon.update(0.2, true, 0.0);
  slideRibbon.update(0.6, true, 0.05);  // travels 0.4
  slideRibbon.update(0.6, false, 0.0);
  CHECK(slideRibbon.release == true);
  CHECK(slideRibbon.tap == false);
}
