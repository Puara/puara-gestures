#pragma once

#if defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
#include <puara/utils/magnetometerCalibration_embedded.h>
#else
#include <puara/utils/magnetometerCalibration.h>
#endif

#include <puara/utils/magnetometerCalibration.h>