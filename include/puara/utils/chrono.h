//********************************************************************************//
// Puara Gestures - Utilities - Chrono (.h)                                       //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
//                                                                                // 
// This file provides utility functions for high-resolution time measurements     //
// function both on desktop and embedded platforms.                               //
//********************************************************************************//

#pragma once

#include <chrono>

namespace puara_gestures::utils
{

/**
 *  @brief Simple function to get the current elapsed time in microseconds.
 */
inline unsigned long long getCurrentTimeMicroseconds()
{
  auto currentTimePoint = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      currentTimePoint.time_since_epoch());
  return duration.count();
}

}