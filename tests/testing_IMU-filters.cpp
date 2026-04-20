#include <catch2/catch_all.hpp>
#include <cmath>
#include <puara/utils.h>
#include <thread>
#include <chrono>

using namespace Catch;

static puara_gestures::Imu9Axis makeStaticImu() {
    return puara_gestures::Imu9Axis{
        {0.0, 0.0, 9.81},   // accel x/y/z
        {1.0, 2.0, 3.0},     // gyro x/y/z
        {0.3, 0.0, 0.5}      // mag x/y/z
    };
}

static bool isQuaternionNormalized(const puara_gestures::Quaternion& q) {
    const double norm = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
    return std::abs(norm - 1.0) < 1e-6;
}

TEST_CASE("Madgwick filter updates and produces a normalized quaternion", "[imu-filters]") {
    auto imu = makeStaticImu();
    puara_gestures::MadgwickQuaternionFilter filter(0.1);

    REQUIRE(filter.update(imu, true) == false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    REQUIRE(filter.update(imu, true) == true);

    const auto& q = filter.getQuaternion();
    REQUIRE(isQuaternionNormalized(q));
}

TEST_CASE("Mahony filter updates and produces a normalized quaternion", "[imu-filters]") {
    auto imu = makeStaticImu();
    puara_gestures::MahonyQuaternionFilter filter(1.0, 0.0);

    REQUIRE(filter.update(imu, true) == false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    REQUIRE(filter.update(imu, true) == true);

    const auto& q = filter.getQuaternion();
    REQUIRE(isQuaternionNormalized(q));
}

TEST_CASE("Kalman filter updates and produces a normalized quaternion", "[imu-filters]") {
    auto imu = makeStaticImu();
    puara_gestures::KalmanQuaternionFilter filter(0.001, 0.01);

    REQUIRE(filter.update(imu, true) == false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    REQUIRE(filter.update(imu, true) == true);

    const auto& q = filter.getQuaternion();
    REQUIRE(isQuaternionNormalized(q));
}
