/*
  PlatformIO Embedded Verification Test for puara-gestures

  This test runner converts a small, portable subset of the existing
  Catch2 host tests into an embedded-friendly serial output harness.
*/

#include <Arduino.h>
#include <algorithm>
#include <cmath>

#include <puara/gestures.h>

static int g_failures = 0;
static int g_checks = 0;

static void logResult(bool pass, const char* testName) {
  g_checks += 1;
  if (!pass) {
    g_failures += 1;
    Serial.print("FAIL: ");
    Serial.println(testName);
  } else {
    Serial.print("PASS: ");
    Serial.println(testName);
  }
}

static bool almostEqual(double a, double b, double tol = 1e-6) {
  return std::fabs(a - b) <= tol;
}

static void testEigen() {
  const char* name = "Eigen identity matrix";
  Eigen::Matrix3f mat = Eigen::Matrix3f::Identity();
  logResult(mat(0, 0) == 1.0f && mat(1, 1) == 1.0f && mat(2, 2) == 1.0f, name);
}

static void testArrayAverage() {
  const char* name = "utils::arrayAverage and arrayAverageWithoutZero";
  const int data[] = {1, 2, 3, 4, 5};
  const double avg1 = puara_gestures::utils::arrayAverage(data, 0, 5);
  const double avg2 = puara_gestures::utils::arrayAverage(data, 1, 4);

  double data2[] = {1.0, 0.0, 3.0, 0.0, 5.0};
  const double avgNoZero = puara_gestures::utils::arrayAverageWithoutZero(data2, 5);

  logResult(almostEqual(avg1, 3.0) && almostEqual(avg2, 3.0) && almostEqual(avgNoZero, 3.0), name);
}

static void testBitShiftArrayL() {
  const char* name = "utils::bitShiftArrayL";
  int orig[] = {1, 1, 0, 0};
  int shifted[4] = {0};
  int shifted2[4] = {0};

  puara_gestures::utils::bitShiftArrayL(orig, shifted, 4, 1);
  puara_gestures::utils::bitShiftArrayL(orig, shifted2, 4, 2);

  bool ok = (shifted[0] == ((orig[0] << 1) | (orig[1] >> 7))) &&
            (shifted[1] == ((orig[1] << 1) | (orig[2] >> 7))) &&
            (shifted[2] == ((orig[2] << 1) | (orig[3] >> 7))) &&
            (shifted[3] == (orig[3] << 1)) &&
            (shifted2[0] == ((shifted[0] << 1) | (shifted[1] >> 7))) &&
            (shifted2[1] == ((shifted[1] << 1) | (shifted[2] >> 7))) &&
            (shifted2[2] == ((shifted[2] << 1) | (shifted[3] >> 7))) &&
            (shifted2[3] == (shifted[3] << 1));

  logResult(ok, name);
}

static void testBlobDetector() {
  const char* name = "BlobDetector core behavior";
  puara_gestures::BlobDetector<4> detector;
  int data[] = {0, 1, 1, 1, 0};

  detector.detect1D(data, 5);
  bool ok = detector.blobCount == 1 && detector.blobStartPos[0] == 1 && detector.blobSize[0] == 3;
  logResult(ok, name);
}

static void testStructAliases() {
  const char* name = "Spherical struct aliases";
  puara_gestures::Spherical s{};
  s.azimuth = 1.1;
  s.elevation = 2.2;
  s.distance = 3.3;
  bool ok = almostEqual(s.yaw, 1.1) && almostEqual(s.pitch, 2.2) && almostEqual(s.r, 3.3);
  s.yaw = -0.5;
  s.pitch = -1.25;
  s.r = 9.75;
  ok &= almostEqual(s.azimuth, -0.5) && almostEqual(s.elevation, -1.25) && almostEqual(s.distance, 9.75);
  logResult(ok, name);
}

static void testBoostCircularBuffer() {
  const char* name = "boost::circular_buffer basic";
  boost::circular_buffer<int> buf(4);
  buf.push_back(1);
  buf.push_back(2);
  buf.push_back(3);
  bool ok = (buf.size() == 3 && buf[0] == 1 && buf[2] == 3);
  logResult(ok, name);
}

