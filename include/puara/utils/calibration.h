#pragma once

// Puara Gestures - Magnetometer Calibration

// Min/Max Scaling Calibration 
// Preferred Embedded Calibration
// Computes hard-iron bias from min/max of each axis
// Scales axes to equal length / radius
// Avoids heavy Eigen math and is less CPU/stack heavy hence, embedded-friendly
#include <puara/utils/magnetometerCalibration_MinMaxScaling.h>


// Ellipsoid Fit Calibration
// More accurate calibration that accounts for soft-iron distortion
// Computes hard-iron bias and soft-iron matrix from eigen decomposition of the covariance of the magnetometer points
// More CPU and stack heavy, less embedded-friendly, but more accurate
//#include <puara/utils/magnetometerCalibration_EllipsoidFit.h>

