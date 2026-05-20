/**
 * @file calibration.h
 * @brief Wrapper header for magnetometer calibration utilities.
 *
 * @details
 * This header includes the embedded-friendly min/max scaling calibration
 * implementation. The min/max algorithm computes hard-iron bias from axis
 * extrema and scales each axis to equal radius without requiring heavy Eigen
 * dependency usage.
 *
 * @note
 * An optional ellipsoid-fit calibration method is documented here but is
 * intentionally excluded until its implementation has been fully validated
 * for release.
 */
#pragma once
#include <puara/utils/includeEigen.h>
#if defined(PUARA_HAS_EIGEN)
  #include <puara/utils/magnetometerCalibration_MinMaxScaling.h>
#endif


// Optional ellipsoid-fit calibration implementation.
// This code path is currently omitted pending validation and release readiness.
// #include <puara/utils/magnetometerCalibration_EllipsoidFit.h>
