# puara-gestures Tests

## Host test instructions

To run tests, move into `puara-gestures/tests/` and do:

```bash
mkdir -p build && cd build
cmake ../
cmake --build .
ctest -V
```

## Clean up host build files

When you are done, remove the build folder by moving into the test folder level and deleting `build/`:

```bash
cd ..
rm -rf build
```
## Embedded PlatformIO testing

The CI creates `tests/platformio/platformio.ini` dynamically using `tests/generate-platformio.ini.sh`.
That script defines two embedded test environments:

- `build_with_3rdparty_libs`
  - validates the build when `IMU_Sensor_Fusion` and `Boost` are consumed as a separate third-party dependency.
  - this is the “3rd-party libs” embedded test path.
  - ArduinoEigen is still used as the arduino framework created with platformIO requires an arduino compatible Eigen lib.

- `build_with_arduino_libs`
  - validates the build when the project is consumed with Arduino-style dependencies like `ArduinoEigen` and `boost-embedded-190`.
  - this is the “Arduino libs” embedded test path.

The CI workflow runs both environments separately, so it checks both dependency configurations.

### Run embedded tests locally

```bash
cd /path/to/puara-gestures
bash tests/generate-platformio.ini.sh $(git rev-parse --short HEAD)
cd tests/platformio
platformio run -e build_with_3rdparty_libs
platformio run -e build_with_arduino_libs
```

This preserves the current host test build instructions above while documenting the two PlatformIO test variants used by CI.
