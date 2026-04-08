/*
  PlatformIO Embedded Verification Test for puara-gestures

  This test runner converts a small, portable subset of the existing
  Catch2 host tests into an embedded-friendly serial output harness.
*/

#include <Arduino.h>
#include <cmath>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <boost/circular_buffer.hpp>

#include <puara/utils.h>
#include <puara/utils/blobDetector.h>
#include <puara/utils/leakyintegrator.h>
#include <puara/structs.h>

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
  ok &= (mapper.range(5) == 5.0);
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