static void testCircularBuffer() {
  const char* name = "CircularBuffer fixed capacity";
  puara_gestures::utils::CircularBuffer buf;
  buf.size = 3;
  buf.add(1.0);
  buf.add(2.0);
  buf.add(3.0);
  buf.add(4.0);

  bool ok = (buf.buffer.size() == 3 && buf.buffer[0] == 4.0 && buf.buffer[1] == 3.0 && buf.buffer[2] == 2.0);
  logResult(ok, name);
}

static void testMapRange() {
  const char* name = "MapRange mapping behavior";
  puara_gestures::utils::MapRange mapper;
  mapper.inMin = 0;
  mapper.inMax = 10;
  mapper.outMin = 0;
  mapper.outMax = 100;

  bool ok = (mapper.range(5) == 50.0) && (mapper.range(0) == 0.0) && (mapper.range(10) == 100.0);
  mapper.outMin = 2;
  mapper.outMax = 2;
  ok &= (mapper.range(5) == 2.0);
  logResult(ok, name);
}

static void testThreshold() {
  const char* name = "Threshold clamp behavior";
  puara_gestures::utils::Threshold threshold(-2.0, 2.0);

  bool ok = (threshold.update(1.5) == 1.5) && (threshold.current == 1.5);
  ok &= (threshold.update(3.0) == 2.0) && (threshold.current == 3.0);
  ok &= (threshold.update(-4.0) == -2.0) && (threshold.current == -4.0);
  ok &= (threshold.update(0.0) == 0.0) && (threshold.current == 0.0);

  logResult(ok, name);
}

static void testSmooth() {
  const char* name = "Smooth rolling average";
  puara_gestures::utils::Smooth smoother(3.0);

  bool ok = almostEqual(smoother.smooth(1.0), 1.0);
  ok &= almostEqual(smoother.smooth(2.0), 1.5);
  ok &= almostEqual(smoother.smooth(4.0), (1.0 + 2.0 + 4.0) / 3.0);
  ok &= almostEqual(smoother.smooth(5.0), (2.0 + 4.0 + 5.0) / 3.0);

  smoother.clear();
  ok &= smoother.list.empty();
  ok &= almostEqual(smoother.smooth(10.0), 10.0);

  logResult(ok, name);
}

static void testRollDescriptor() {
  const char* name = "Roll descriptor embedded sample";
  struct Sample { double timestamp, ax, ay, az, gx, gy, gz, mx, my, mz, expected; };
  const Sample samples[] = {
    {1.4e-05, -0.0219727, -0.0134277, 0.992676, 7.01904, 3.17383, -2.01416, -0.856567, 0.672119, 1.00867, 1.71506e-06},
    {0.059329, -0.0234375, -0.0114746, 0.989014, 7.56836, 3.2959, -1.89209, -0.858154, 0.668823, 1.01111, 0.0214476},
    {0.049012, -0.0197754, -0.0151367, 0.994385, 7.69043, 3.2959, -2.0752, -0.853516, 0.669556, 1.01416, 0.0414739}
  };

  puara_gestures::Roll test;
  bool ok = true;

  for (auto const& s : samples) {
    puara_gestures::Coord3D accl{s.ax, s.ay, s.az};
    puara_gestures::Coord3D gyro{s.gx, s.gy, s.gz};
    puara_gestures::Coord3D mag{s.mx, s.my, s.mz};
    double measured = test.roll(accl, gyro, mag, s.timestamp);
    double diff = std::fabs(measured - s.expected);
    ok &= std::isfinite(measured);
    ok &= diff < 0.35;
  }

  logResult(ok, name);
}

static void testTiltDescriptor() {
  const char* name = "Tilt descriptor embedded sample";
  struct Sample { double timestamp, ax, ay, az, gx, gy, gz, mx, my, mz, expected; };
  const Sample samples[] = {
    {1.4e-05, 0.0400391, -0.512451, 0.842529, 6.16455, 2.99072, -0.12207, 0.581909, 0.17627, -0.458496, 4.68283e-08},
    {0.033594, 0.0383301, -0.506103, 0.843994, 5.92041, 3.54004, -0.305176, 0.584961, 0.170654, -0.455688, 0.029345},
    {0.020996, 0.0395508, -0.505127, 0.841797, 5.00488, 3.66211, -0.0610352, 0.582642, 0.169678, -0.46228, 0.0572487}
  };

  puara_gestures::Tilt test;
  bool ok = true;

  for (auto const& s : samples) {
    puara_gestures::Coord3D accl{s.ax, s.ay, s.az};
    puara_gestures::Coord3D gyro{s.gx, s.gy, s.gz};
    puara_gestures::Coord3D mag{s.mx, s.my, s.mz};
    double measured = test.tilt(accl, gyro, mag, s.timestamp);
    double diff = std::fabs(measured - s.expected);
    ok &= std::isfinite(measured);
    ok &= diff < 0.35;
  }

  logResult(ok, name);
}

