/*
 * MahonyQuaternionFilter
 * ----------------------
 *
 * A lightweight Mahony AHRS filter for 9-DoF IMU data.
 * This implementation uses accelerometer, gyroscope, and magnetometer
 * data to maintain an orientation quaternion. The filter blends gyroscope
 * integration with feedback from accelerometer/magnetometer errors.
 *
 * Usage:
 *   - The class stores orientation as `puara_gestures::Quaternion`.
 *   - Input data is accepted through `puara_gestures::Imu9Axis`.
 *   - Gyroscope values are assumed to be in radians/sec by default.
 *   - Use `gyroDegrees = true` when gyro values are in degrees/sec.
 *
 * Example:
 *   puara_gestures::MahonyQuaternionFilter filter(1.0, 0.0);
 *   puara_gestures::Imu9Axis imu{ {0.0, 0.0, 9.81}, {1.0, 2.0, 3.0}, {0.3, 0.0, 0.5} };
 *   bool ok = filter.update(imu, true);
 */

#pragma once

#ifndef MAHONYQUATERNION_H
#define MAHONYQUATERNION_H

#include <cmath>
#include <cstdint>
#include <puara/structs.h>
#include <puara/utils/chrono.h>

namespace puara_gestures {

struct MahonyQuaternionFilter {
    // Kp and Ki are the proportional and integral gains for the Mahony AHRS.
    // Kp controls fast correction from accelerometer/magnetometer error.
    // Ki accumulates slow gyroscope bias drift correction over time.
    double twoKp;
    double twoKi;

    Quaternion quaternion;
    Quaternion integralFB;
    uint64_t lastUpdateMicros = 0;

    explicit MahonyQuaternionFilter(double kp = 1.0, double ki = 0.0)
        : twoKp(2.0 * kp)
        , twoKi(2.0 * ki)
        , quaternion{1.0, 0.0, 0.0, 0.0}
        , integralFB{0.0, 0.0, 0.0, 0.0}
        , lastUpdateMicros(0)
    {
    }

    void reset() {
        quaternion = {1.0, 0.0, 0.0, 0.0};
        integralFB = {0.0, 0.0, 0.0, 0.0};
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
        pitch = std::asin(clamp(2.0 * (w * y - z * x), -1.0, 1.0));
        yaw = std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
    }

    void getEulerDegrees(double& roll, double& pitch, double& yaw) const {
        getEulerRadians(roll, pitch, yaw);
        roll *= RadToDeg;
        pitch *= RadToDeg;
        yaw *= RadToDeg;
    }

    // updateWithTimestamp allows the caller to provide a synthetic timestamp for testing
    // but user code will typically call update() with real-time data which in turn calls this function.
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
        double gx = imu.gyro.x;
        double gy = imu.gyro.y;
        double gz = imu.gyro.z;
        if (gyroDegrees) {
            gx *= DegToRad;
            gy *= DegToRad;
            gz *= DegToRad;
        }

        return updateQuaternion(gx, gy, gz,
                                imu.accl.x, imu.accl.y, imu.accl.z,
                                imu.magn.x, imu.magn.y, imu.magn.z,
                                deltatSeconds);
    }

    bool updateQuaternion(double gx, double gy, double gz,
                          double ax, double ay, double az,
                          double mx, double my, double mz,
                          double deltat) {
        if (deltat <= 0.0) {
            return false;
        }

        // Current quaternion state is the prior orientation estimate.
        double q0 = quaternion.w;
        double q1 = quaternion.x;
        double q2 = quaternion.y;
        double q3 = quaternion.z;

        double recipNorm;
        double vx, vy, vz;
        double wx, wy, wz;
        double ex, ey, ez;
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

        if (mx == 0.0 && my == 0.0 && mz == 0.0) {
            // Use the IMU-only path when magnetometer data is unavailable.
            return updateQuaternionIMU(gx, gy, gz, ax, ay, az, deltat);
        }

        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        double _2q0mx = 2.0 * q0 * mx;
        double _2q0my = 2.0 * q0 * my;
        double _2q0mz = 2.0 * q0 * mz;
        double _2q1mx = 2.0 * q1 * mx;
        double hx = mx * (0.5 - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2);
        double hy = mx * (q1q2 + q0q3) + my * (0.5 - q1q1 - q3q3) + mz * (q2q3 - q0q1);
        double bx = std::sqrt(hx * hx + hy * hy);
        double bz = mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5 - q1q1 - q2q2);

        // Compute the expected direction of gravity and magnetic flux in body frame
        // as predicted by the current quaternion estimate.
        vx = 2.0 * (q1q3 - q0q2);
        vy = 2.0 * (q0q1 + q2q3);
        vz = q0q0 - q1q1 - q2q2 + q3q3;
        wx = 2.0 * bx * (0.5 - q2q2 - q3q3) + 2.0 * bz * (q1q3 - q0q2);
        wy = 2.0 * bx * (q1q2 - q0q3) + 2.0 * bz * (q0q1 + q2q3);
        wz = 2.0 * bx * (q0q2 + q1q3) + 2.0 * bz * (0.5 - q1q1 - q2q2);

        // Compute the error between measured and estimated directions.
        // The cross products produce a corrective feedback vector in body frame.
        ex = (ay * vz - az * vy) + (my * wz - mz * wy);
        ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
        ez = (ax * vy - ay * vx) + (mx * wy - my * wx);

        if (twoKi > 0.0) {
            // Integral feedback compensates for long-term gyro drift.
            integralFB.x += twoKi * ex * deltat;
            integralFB.y += twoKi * ey * deltat;
            integralFB.z += twoKi * ez * deltat;
            gx += integralFB.x;
            gy += integralFB.y;
            gz += integralFB.z;
        } else {
            integralFB = {0.0, 0.0, 0.0, 0.0};
        }

        gx += twoKp * ex;
        gy += twoKp * ey;
        gz += twoKp * ez;

        double qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
        double qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
        double qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
        double qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

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

    bool updateQuaternionIMU(double gx, double gy, double gz,
                             double ax, double ay, double az,
                             double deltat) {
        if (deltat <= 0.0) {
            return false;
        }

        double q0 = quaternion.w;
        double q1 = quaternion.x;
        double q2 = quaternion.y;
        double q3 = quaternion.z;

        double recipNorm;
        double vx = 2.0 * (q1 * q3 - q0 * q2);
        double vy = 2.0 * (q0 * q1 + q2 * q3);
        double vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

        double ex = (ay * vz - az * vy);
        double ey = (az * vx - ax * vz);
        double ez = (ax * vy - ay * vx);

        if (twoKi > 0.0) {            // Integral feedback compensates for long-term gyro drift.            integralFB.x += twoKi * ex * deltat;
            integralFB.y += twoKi * ey * deltat;
            integralFB.z += twoKi * ez * deltat;
            gx += integralFB.x;
            gy += integralFB.y;
            gz += integralFB.z;
        } else {
            integralFB = {0.0, 0.0, 0.0, 0.0};
        }

        gx += twoKp * ex;
        gy += twoKp * ey;
        gz += twoKp * ez;

        double qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
        double qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
        double qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
        double qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

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

    static double clamp(double value, double lo, double hi) {
        return value < lo ? lo : (value > hi ? hi : value);
    }

    static double invSqrt(double value) {
        return 1.0 / std::sqrt(value);
    }

    static constexpr double DegToRad = 0.017453292519943295;
    static constexpr double RadToDeg = 57.29577951308232;
};

} // namespace puara_gestures

#endif
