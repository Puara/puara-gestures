/**
* @file kalmanQuaternion.h
* @brief Simplified Kalman-style quaternion filter for 9-DoF IMU orientation.
* @see https://github.com/Puara/puara-gestures
* @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
*/
#pragma once

#include <algorithm>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <cstdint>
#include <puara/structs.h>
#include <puara/utils/chrono.h>

/**
 * @class KalmanQuaternionFilter
 * @brief Simplified Kalman-style quaternion filter for 9-DoF IMU orientation.
 *
 * @details
 * This implementation performs gyro-based quaternion prediction and then
 * blends toward an accel/magnetometer-based orientation measurement.
 * It is intentionally compact and similar in usage style to the Madgwick and
 * Mahony filters in this project.
 *
 * Usage:
 * @li The class stores orientation as `puara_gestures::Quaternion`.
 * @li Input data is accepted through `puara_gestures::Imu9Axis`.
 * @li Gyroscope values are assumed to be in radians/sec by default.
 * @li Use `gyroDegrees = true` when gyro values are in degrees/sec.
 *
 * Example:
 * @code
 *   puara_gestures::KalmanQuaternionFilter filter(0.001, 0.01);
 *   puara_gestures::Imu9Axis imu{
 *       {0.0, 0.0, 9.81},
 *       {1.0, 2.0, 3.0},
 *       {0.3, 0.0, 0.5}
 *   };
 *   bool ok = filter.update(imu, true);
 *   if (ok) {
 *       auto q = filter.getQuaternion();
 *       double roll, pitch, yaw;
 *       filter.getEulerDegrees(roll, pitch, yaw);
 *   }
 * @endcode
 */

namespace puara_gestures {

struct KalmanQuaternionFilter {
    // Simplified Kalman-style quaternion fusion.
    // This implementation uses gyro prediction and an adaptive blend
    // toward an accel/magnetometer-based orientation estimate.
    // A true quaternion EKF is much larger; this keeps the interface
    // compact and similar to the other AHRS filters.
    double processNoise{};
    double measurementNoise{};
    Quaternion quaternion{};
    uint64_t lastUpdateMicros{};

    explicit KalmanQuaternionFilter(double processNoise_ = 0.001, double measurementNoise_ = 0.01)
        : processNoise(processNoise_)
        , measurementNoise(measurementNoise_)
        , quaternion{1.0, 0.0, 0.0, 0.0}
        , lastUpdateMicros(0)
    {
        if (processNoise < 0.0) {
            processNoise = 0.0;
        }
        if (measurementNoise < 0.0) {
            measurementNoise = 0.0;
        }
        if (processNoise + measurementNoise <= 0.0) {
            measurementNoise = 1e-6;
        }
    }

    void reset() {
        quaternion = {1.0, 0.0, 0.0, 0.0};
        lastUpdateMicros = 0;
    }

    bool update(const Imu9Axis& imu, bool gyroDegrees = false) {
        return updateWithTimestamp(imu, utils::getCurrentTimeMicroseconds(), gyroDegrees);
    }

    const Quaternion& getQuaternion() const {
        return quaternion;
    }

    void getEulerRadians(double& roll, double& pitch, double& yaw) const {
        const double w = quaternion.w;
        const double x = quaternion.x;
        const double y = quaternion.y;
        const double z = quaternion.z;

        roll = std::atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y));
        pitch = std::asin(std::clamp(2.0 * (w * y - z * x), -1.0, 1.0));
        yaw = std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
    }

    void getEulerDegrees(double& roll, double& pitch, double& yaw) const {
        getEulerRadians(roll, pitch, yaw);
        roll *= RadToDeg;
        pitch *= RadToDeg;
        yaw *= RadToDeg;
    }
  // updateWithTimestamp is exposed for testing with synthetic timestamps, but
  // typical usage is to call update() with real-time data.
    bool updateWithTimestamp(const Imu9Axis& imu, uint64_t currentMicros, bool gyroDegrees) {
        if (currentMicros == 0) {
            return false;
        }
        if (lastUpdateMicros == 0) {
            lastUpdateMicros = currentMicros;
            return false;
        }

        double deltatSeconds = double(currentMicros - lastUpdateMicros) * 1.0e-6;
        lastUpdateMicros = currentMicros;
        return updateInternal(imu, deltatSeconds, gyroDegrees);
    }