static void testSimpleTiltRollDescriptor() {
  const char* name = "Simple Tilt/Roll embedded sample";

  puara_gestures::Tilt_Roll simple;
  simple.update(0.0, 0.0, 1.0);
  bool ok = true;
  ok &= almostEqual(simple.current_roll_value(), M_PI_2, 1e-6);
  ok &= almostEqual(simple.current_tilt_value(), 0.0, 1e-6);

  simple.update(1.0, 0.0, 0.0);
  ok &= almostEqual(simple.current_roll_value(), 0.0, 1e-6);
  ok &= almostEqual(simple.current_tilt_value(), M_PI_2, 1e-3);

  puara_gestures::Coord3D imu_data{0.0, 0.0, 1.0};
  puara_gestures::Tilt_Roll tied(&imu_data);
  tied.update();
  ok &= almostEqual(tied.current_roll_value(), M_PI_2, 1e-6);
  ok &= almostEqual(tied.current_tilt_value(), 0.0, 1e-6);

  logResult(ok, name);
}

static void testTouchDescriptor() {
  const char* name = "Touch descriptor embedded sample";
  constexpr int maxNumBlobs = 4;
  constexpr int touchSizeEdge = 4;
  constexpr int touchSize = 16;

  puara_gestures::TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> detector;
  int touchArray[touchSize] = {0};

  touchArray[0] = 1;
  touchArray[5] = 1;
  touchArray[6] = 1;
  touchArray[8] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[14] = 1;
  touchArray[15] = 1;

  detector.update(touchArray, touchSize);

  bool ok = true;
  ok &= almostEqual(detector.totalTouchAverage, 0.5, 1e-3);
  ok &= almostEqual(detector.topTouchAverage, 0.25, 1e-3);
  ok &= almostEqual(detector.middleTouchAverage, 0.625, 1e-3);
  ok &= almostEqual(detector.bottomTouchAverage, 0.5, 1e-3);
  ok &= detector.totalBrush >= 0.0f;
  ok &= detector.totalRub >= 0.0f;

  std::fill(std::begin(touchArray), std::end(touchArray), 0);
  touchArray[1] = 1;
  touchArray[6] = 1;
  touchArray[7] = 1;
  touchArray[9] = 1;
  touchArray[10] = 1;
  touchArray[11] = 1;
  touchArray[15] = 1;

  detector.update(touchArray, touchSize);
  ok &= almostEqual(detector.totalTouchAverage, 0.4375, 1e-3);
  ok &= almostEqual(detector.topTouchAverage, 0.25, 1e-3);
  ok &= almostEqual(detector.middleTouchAverage, 0.625, 1e-3);
  ok &= almostEqual(detector.bottomTouchAverage, 0.25, 1e-3);
  ok &= detector.totalBrush >= 0.0f;
  ok &= detector.totalRub >= 0.0f;

  logResult(ok, name);
}

static void testButtonDescriptor() {
  const char* name = "Button descriptor embedded sample";

  puara_gestures::Button button;
  button.countInterval = 1;
  button.holdInterval = 1000;

  button.update(1);
  delay(5);
  button.update(0);
  delay(5);
  button.update(0);

  bool ok = true;
  ok &= (button.tap == 1);
  ok &= (button.doubleTap == 0);
  ok &= (button.tripleTap == 0);
  ok &= (button.pressTime > 0);
  ok &= (button.count == 0);

  puara_gestures::Button doubleTapButton;
  doubleTapButton.countInterval = 1;
  doubleTapButton.holdInterval = 1000;

  doubleTapButton.update(1);
  delay(5);
  doubleTapButton.update(0);
  doubleTapButton.update(1);
  delay(5);
  doubleTapButton.update(0);
  delay(5);
  doubleTapButton.update(0);

  ok &= (doubleTapButton.tap == 0);
  ok &= (doubleTapButton.doubleTap == 1);
  ok &= (doubleTapButton.tripleTap == 0);

  puara_gestures::Button holdButton;
  holdButton.countInterval = 1000;
  holdButton.holdInterval = 5;

  holdButton.update(1);
  delay(10);
  holdButton.update(1);

  ok &= holdButton.hold;
  logResult(ok, name);
}

