//********************************************************************************//
// Puara Gestures - Embedded Magnetometer Calibration (.h)
// https://github.com/Puara/puara-gestures
// Société des Arts Technologiques (SAT) - https://sat.qc.ca
//********************************************************************************//

#pragma once

#include <puara/structs.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>


#if defined(Arduino_h)
  #include <ArduinoEigen.h>
#else
 #include <Eigen/Core>
 #include <Eigen/Dense>
#endif

namespace puara_gestures::utils
{

/**
 * @brief Embedded-friendly magnetometer calibration helper.
 *
 * This implementation preserves the same public API as the desktop
 * calibration path, but avoids heavy dynamic Eigen workloads by using a
 * simplified hard-iron / soft-iron estimate and a fixed sample count.
 */
class Embedded_Calibration
{
public:
  static constexpr size_t MaxSamples = 512;

  Imu9Axis myCalIMU;
  std::vector<Coord3D> rawMagData;
  Eigen::MatrixXd softIronMatrix;
  Eigen::VectorXd hardIronBias;
  bool enforceRadialEqualization = false;
  double calibrationRadius = 1.0;
  size_t maxSamples = MaxSamples;

  int gravitationField
      = 234; // should be 1000 by default, this was calculated for the LSM9DS1 in Montreal

  Embedded_Calibration()
      : rawMagData()
      , softIronMatrix(3, 3)
      , hardIronBias(3)
      , enforceRadialEqualization(false)
      , calibrationRadius(1.0)
      , maxSamples(MaxSamples)
  {
    rawMagData.reserve(maxSamples);
    softIronMatrix.setIdentity();
    hardIronBias.setZero();
  }

  explicit Embedded_Calibration(size_t sampleCount)
      : rawMagData()
      , softIronMatrix(3, 3)
      , hardIronBias(3)
      , enforceRadialEqualization(false)
      , calibrationRadius(1.0)
      , maxSamples(sampleCount > 0 ? sampleCount : 1)
  {
    rawMagData.reserve(maxSamples);
    softIronMatrix.setIdentity();
    hardIronBias.setZero();
  }

  void applyMagnetometerCalibration(const Imu9Axis& myRawIMU)
  {
    myCalIMU.magn.x = softIronMatrix(0, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(0, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(0, 2) * (myRawIMU.magn.z - hardIronBias(2));

    myCalIMU.magn.y = softIronMatrix(1, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(1, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(1, 2) * (myRawIMU.magn.z - hardIronBias(2));

    myCalIMU.magn.z = softIronMatrix(2, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(2, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(2, 2) * (myRawIMU.magn.z - hardIronBias(2));

    // Optional radial equalization to enforce a consistent magnitude across orientations
    // based off calibrationRadius.
    if(enforceRadialEqualization)
    {
      Eigen::Vector3d calibrated{
          myCalIMU.magn.x,
          myCalIMU.magn.y,
          myCalIMU.magn.z,
      };
      double r = calibrated.norm();
      if(r > std::numeric_limits<double>::epsilon())
      {
        double scale = calibrationRadius / r;
        calibrated *= scale;
        myCalIMU.magn.x = calibrated(0);
        myCalIMU.magn.y = calibrated(1);
        myCalIMU.magn.z = calibrated(2);
      }
    }
  }

  int generateMagnetometerMatrices(const Coord3D* samples, size_t count)
  {
    if(samples == nullptr || count == 0)
    {
      return 0;
    }

    const size_t sampleCount = std::min(count, maxSamples);

    Eigen::Vector3d minVals(
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d maxVals(
        -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity());

    for(size_t i = 0; i < sampleCount; ++i)
    {
      const Eigen::Vector3d sample(samples[i].x, samples[i].y, samples[i].z);
      minVals = minVals.cwiseMin(sample);
      maxVals = maxVals.cwiseMax(sample);
    }

    Eigen::Vector3d bias = (minVals + maxVals) * 0.5;
    Eigen::Vector3d range = (maxVals - minVals).cwiseMax(1e-6);

    Eigen::Vector3d invRange = range.cwiseInverse();
    double meanInvRange = invRange.mean();
    Eigen::Vector3d normalizedScale = invRange / meanInvRange;

    Eigen::Matrix3d soft = normalizedScale.asDiagonal();

    double radiusSum = 0.0;
    for(size_t i = 0; i < sampleCount; ++i)
    {
      Eigen::Vector3d corrected{
          samples[i].x - bias(0),
          samples[i].y - bias(1),
          samples[i].z - bias(2),
      };
      radiusSum += (soft * corrected).norm();
    }

    double meanRadius = radiusSum / static_cast<double>(sampleCount);
    if(meanRadius <= std::numeric_limits<double>::epsilon())
    {
      return 0;
    }

    //soft *= static_cast<double>(gravitationField) / meanRadius;
    soft *= calibrationRadius / meanRadius;
    hardIronBias = bias;
    softIronMatrix = soft;

    enforceRadialEqualization = true;
    //calibrationRadius = static_cast<double>(gravitationField);

    return 1;
  }

};

}
