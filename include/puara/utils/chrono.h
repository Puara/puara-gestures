/**
 * @file chrono.h
 * @brief High-resolution time measurement utilities for cross-platform use.
 * @details
 * This header defines portable helpers for monotonic timestamps in both
 * native and embedded environments.
 * @see https://github.com/Puara/puara-gestures
 * @author Société des Arts Technologiques (SAT) - https://sat.qc.ca
 */
#pragma once

#include <chrono>

namespace puara_gestures::utils
{

/**
 * @brief Get the current elapsed time in microseconds.
 *
 * @details
 * This helper is intended for portable use in both desktop and embedded
 * test code that needs a monotonic timestamp. It returns the time since an
 * arbitrary epoch and is not intended to represent "wall-clock" time.
 *
 * Example:
 * @code{.cpp}
 *   auto start = puara_gestures::utils::getCurrentTimeMicroseconds();
 *   // ... work ...
 *   auto end = puara_gestures::utils::getCurrentTimeMicroseconds();
 *   auto elapsed = end - start;
 * @endcode
 *
 * @return unsigned long long The current time in microseconds.
 */
inline unsigned long long getCurrentTimeMicroseconds()
{
  auto currentTimePoint = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      currentTimePoint.time_since_epoch());
  return duration.count();
}

/**
 * @brief Get the current elapsed time in milliseconds.
 *
 * @details
 * This helper is intended for portable use in both desktop and embedded
 * test code that needs a monotonic timestamp. It returns the time since an
 * arbitrary epoch and is not intended to represent "wall-clock" time.
 *
 * @return unsigned long long The current time in milliseconds.
 */
inline unsigned long long getCurrentTimeMilliseconds()
{
  auto currentTimePoint = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      currentTimePoint.time_since_epoch());
  return duration.count();
}
}