static void testJabDescriptor() {
  const char* name = "Jab descriptor embedded sample";

  puara_gestures::Jab jab;
  jab.threshold = 1;

  bool ok = true;
  ok &= almostEqual(jab.update(5.516652), 0.0);
  ok &= almostEqual(jab.update(3.8303106), 1.6863414);
  ok &= almostEqual(jab.update(4.2335014), 1.6863414);
  ok &= almostEqual(jab.update(-0.5012963), 6.0179487);
  ok &= almostEqual(jab.current_value(), 6.0179487);

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Jab tiedJab(&tiedData);
  tiedJab.threshold = 1;
  tiedData.x = 0.0;
  tiedJab.update();
  tiedData.x = 4.0;
  tiedJab.update();
  ok &= almostEqual(tiedJab.current_value(), 4.0);

  puara_gestures::Jab2D jab2d;
  jab2d.threshold(1);
  jab2d.update(0.0, 0.0);
  jab2d.update(3.0, -3.0);
  auto val2d = jab2d.current_value();
  ok &= almostEqual(val2d.x, 3.0);
  ok &= almostEqual(val2d.y, 3.0);

  puara_gestures::Jab3D jab3d;
  jab3d.threshold(1);
  jab3d.update(0.0, 0.0, 0.0);
  jab3d.update(3.0, -3.0, 1.5);
  auto val3d = jab3d.current_value();
  ok &= almostEqual(val3d.x, 3.0);
  ok &= almostEqual(val3d.y, 3.0);
  ok &= almostEqual(val3d.z, 1.5);

  logResult(ok, name);
}

static void testShakeDescriptor() {
  const char* name = "Shake descriptor embedded sample";

  puara_gestures::Shake shake;
  shake.threshold = 0.1;
  shake.fast_leak = 0.6;
  shake.slow_leak = 0.3;
  shake.frequency(0);

  bool ok = true;
  ok &= almostEqual(shake.update(10.0), 1.0);
  ok &= almostEqual(shake.current_value(), 1.0);
  ok &= almostEqual(shake.update(0.0), 0.3);
  ok &= almostEqual(shake.update(0.0), 0.09);
  ok &= almostEqual(shake.update(0.0), 0.027);
  ok &= almostEqual(shake.update(0.0), 0.0);

  puara_gestures::Coord1D tiedData{0.0};
  puara_gestures::Shake tiedShake(&tiedData);
  tiedShake.threshold = 0.1;
  tiedShake.frequency(0);
  tiedData.x = 0.0;
  tiedShake.update();
  tiedData.x = 10.0;
  tiedShake.update();
  ok &= almostEqual(tiedShake.current_value(), 1.0);
  ok &= almostEqual(tiedData.x, 10.0);

  puara_gestures::Shake2D shake2d;
  shake2d.threshold(0.1);
  shake2d.frequency(0);
  shake2d.update(10.0, 5.0);
  auto val2d = shake2d.current_value();
  ok &= almostEqual(val2d.x, 1.0);
  ok &= almostEqual(val2d.y, 0.5);

  puara_gestures::Shake3D shake3d;
  shake3d.threshold(0.1);
  shake3d.frequency(0);
  shake3d.update(10.0, 5.0, 2.0);
  auto val3d = shake3d.current_value();
  ok &= almostEqual(val3d.x, 1.0);
  ok &= almostEqual(val3d.y, 0.5);
  ok &= almostEqual(val3d.z, 0.2);

  logResult(ok, name);
}

