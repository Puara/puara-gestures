/*
 * MadgwickQuaternionFilter
 * ------------------------
 *
 * This header defines a lightweight Madgwick AHRS filter that consumes 9-DoF
 * IMU readings and produces a normalized quaternion representing orientation.
 *
 * It is based on the Madgwick AHRS algorithm implementation from the
 * x-io.co.uk open-source IMU/AHRS code. The filter uses accelerometer,
 * gyroscope and magnetometer data to fuse a stable quaternion estimate.
 *
 * Usage:
 *   - The class stores orientation as `puara_gestures::Quaternion`.
 *   - Input data is accepted through `puara_gestures::Imu9Axis`.
 *   - By default, gyroscope data is expected in radians/sec.
 *   - Use `gyroDegrees = true` when gyro values are in degrees/sec.
 *
 * Example:
 *
 *   #include <puara/descriptors/madgwickQuaternion.h>
 *
 *   void example() {
 *       puara_gestures::MadgwickQuaternionFilter filter(0.1);
 *       puara_gestures::Imu9Axis imu{
 *           {0.0, 0.0, 9.81},   // accel x/y/z
 *           {1.0, 2.0, 3.0},     // gyro x/y/z
 *           {0.3, 0.0, 0.5}      // mag x/y/z
 *       };
 *
 *       bool success = filter.update(imu, true);
 *
 *       if (success) {
 *           auto q = filter.getQuaternion();
 *           double roll, pitch, yaw;
 *           filter.getEulerDegrees(roll, pitch, yaw);
 *       }
 *   }
 */

#ifndef MADGWICKQUATERNION_H
#define MADGWICKQUATERNION_H

#include <cmath>
#include <cstdint>
#include <puara/structs.h>
#include <puara/utils/chrono.h>

namespace puara_gestures {

struct MadgwickQuaternionFilter {
    // beta : Fusion gain for the Madgwick algorithm.
    // Larger values make the filter correct drift faster,
    // while smaller values make the output smoother.
    // Default is 0.1.
    double beta;
    Quaternion quaternion;
    uint64_t lastUpdateMicros = 0;

    explicit MadgwickQuaternionFilter(double beta_ = 0.1)
        : beta(beta_), quaternion{1.0, 0.0, 0.0, 0.0}, lastUpdateMicros(0)
    {
    }

    void reset() {
        quaternion = {1.0, 0.0, 0.0, 0.0};
        lastUpdateMicros = 0;
    }

    // Simple public entry point using the portable timer.
    // The filter computes the elapsed interval automatically.
    bool update(const Imu9Axis& imu, bool gyroDegrees = false) {
        return updateWithTimestamp(imu, utils::getCurrentTimeMicroseconds(), gyroDegrees);
    }

private:
    bool updateWithTimestamp(const Imu9Axis& imu, uint64_t currentMicros, bool gyroDegrees = false) {
        if (currentMicros == 0) {
            return false;
        }
        if (lastUpdateMicros == 0) {
            lastUpdateMicros = currentMicros;
            return false; // first sample only sets the baseline time.
        }

        double deltatSeconds = double(currentMicros - lastUpdateMicros) * 1.0e-6;
        lastUpdateMicros = currentMicros;
        return updateInternal(imu, deltatSeconds, gyroDegrees);
    }

    bool updateInternal(const Imu9Axis& imu, double deltatSeconds, bool gyroDegrees = false) {
        double gx = imu.gyro.x;
        double gy = imu.gyro.y;
        double gz = imu.gyro.z;
        if (gyroDegrees) {
            gx *= DegToRad;
            gy *= DegToRad;
            gz *= DegToRad;
        }
        return update(gx, gy, gz,
                      imu.accl.x, imu.accl.y, imu.accl.z,
                      imu.magn.x, imu.magn.y, imu.magn.z,
                      deltatSeconds);
    }

public:
    const Quaternion& getQuaternion() const {
        return quaternion;
    }

    void getEulerRadians(double& roll, double& pitch, double& yaw) const {
        const double w = quaternion.w;
        const double x = quaternion.x;
        const double y = quaternion.y;
        const double z = quaternion.z;

        roll = std::atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y));
        pitch = std::asin(clamp(2.0 * (w * y - z * x), -1.0, 1.0));
        yaw = std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
    }

    void getEulerDegrees(double& roll, double& pitch, double& yaw) const {
        getEulerRadians(roll, pitch, yaw);
        roll *= RadToDeg;
        pitch *= RadToDeg;
        yaw *= RadToDeg;
    }

