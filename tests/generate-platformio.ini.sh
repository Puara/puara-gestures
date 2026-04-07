#!/bin/bash -eu

# Variables
PUARA_GESTURES_COMMIT_HASH=$1

if [[ -z "$PUARA_GESTURES_PATH" ]]; then
  PUARA_GESTURES_PATH="https://github.com/Puara/puara-gestures.git#$PUARA_GESTURES_COMMIT_HASH"
fi

# We'll put the temporary platformio.ini file in tests/platformio/
OUTPUT_FILE="tests/platformio/platformio.ini"

# Write the file
cat <<EOL > "${OUTPUT_FILE}"
[platformio]
description = Embedded Compilation Tests for Puara Gestures Library
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = tinypico
framework = arduino

[env:build_with_3rdparty_libs]
build_flags =
    -I${PROJECT_LIBDEPS_DIR}/3rdparty/eigen
    -I${PROJECT_LIBDEPS_DIR}/3rdparty/IMU_Sensor_Fusion
lib_deps = $PUARA_GESTURES_PATH

[env:build_with_arduino_libs]
build_flags =
    -I${PROJECT_LIBDEPS_DIR}/3rdparty/IMU_Sensor_Fusion
lib_deps = $PUARA_GESTURES_PATH

EOL
