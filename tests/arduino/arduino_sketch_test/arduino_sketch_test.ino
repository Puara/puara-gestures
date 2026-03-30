/*
  Arduino Compilation Test for puara-gestures
  This sketch verifies that the library compiles correctly on Arduino platforms
  and that dependencies (ArduinoEigen, boost-embedded-190) are available.
  
  This is NOT a functional test - it's purely a compilation verification.
*/

#include <ArduinoEigen.h>
#include <ArduinoEigenDense.h>
#include <ArduinoEigenSparse.h>
#include <boost-embedded-190.h>

// Include puara-gestures headers
#include <puara/gestures.hpp>

void setup() {
  Serial.begin(9600);
  
  // Verify basic type instantiation
  // This ensures the library headers parse correctly
  Eigen::Matrix3f mat = Eigen::Matrix3f::Identity();
  
  Serial.println("puara-gestures compilation test passed!");
  Serial.println("Library headers verified on target platform.");
}

void loop() {
  // Empty - this is a compilation test only
  delay(1000);
}