private:
    static constexpr double DegToRad = 0.017453292519943295;
    static constexpr double RadToDeg = 57.29577951308232;

    static double clamp(double value, double lo, double hi) {
        return value < lo ? lo : (value > hi ? hi : value);
    }

    static double invSqrt(double value) {
        return 1.0 / std::sqrt(value);
    }

    bool update(double gx, double gy, double gz,
                double ax, double ay, double az,
                double mx, double my, double mz,
                double deltat) {
        if (deltat <= 0.0) {
            return false;
        }

        if (mx == 0.0 && my == 0.0 && mz == 0.0) {
            return updateIMU(gx, gy, gz, ax, ay, az, deltat);
        }

        if ((ax == 0.0 && ay == 0.0 && az == 0.0) ||
            (mx == 0.0 && my == 0.0 && mz == 0.0)) {
            return false;
        }

        double q0 = quaternion.w;
        double q1 = quaternion.x;
        double q2 = quaternion.y;
        double q3 = quaternion.z;

        double recipNorm;
        double s0, s1, s2, s3;
        double qDot1, qDot2, qDot3, qDot4;
        double hx, hy, _2bx, _2bz;
        double _2q0mx, _2q0my, _2q0mz, _2q1mx;
        double _2q0 = 2.0 * q0;
        double _2q1 = 2.0 * q1;
        double _2q2 = 2.0 * q2;
        double _2q3 = 2.0 * q3;
        double _2q0q2 = 2.0 * q0 * q2;
        double _2q0q3 = 2.0 * q0 * q3;
        double _2q1q2 = 2.0 * q1 * q2;
        double _2q1q3 = 2.0 * q1 * q3;
        double _2q2q3 = 2.0 * q2 * q3;
        double q0q0 = q0 * q0;
        double q0q1 = q0 * q1;
        double q0q2 = q0 * q2;
        double q0q3 = q0 * q3;
        double q1q1 = q1 * q1;
        double q1q2 = q1 * q2;
        double q1q3 = q1 * q3;
        double q2q2 = q2 * q2;
        double q2q3 = q2 * q3;
        double q3q3 = q3 * q3;

        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        _2q0mx = 2.0 * q0 * mx;
        _2q0my = 2.0 * q0 * my;
        _2q0mz = 2.0 * q0 * mz;
        _2q1mx = 2.0 * q1 * mx;
        hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1mx * q3 - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
        _2bx = std::sqrt(hx * hx + hy * hy);
        _2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
        _2bx *= 2.0;
        _2bz *= 2.0;

        double _4bx = 2.0 * _2bx;
        double _4bz = 2.0 * _2bz;

        s0 = -_2q2 * (2.0 * q1q3 - _2q0q2 - ax)
             + _2q1 * (2.0 * q0q1 + _2q2q3 - ay)
             - _2bz * q2 * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
             + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
             + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);

        s1 = _2q3 * (2.0 * q1q3 - _2q0q2 - ax)
             + _2q0 * (2.0 * q0q1 + _2q2q3 - ay)
             - 4.0 * q1 * (1.0 - 2.0 * q1q1 - 2.0 * q2q2 - az)
             + _2bz * q3 * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
             + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
             + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);

        s2 = -_2q0 * (2.0 * q1q3 - _2q0q2 - ax)
             + _2q3 * (2.0 * q0q1 + _2q2q3 - ay)
             - 4.0 * q2 * (1.0 - 2.0 * q1q1 - 2.0 * q2q2 - az)
             + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
             + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
             + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);

        s3 = _2q1 * (2.0 * q1q3 - _2q0q2 - ax)
             + _2q2 * (2.0 * q0q1 + _2q2q3 - ay)
             + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5 - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx)
             + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my)
             + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5 - q1q1 - q2q2) - mz);

        {
            const double sNorm = s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3;
            if (sNorm > 0.0) {
                recipNorm = invSqrt(sNorm);
                s0 *= recipNorm;
                s1 *= recipNorm;
                s2 *= recipNorm;
                s3 *= recipNorm;
            }
        }

        qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz) - beta * s0;
        qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy) - beta * s1;
        qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx) - beta * s2;
        qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx) - beta * s3;

        q0 += qDot1 * deltat;
        q1 += qDot2 * deltat;
        q2 += qDot3 * deltat;
        q3 += qDot4 * deltat;

        recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
        quaternion.w = q0 * recipNorm;
        quaternion.x = q1 * recipNorm;
        quaternion.y = q2 * recipNorm;
        quaternion.z = q3 * recipNorm;

        return true;
    }

    bool updateIMU(double gx, double gy, double gz,
                   double ax, double ay, double az,
                   double deltat) {
        if (deltat <= 0.0) {
            return false;
        }
        if (ax == 0.0 && ay == 0.0 && az == 0.0) {
            return false;
        }

        double q0 = quaternion.w;
        double q1 = quaternion.x;
        double q2 = quaternion.y;
        double q3 = quaternion.z;

        double recipNorm;
        double s0, s1, s2, s3;
        double qDot1, qDot2, qDot3, qDot4;
        double _2q0 = 2.0 * q0;
        double _2q1 = 2.0 * q1;
        double _2q2 = 2.0 * q2;
        double _2q3 = 2.0 * q3;
        double _4q0 = 4.0 * q0;
        double _4q1 = 4.0 * q1;
        double _4q2 = 4.0 * q2;
        double _8q1 = 8.0 * q1;
        double _8q2 = 8.0 * q2;
        double q0q0 = q0 * q0;
        double q1q1 = q1 * q1;
        double q2q2 = q2 * q2;
        double q3q3 = q3 * q3;

        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
        qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
        qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
        qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0 * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0 * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0 * q1q1 * q3 - _2q1 * ax + 4.0 * q2q2 * q3 - _2q2 * ay;

        {
            const double sNorm = s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3;
            if (sNorm > 0.0) {
                recipNorm = invSqrt(sNorm);
                s0 *= recipNorm;
                s1 *= recipNorm;
                s2 *= recipNorm;
                s3 *= recipNorm;
            }
        }

        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;

        q0 += qDot1 * deltat;
        q1 += qDot2 * deltat;
        q2 += qDot3 * deltat;
        q3 += qDot4 * deltat;

        recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
        quaternion.w = q0 * recipNorm;
        quaternion.x = q1 * recipNorm;
        quaternion.y = q2 * recipNorm;
        quaternion.z = q3 * recipNorm;

        return true;
    }
};

} // namespace puara_gestures

#endif