static void testIMUFilters() {
  const char* name = "IMU filter quaternion normalization";

  puara_gestures::Imu9Axis imu{
      {0.0, 0.0, 9.81},
      {1.0, 2.0, 3.0},
      {0.3, 0.0, 0.5}
  };

  bool ok = true;

  puara_gestures::MadgwickQuaternionFilter madgwick(0.1);
  ok &= (madgwick.update(imu, true) == false);
  delay(5);
  ok &= (madgwick.update(imu, true) == true);
  ok &= almostEqual(madgwick.getQuaternion().w * madgwick.getQuaternion().w +
                    madgwick.getQuaternion().x * madgwick.getQuaternion().x +
                    madgwick.getQuaternion().y * madgwick.getQuaternion().y +
                    madgwick.getQuaternion().z * madgwick.getQuaternion().z,
                    1.0);
  madgwick.reset();
  ok &= almostEqual(madgwick.getQuaternion().w, 1.0) &&
        almostEqual(madgwick.getQuaternion().x, 0.0) &&
        almostEqual(madgwick.getQuaternion().y, 0.0) &&
        almostEqual(madgwick.getQuaternion().z, 0.0) &&
        almostEqual(madgwick.lastUpdateMicros, 0);

  ok &= (madgwick.updateWithTimestamp(imu, 0, true) == false);
  ok &= (madgwick.updateWithTimestamp(imu, 1000, true) == false);
  ok &= (madgwick.updateWithTimestamp(imu, 1500, true) == true);
  ok &= almostEqual(madgwick.getQuaternion().w * madgwick.getQuaternion().w +
                    madgwick.getQuaternion().x * madgwick.getQuaternion().x +
                    madgwick.getQuaternion().y * madgwick.getQuaternion().y +
                    madgwick.getQuaternion().z * madgwick.getQuaternion().z,
                    1.0);


  puara_gestures::MahonyQuaternionFilter mahony(1.0, 0.0);
  ok &= (mahony.update(imu, true) == false);
  delay(5);
  ok &= (mahony.update(imu, true) == true);
  ok &= almostEqual(mahony.getQuaternion().w * mahony.getQuaternion().w +
                    mahony.getQuaternion().x * mahony.getQuaternion().x +
                    mahony.getQuaternion().y * mahony.getQuaternion().y +
                    mahony.getQuaternion().z * mahony.getQuaternion().z,
                    1.0);

  mahony.reset();
  ok &= almostEqual(mahony.getQuaternion().w, 1.0) &&
        almostEqual(mahony.getQuaternion().x, 0.0) &&
        almostEqual(mahony.getQuaternion().y, 0.0) &&
        almostEqual(mahony.getQuaternion().z, 0.0) &&
        almostEqual(mahony.lastUpdateMicros, 0);

  ok &= (mahony.updateWithTimestamp(imu, 0, true) == false);
  ok &= (mahony.updateWithTimestamp(imu, 1000, true) == false);
  ok &= (mahony.updateWithTimestamp(imu, 1500, true) == true);
  ok &= almostEqual(mahony.getQuaternion().w * mahony.getQuaternion().w +
                    mahony.getQuaternion().x * mahony.getQuaternion().x +
                    mahony.getQuaternion().y * mahony.getQuaternion().y +
                    mahony.getQuaternion().z * mahony.getQuaternion().z,
                    1.0);


  puara_gestures::KalmanQuaternionFilter kalman(0.001, 0.01);
  ok &= (kalman.update(imu, true) == false);
  delay(5);
  ok &= (kalman.update(imu, true) == true);
  ok &= almostEqual(kalman.getQuaternion().w * kalman.getQuaternion().w +
                    kalman.getQuaternion().x * kalman.getQuaternion().x +
                    kalman.getQuaternion().y * kalman.getQuaternion().y +
                    kalman.getQuaternion().z * kalman.getQuaternion().z,
                    1.0);

  kalman.reset();
  ok &= almostEqual(kalman.getQuaternion().w, 1.0) &&
        almostEqual(kalman.getQuaternion().x, 0.0) &&
        almostEqual(kalman.getQuaternion().y, 0.0) &&
        almostEqual(kalman.getQuaternion().z, 0.0) &&
        almostEqual(kalman.lastUpdateMicros, 0);

  ok &= (kalman.updateWithTimestamp(imu, 0, true) == false);
  ok &= (kalman.updateWithTimestamp(imu, 1000, true) == false);
  ok &= (kalman.updateWithTimestamp(imu, 1500, true) == true);
  ok &= almostEqual(kalman.getQuaternion().w * kalman.getQuaternion().w +
                    kalman.getQuaternion().x * kalman.getQuaternion().x +
                    kalman.getQuaternion().y * kalman.getQuaternion().y +
                    kalman.getQuaternion().z * kalman.getQuaternion().z,
                    1.0);

  logResult(ok, name);
}

