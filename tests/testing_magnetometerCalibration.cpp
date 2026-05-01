#include <catch2/catch_all.hpp>

#include <filesystem>
#include <rapidcsv.h>
#include <string>
#include <vector>
#include <boost/math/constants/constants.hpp>

#include <puara/utils/calibration.h>

using namespace Catch;
using namespace puara_gestures;
using namespace puara_gestures::utils;

#ifndef M_PI
#define M_PI boost::math::constants::pi<double>()
#endif

// This suite validates the trusted float-recorded magnetometer file only.
// The integer dataset is excluded from the core Catch2 validation so the
// calibration expectations remain aligned with the desired process.

static std::vector<Coord3D> loadMagnetometerCsv(const std::filesystem::path& csvPath)
{
    rapidcsv::Document doc(csvPath.string(), rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams(','));
    std::vector<double> colX = doc.GetColumn<double>("magn_x");
    std::vector<double> colY = doc.GetColumn<double>("magn_y");
    std::vector<double> colZ = doc.GetColumn<double>("magn_z");

    std::vector<Coord3D> points;
    points.reserve(colX.size());
    for (size_t i = 0; i < colX.size(); ++i)
    {
        points.push_back({colX[i], colY[i], colZ[i]});
    }
    return points;
}

static std::vector<Coord3D> makeBiasedScaledSphere(size_t count)
{
    std::vector<Coord3D> samples;
    samples.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        double longitude = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(count);
        double latitude = M_PI * static_cast<double>(i) / static_cast<double>(count) - M_PI / 2.0;
        double x = std::cos(latitude) * std::cos(longitude);
        double y = std::cos(latitude) * std::sin(longitude);
        double z = std::sin(latitude);

        x = x * 1.4 + 0.25;
        y = y * 0.7 - 0.12;
        z = z * 1.2 + 0.08;

        samples.push_back({x, y, z});
    }
    return samples;
}

TEST_CASE("Calibration: generateMagnetometerMatrices returns 0 on empty input", "[calibration]")
{
    Embedded_Magnetometer_Calibration calib;
    auto result = calib.generateMagnetometerMatrices(nullptr, 0);
    REQUIRE(result == 0);
}

TEST_CASE("Calibration: generateMagnetometerMatrices finds bias and scale on synthetic ellipsoid data", "[calibration]")
{
    auto samples = makeBiasedScaledSphere(48);
    Embedded_Magnetometer_Calibration calib(samples.size());

    auto result = calib.generateMagnetometerMatrices(samples.data(), samples.size());
    REQUIRE(result == 1);
    REQUIRE(calib.softIronMatrix.rows() == 3);
    REQUIRE(calib.softIronMatrix.cols() == 3);
    REQUIRE(calib.hardIronBias.size() == 3);
    REQUIRE(calib.enforceRadialEqualization == true);

    double radiusSum = 0.0;
    double radiusSqSum = 0.0;
    for (auto const& p : samples)
    {
        Imu9Axis raw;
        raw.magn = p;
        calib.applyMagnetometerCalibration(raw);
        Eigen::Vector3d calibrated{calib.myCalIMU.magn.x, calib.myCalIMU.magn.y, calib.myCalIMU.magn.z};
        double r = calibrated.norm();
        radiusSum += r;
        radiusSqSum += r * r;
    }

    double meanRadius = radiusSum / static_cast<double>(samples.size());
    double varRadius = radiusSqSum / static_cast<double>(samples.size()) - meanRadius * meanRadius;
    double stdRadius = std::sqrt(varRadius);

    REQUIRE(meanRadius == Approx(1.0).margin(0.25));
    REQUIRE(stdRadius < 0.2);
}

TEST_CASE("Calibration: applyMagnetometerCalibration removes hard-iron bias", "[calibration]")
{
    Embedded_Magnetometer_Calibration calib;
    calib.softIronMatrix = Eigen::MatrixXd::Identity(3, 3);
    calib.hardIronBias << 0.5, -0.5, 1.0;

    Imu9Axis raw;
    raw.magn = {1.5, -1.0, 2.0};

    calib.applyMagnetometerCalibration(raw);

    REQUIRE(calib.myCalIMU.magn.x == Approx(1.0));
    REQUIRE(calib.myCalIMU.magn.y == Approx(-0.5));
    REQUIRE(calib.myCalIMU.magn.z == Approx(1.0));
}

static void verifySphereLikeCalibration(Embedded_Magnetometer_Calibration& calib, std::vector<Coord3D> const& samples)
{
    std::vector<Eigen::Vector3d> calibratedPoints;
    calibratedPoints.reserve(samples.size());
    for (auto const& p : samples)
    {
        Imu9Axis raw;
        raw.magn = p;
        calib.applyMagnetometerCalibration(raw);
        calibratedPoints.emplace_back(calib.myCalIMU.magn.x, calib.myCalIMU.magn.y, calib.myCalIMU.magn.z);
    }

    double radiusSum = 0.0;
    double radiusSqSum = 0.0;
    double radiusMin = std::numeric_limits<double>::infinity();
    double radiusMax = 0.0;
    Eigen::Vector3d mean = Eigen::Vector3d::Zero();

    for (auto const& v : calibratedPoints)
    {
        double r = v.norm();
        radiusSum += r;
        radiusSqSum += r * r;
        radiusMin = std::min(radiusMin, r);
        radiusMax = std::max(radiusMax, r);
        mean += v;
    }

    mean /= static_cast<double>(calibratedPoints.size());
    double meanRadius = radiusSum / static_cast<double>(calibratedPoints.size());
    double varRadius = radiusSqSum / static_cast<double>(calibratedPoints.size()) - meanRadius * meanRadius;
    double stdRadius = std::sqrt(varRadius);

    REQUIRE(mean.norm() < 0.1);
    REQUIRE(meanRadius == Approx(1.0).margin(0.25));
    REQUIRE(stdRadius < 0.12);
    REQUIRE((radiusMax - radiusMin) < 0.55);
}

TEST_CASE("Calibration: generateMagnetometerMatrices with magnetometer_raw_floats.csv yields sphere-like calibrated field", "[calibration][csv]")
{
    std::filesystem::path csvPath = std::filesystem::path(__FILE__).parent_path() / "data" / "magnetometer_raw_floats.csv";
    REQUIRE(std::filesystem::exists(csvPath));

    auto samples = loadMagnetometerCsv(csvPath);
    REQUIRE(samples.size() > 100);

    Embedded_Magnetometer_Calibration calib(samples.size());
    REQUIRE(calib.generateMagnetometerMatrices(samples.data(), samples.size()) == 1);
    REQUIRE(calib.enforceRadialEqualization == true);
    verifySphereLikeCalibration(calib, samples);
}
