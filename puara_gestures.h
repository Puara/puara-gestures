//********************************************************************************//
// Puara Gestures - Sensor fusion and gestural descriptors algorithms             //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#pragma once

#include <cmath>

#include <algorithm>
#include <chrono>
#include <deque>

// puara-gesture descriptors
#include "descriptors/IMU_Sensor_Fusion/imu_orientation.h"
#include "descriptors/puara-button.h"
#include "descriptors/puara-jab.h"
#include "descriptors/puara-roll.h"
#include "descriptors/puara-shake.h"
#include "descriptors/puara-tilt.h"
#include "testing_roll.h"
#include "testing_tilt.h"
