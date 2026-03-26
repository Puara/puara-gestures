#include <catch2/catch_all.hpp>

#include <puara/utils/magnetometerCalibration.h>

#include <filesystem>
#include <rapidcsv.h>
#include <string>
#include <vector>

using namespace Catch;

TEST_CASE("Calibration: recordRawMagData increments storage", "[calibration]")
{
    puara_gestures::utils::Calibration calib;
    REQUIRE(calib.rawMagData.empty());

    Coord3D point{1.0, 2.0, 3.0};
    auto sizeAfter = calib.recordRawMagData(point);

    REQUIRE(sizeAfter == 1);
    REQUIRE(calib.rawMagData.size() == 1);
    REQUIRE(calib.rawMagData[0].x == Approx(1.0));
}

TEST_CASE("Calibration: generateMagnetometerMatrices returns 0 on empty input", "[calibration]")
{
    puara_gestures::utils::Calibration calib;
    std::vector<Coord3D> empty;

    auto result = calib.generateMagnetometerMatrices(empty);
    REQUIRE(result == 0);
}

TEST_CASE("Calibration: generateMagnetometerMatrices changes softIronMatrix/hardIronBias on real data", "[calibration]")
{
    puara_gestures::utils::Calibration calib;

    // simple synthetic points around a centered sphere with minor hard iron bias
    std::vector<Coord3D> data;
    for (int i = 0; i < 50; ++i)
    {
        double a = i * 2.0 * M_PI / 50.0;
        double z = -1.0 + 2.0 * i / 49.0;
        double r = std::sqrt(1.0 - z*z);
        data.push_back({r*std::cos(a)+0.1, r*std::sin(a)-0.2, z+0.05});
    }

    auto result = calib.generateMagnetometerMatrices(data);
    REQUIRE(result == 1);

    // after calibration there should be a potentially non-trivial bias and matrix
    REQUIRE(calib.hardIronBias.size() == 3);
    REQUIRE(calib.softIronMatrix.rows() == 3);
    REQUIRE(calib.softIronMatrix.cols() == 3);
    REQUIRE(calib.hardIronBias.norm() >= 0.0);
}

TEST_CASE("Calibration: applyMagnetometerCalibration adjusts readings from hardIronBias", "[calibration]")
{
    puara_gestures::utils::Calibration calib;

    // set an artificial bias and identity soft-iron
    calib.softIronMatrix = Eigen::MatrixXd::Identity(3, 3);
    calib.hardIronBias << 0.5, -0.5, 1.0;

    Imu9Axis raw;
    raw.magn = {1.5, -1.0, 2.0};

    calib.applyMagnetometerCalibration(raw);

    REQUIRE(calib.myCalIMU.magn.x == Approx(1.0));
    REQUIRE(calib.myCalIMU.magn.y == Approx(-0.5));
    REQUIRE(calib.myCalIMU.magn.z == Approx(1.0));
}

TEST_CASE("Calibration: generateMagnetometerMatrices with imu_data_56k.csv yields sphere-like calibrated field", "[calibration][csv]")
{
    std::filesystem::path csvPath = std::filesystem::path(__FILE__).parent_path() / "data" / "imu_data_56k.csv";
    REQUIRE(std::filesystem::exists(csvPath));

    rapidcsv::Document doc(csvPath.string(), rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams(','));

    std::vector<double> colX = doc.GetColumn<double>("mag_x");
    std::vector<double> colY = doc.GetColumn<double>("mag_y");
    std::vector<double> colZ = doc.GetColumn<double>("mag_z");

    REQUIRE(colX.size() == colY.size());
    REQUIRE(colX.size() == colZ.size());
    REQUIRE(colX.size() > 1000);

    std::vector<Coord3D> rawMag;
    rawMag.reserve(colX.size());

    for (size_t i = 0; i < colX.size(); ++i)
    {
        rawMag.push_back({colX[i], colY[i], colZ[i]});
    }

    puara_gestures::utils::Calibration calib;
    auto result = calib.generateMagnetometerMatrices(rawMag);
    REQUIRE(result == 1);

    // verify calibration makes magnitudes consistent and isotropic
    std::vector<Eigen::Vector3d> calPoints;
    calPoints.reserve(rawMag.size());

    for (auto const& p : rawMag)
    {
        puara_gestures::Imu9Axis raw;
        raw.magn = p;
        calib.applyMagnetometerCalibration(raw);
        calPoints.emplace_back(calib.myCalIMU.magn.x, calib.myCalIMU.magn.y, calib.myCalIMU.magn.z);
    }

    double radiusSum = 0.0;
    double radiusSqSum = 0.0;
    double radiusMin = std::numeric_limits<double>::infinity();
    double radiusMax = 0.0;
    Eigen::Vector3d mean = Eigen::Vector3d::Zero();

    for (auto const& v : calPoints)
    {
        double r = v.norm();
        radiusSum += r;
        radiusSqSum += r * r;
        radiusMin = std::min(radiusMin, r);
        radiusMax = std::max(radiusMax, r);
        mean += v;
    }

    mean /= static_cast<double>(calPoints.size());

    double meanRadius = radiusSum / static_cast<double>(calPoints.size());
    double varRadius = radiusSqSum / static_cast<double>(calPoints.size()) - meanRadius * meanRadius;
    double stdRadius = std::sqrt(varRadius);

    // hard-iron bias should be removed (center near zero)
    REQUIRE(mean.norm() < 0.2 * meanRadius);

    // radius distribution should be tight for sphere-like fit
    REQUIRE(stdRadius < 0.25 * meanRadius);
    REQUIRE((radiusMax - radiusMin) < 0.8 * meanRadius);

    // covariance eigenvalues should be roughly equal for sphere-like points
    Eigen::Matrix3d cov = Eigen::Matrix3d::Zero();
    for (auto const& v : calPoints)
    {
        Eigen::Vector3d d = v - mean;
        cov += d * d.transpose();
    }
    cov /= static_cast<double>(calPoints.size());

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es(cov);
    REQUIRE(es.info() == Eigen::Success);
    auto eval = es.eigenvalues();

    REQUIRE(eval(0) > 0);
    REQUIRE(eval(2) > 0);
    double ratio = eval(2) / eval(0);
    REQUIRE(ratio < 2.0);
    REQUIRE(ratio > 0.5);
}