static void testEmbeddedMagnetometerCalibration() {
  const char* name = "Embedded magnetometer calibration";
  constexpr size_t sampleCount = 24;
  puara_gestures::Coord3D samples[sampleCount];

  for (size_t i = 0; i < sampleCount; ++i) {
    const double longitude = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(sampleCount);
    const double latitude = M_PI * static_cast<double>(i) / static_cast<double>(sampleCount) - M_PI / 2.0;
    double x = std::cos(latitude) * std::cos(longitude);
    double y = std::cos(latitude) * std::sin(longitude);
    double z = std::sin(latitude);

    x = x * 1.25 + 0.22;
    y = y * 0.68 - 0.16;
    z = z * 1.14 + 0.06;
    samples[i] = {x, y, z};
  }

  puara_gestures::utils::Embedded_Magnetometer_Calibration calib(sampleCount);
  bool ok = (calib.generateMagnetometerMatrices(samples, sampleCount) == 1);

  for (size_t i = 0; ok && i < sampleCount; ++i) {
    puara_gestures::Imu9Axis raw{};
    raw.magn = samples[i];
    calib.applyMagnetometerCalibration(raw);
    const double r = std::sqrt(calib.myCalIMU.magn.x * calib.myCalIMU.magn.x +
                              calib.myCalIMU.magn.y * calib.myCalIMU.magn.y +
                              calib.myCalIMU.magn.z * calib.myCalIMU.magn.z);
    ok &= almostEqual(r, 1.0, 0.25);
  }

  logResult(ok, name);
}

static void testRollingMinMax() {
  const char* name = "RollingMinMax sliding range";
  puara_gestures::utils::RollingMinMax<int> window(3);

  bool ok = true;
  auto result1 = window.update(5);
  ok &= (result1.min == 5 && result1.max == 5);

  auto result2 = window.update(3);
  ok &= (result2.min == 3 && result2.max == 5);

  auto result3 = window.update(8);
  ok &= (result3.min == 3 && result3.max == 8);

  auto result4 = window.update(1);
  ok &= (result4.min == 1 && result4.max == 8);

  auto result5 = window.update(9);
  ok &= (result5.min == 1 && result5.max == 9);

  logResult(ok, name);
}

static void testDiscretizer() {
  const char* name = "Discretizer change detection";
  puara_gestures::utils::Discretizer<int> detector;

  bool ok = detector.isNew(10) && (detector.getLatestValue() == 10);
  ok &= !detector.isNew(10);
  ok &= detector.isNew(20) && (detector.getLatestValue() == 20);
  ok &= !detector.isNew(20);
  ok &= detector.isNew(15) && (detector.getLatestValue() == 15);

  logResult(ok, name);
}

static void runEmbeddedTests() {
  Serial.println("=== puara-gestures embedded sanity tests ===");

  testEigen();
  testArrayAverage();
  testBitShiftArrayL();
  testBlobDetector();
  testStructAliases();
  testBoostCircularBuffer();
  testCircularBuffer();
  testMapRange();
  testThreshold();
  testSmooth();
  testRollDescriptor();
  testTiltDescriptor();
  testSimpleTiltRollDescriptor();
  testTouchDescriptor();
  testButtonDescriptor();
  testJabDescriptor();
  testShakeDescriptor();
  testIMUFilters();
  testEmbeddedMagnetometerCalibration();
  testRollingMinMax();
  testDiscretizer();

  Serial.print("Total checks: ");
  Serial.println(g_checks);
  Serial.print("Failures: ");
  Serial.println(g_failures);

  if (g_failures == 0) {
    Serial.println("=== EMBEDDED TESTS PASSED ===");
  } else {
    Serial.println("=== EMBEDDED TESTS FAILED ===");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  runEmbeddedTests();
}

void loop() {
  delay(1000);
}
