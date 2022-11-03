/**************************************************************************
 *                                                                         *
 * Sensor Fusion code for estimating orientation of Arduino-based IMU      *
 * 2022 Joseph Malloch, Brady Boettcher                                    *
 * Input Devices and Music Interaction Laboratory                          *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU License.                                  *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU License V2 for more details.                                        *
 *                                                                         *
 ***************************************************************************/

#include "imu_orientation.h"
#include <cmath>

// Constants
double PI = 3.141592653589793;
double halfPI = 1.570796326794897;
double twoPI = 6.283185307179586;

ImuOrientation::Quaternion ImuOrientation::getOrientation(Quaternion &prev, Axes& accel, Axes& mag, Axes& gyro, double weight)
{
    // calculate polar representation of accelerometer data
    accel.roll = atan2(accel.z, accel.y);
    accel.magnitude = sqrt(pow(accel.z, 2) + pow(accel.y, 2));
    accel.tilt = atan2(accel.x, accel.magnitude);
    accel.magnitude = sqrt(pow(accel.x, 2) + pow(accel.magnitude, 2));
    accel.magnitude *= 0.00390625;

    // calculate accelerometer quaternions
    Quaternion qar, qat;
    qar.w = cos(accel.roll * 0.5);
    qar.y = sin(accel.roll * 0.5);
    qar.x = qar.z = 0;
    Quaternion qar_inv = qar.inverse();
    qat.w = cos(accel.tilt * 0.5);
    qat.x = sin(accel.tilt * 0.5);
    qat.y = qat.z = 0;
    Quaternion qat_inv = qat.inverse();
    Quaternion qa = qat.multiply(qar);

    // calculate polar representation of magnetometer data
    mag.roll = atan2(mag.z, mag.y) - PI;
    mag.magnitude = sqrt(pow(mag.z, 2) + pow(mag.y, 2));
    mag.tilt = atan2(mag.x, mag.magnitude);

    // calculate magnetometer quaternions
    Quaternion qmr, qmt;
    qmr.w = cos(mag.roll * 0.5);
    qmr.y = sin(mag.roll * 0.5);
    qmr.x = qmr.z = 0;
    qmt.w = cos(mag.tilt * 0.5);
    qmt.x = sin(mag.tilt * 0.5);
    qmt.y = qmt.z = 0;
    Quaternion qm = qmt.multiply(qmr);

    // rotate the magnetometer quaternion
    qm = qm.multiply(qar_inv);
    qm = qm.multiply(qat_inv);

    // extract azimuth
    double azimuth = atan2(qm.x, qm.y) + PI;
    if (qm.w > 0.)
        azimuth += PI;
    azimuth = fmod(azimuth, twoPI) - PI;

    // replace qm with just azimuth
    qm.w = cos(azimuth * 0.5);
    qm.x = qm.y = 0;
    qm.z = sin(azimuth * 0.5);

    // construct quaternion from combined accelerometer and magnetometer azimuth data
    Quaternion qam = qm.multiply(qa);

    // construct quaternion from gyroscope axes
    Quaternion qg;
    qg.w = cos((gyro.x + gyro.y + gyro.z) * 0.5);
    qg.x = sin(gyro.z * 0.5);
    qg.y = sin(gyro.x * 0.5);
    qg.z = sin(gyro.y * 0.5);
    /*
     // calculate ema and emd
     x->average = accel->magnitude * x->weight + x->average * (1 - x->weight);
     float temp = abs(accel->magnitude - x->average);
     x->deviation = temp * x->weight + x->deviation * (1 - x->weight);
     float weight;
     if (accel->magnitude < 0.05)
     weight = 1;
     else {
     weight = (accel->magnitude - 0.004) * 0.045918367346939 + 0.99;
     weight = weight < 0.9999 ? weight : 0.9999;
     }
     */

    // complementary filter:
    // integrate latest gyro quaternion with stored orientation
    Quaternion orientation = prev.multiply(qg);
    // SLERP between stored orientation and new accel + mag estimate
    Quaternion delayed = orientation;
    orientation = qam.slerp(orientation, weight);

    // use the shortest distance from previous orientation
    return delayed.getMinimumDistance(orientation);
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::multiply(Quaternion &q)
{
    Quaternion o;
    o.w = w * q.w - x * q.x - y * q.y - z * q.z;
    o.x = x * q.w + w * q.x + y * q.z - z * q.y;
    o.y = w * q.y - x * q.z + y * q.w + z * q.x;
    o.z = w * q.z + x * q.y - y * q.x + z * q.w;
    return o;
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::inverse()
{
    Quaternion o;
    float norm_squared = w * w + x * x + y * y + z * z;
    if (norm_squared == 0) norm_squared = 0.0000001;
    o.w = w / norm_squared;
    o.x = x * -1 / norm_squared;
    o.y = y * -1 / norm_squared;
    o.z = z * -1 / norm_squared;
    return o;
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::conjugate()
{
    return Quaternion(w, -x, -y, -z);
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::slerp(Quaternion &q, double weight)
{
    Quaternion o;
    double dot = dotProduct(q);
    if (dot > 0.9995) {
        o.w = w + (q.w - w) * weight;
        o.x = x + (q.x - x) * weight;
        o.y = y + (q.y - y) * weight;
        o.z = z + (q.z - z) * weight;
        o.normalize();
        return o;
    }

    if (dot > 1)
        dot = 1;
    else if (dot < -1)
        dot = -1;

    double theta_0 = acos(dot);
    double theta = (0. < theta_0 && theta_0 < halfPI) ? theta_0 * weight : (theta_0 - PI) * weight;

    o.w = q.w - w * dot;
    o.x = q.x - x * dot;
    o.y = q.y - y * dot;
    o.z = q.z - z * dot;

    o.normalize();

    o.w = w * cos(theta) + o.w * sin(theta);
    o.x = x * cos(theta) + o.x * sin(theta);
    o.y = y * cos(theta) + o.y * sin(theta);
    o.z = z * cos(theta) + o.z * sin(theta);
    return o;
}

double ImuOrientation::Quaternion::dotProduct(Quaternion &q)
{
    return w * q.w + x * q.x + y * q.y + z * q.z;
}

void ImuOrientation::Quaternion::normalize()
{
    double norm = sqrt(w * w + x * x + y * y + z * z);
    if (norm == 0)
      norm = 0.0000001;
    w /= norm;
    x /= norm;
    y /= norm;
    z /= norm;
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::normalized()
{
    double norm = sqrt(w * w + x * x + y * y + z * z);
    if (norm == 0)
      norm = 0.0000001;
    return Quaternion(w / norm, x / norm, y / norm, z / norm);
}

ImuOrientation::Quaternion ImuOrientation::Quaternion::getMinimumDistance(Quaternion &q)
{
    Quaternion o = q;
    // use the shortest distance
    if (dotProduct(q) < 0) {
        o.w = o.w * -1;
        o.x = o.x * -1;
        o.y = o.y * -1;
        o.z = o.z * -1;
    }
    return o;
}
