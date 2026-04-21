/*
  PlatformIO Embedded Verification Test for puara-gestures

  This test runner converts a small, portable subset of the existing
  Catch2 host tests into an embedded-friendly serial output harness.
*/

#include <Arduino.h>
#include <boost/circular_buffer.hpp>
#include <cmath>
#include <puara/structs.h>
#include <puara/utils.h>
#include <puara/utils/blobDetector.h>
#include <puara/utils/calibration.h>
#include <puara/utils/includeEigen.h>
#include <puara/utils/leakyintegrator.h>


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

  puara_gestures::MahonyQuaternionFilter mahony(1.0, 0.0);
  ok &= (mahony.update(imu, true) == false);
  delay(5);
  ok &= (mahony.update(imu, true) == true);
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

  logResult(ok, name);
}

// static void testEmbeddedMagnetometerCalibration() {
//   const char* name = "Embedded magnetometer calibration";
//   constexpr size_t sampleCount = 24;
//   puara_gestures::Coord3D samples[sampleCount];

//   for (size_t i = 0; i < sampleCount; ++i) {
//     const double longitude = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(sampleCount);
//     const double latitude = M_PI * static_cast<double>(i) / static_cast<double>(sampleCount) - M_PI / 2.0;
//     double x = std::cos(latitude) * std::cos(longitude);
//     double y = std::cos(latitude) * std::sin(longitude);
//     double z = std::sin(latitude);

//     x = x * 1.25 + 0.22;
//     y = y * 0.68 - 0.16;
//     z = z * 1.14 + 0.06;
//     samples[i] = {x, y, z};
//   }

//   puara_gestures::utils::Embedded_Calibration calib(sampleCount);
//   bool ok = (calib.generateMagnetometerMatrices(samples, sampleCount) == 1);

//   for (size_t i = 0; ok && i < sampleCount; ++i) {
//     puara_gestures::Imu9Axis raw{};
//     raw.magn = samples[i];
//     calib.applyMagnetometerCalibration(raw);
//     const double r = std::sqrt(raw.magn.x * raw.magn.x + raw.magn.y * raw.magn.y + raw.magn.z * raw.magn.z);
//     ok &= almostEqual(r, 1.0, 0.25);
//   }

//   logResult(ok, name);
// }

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
  testIMUFilters();
  // testEmbeddedMagnetometerCalibration();
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