private:
    bool updateInternal(const Imu9Axis& imu, double deltatSeconds, bool gyroDegrees) {
        if (deltatSeconds <= 0.0) {
            return false;
        }

        double gx = imu.gyro.x;
        double gy = imu.gyro.y;
        double gz = imu.gyro.z;
        if (gyroDegrees) {
            gx *= DegToRad;
            gy *= DegToRad;
            gz *= DegToRad;
        }

        predictQuaternion(gx, gy, gz, deltatSeconds);

        Quaternion measured;
        if (!estimateQuaternionFromAccelMag(imu.accl.x, imu.accl.y, imu.accl.z,
                                           imu.magn.x, imu.magn.y, imu.magn.z,
                                           measured)) {
            return true;
        }

        double gain = processNoise / (processNoise + measurementNoise);
        gain = std::clamp(gain, 0.0, 1.0);
        quaternion = slerp(quaternion, measured, gain);
        normalizeQuaternion(quaternion);

        return true;
    }

    void predictQuaternion(double gx, double gy, double gz, double deltat) {
        // Gyro integration step: propagate the quaternion forward using angular velocity.
        // This is the prediction phase of the simplified Kalman-style filter.
        double q0 = quaternion.w;
        double q1 = quaternion.x;
        double q2 = quaternion.y;
        double q3 = quaternion.z;

        double qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
        double qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
        double qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
        double qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

        q0 += qDot1 * deltat;
        q1 += qDot2 * deltat;
        q2 += qDot3 * deltat;
        q3 += qDot4 * deltat;

        quaternion.w = q0;
        quaternion.x = q1;
        quaternion.y = q2;
        quaternion.z = q3;
        normalizeQuaternion(quaternion);
    }

    static bool estimateQuaternionFromAccelMag(double ax, double ay, double az,
                                               double mx, double my, double mz,
                                               Quaternion& quaternionOut) {
        // Normalize accelerometer data to extract the gravity direction.
        double norm = std::hypot(ax, ay, az);
        if (norm == 0.0) {
            return false;
        }
        ax /= norm;
        ay /= norm;
        az /= norm;

        // Compute roll and pitch from the gravity vector.
        double roll = std::atan2(ay, az);
        double pitch = std::atan2(-ax, std::hypot(ay, az));

        double sinRoll = std::sin(roll);
        double cosRoll = std::cos(roll);
        double sinPitch = std::sin(pitch);
        double cosPitch = std::cos(pitch);

        // Rotate the magnetometer measurements into the horizontal plane.
        // This removes the effect of tilt and allows yaw to be computed.
        double mx2 = mx * cosPitch + my * sinPitch * sinRoll + mz * sinPitch * cosRoll;
        double my2 = my * cosRoll - mz * sinRoll;
        if (mx2 == 0.0 && my2 == 0.0) {
            return false;
        }

        double yaw = std::atan2(-my2, mx2);
        quaternionOut = quaternionFromEuler(roll, pitch, yaw);
        return true;
    }

    static Quaternion quaternionFromEuler(double roll, double pitch, double yaw) {
        double cr = std::cos(roll * 0.5);
        double sr = std::sin(roll * 0.5);
        double cp = std::cos(pitch * 0.5);
        double sp = std::sin(pitch * 0.5);
        double cy = std::cos(yaw * 0.5);
        double sy = std::sin(yaw * 0.5);

        return Quaternion{
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
        };
    }

    static Quaternion slerp(const Quaternion& a, const Quaternion& b, double t) {
        // Spherical linear interpolation between two quaternion orientations.
        // This blends the predicted orientation and the measured orientation smoothly.
        double cosTheta = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
        Quaternion target = b;
        if (cosTheta < 0.0) {
            target.w = -target.w;
            target.x = -target.x;
            target.y = -target.y;
            target.z = -target.z;
            cosTheta = -cosTheta;
        }

        if (cosTheta > 0.9995) {
            Quaternion result{
                a.w + t * (target.w - a.w),
                a.x + t * (target.x - a.x),
                a.y + t * (target.y - a.y),
                a.z + t * (target.z - a.z)
            };
            normalizeQuaternion(result);
            return result;
        }

        double theta = std::acos(cosTheta);
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
        double aFactor = std::sin((1.0 - t) * theta) / sinTheta;
        double bFactor = std::sin(t * theta) / sinTheta;

        return Quaternion{
            a.w * aFactor + target.w * bFactor,
            a.x * aFactor + target.x * bFactor,
            a.y * aFactor + target.y * bFactor,
            a.z * aFactor + target.z * bFactor
        };
    }

    static void normalizeQuaternion(Quaternion& q) {
        double norm = std::hypot(std::hypot(q.w, q.x), std::hypot(q.y, q.z));
        if (norm == 0.0) {
            q = {1.0, 0.0, 0.0, 0.0};
            return;
        }
        q.w /= norm;
        q.x /= norm;
        q.y /= norm;
        q.z /= norm;
    }

    static constexpr double DegToRad = boost::math::constants::degree<double>();
    static constexpr double RadToDeg = boost::math::constants::radian<double>();
};

} // namespace puara_gestures

