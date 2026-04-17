# puara-gestures Test Coverage Checklist

This document summarizes the current test coverage for `puara-gestures` based on the existing `tests` sources and header classes.

## Existing Catch2 test files
- `tests/testing_structs.cpp`
- `tests/test_utils.cpp`
- `tests/testing_blobDetector.cpp`
- `tests/testing_leakyintegrator.cpp`
- `tests/testing_magnetometerCalibration.cpp`

## Coverage status

### Covered / already tested
- [x] `Spherical` alias fields, `.yaw`, `.pitch`, `.r` (`tests/testing_structs.cpp`)
- [x] `Coord1D`, `Coord2D`, `Coord3D` assignable values (`tests/testing_structs.cpp`)
- [x] `DiscreteArray` construction and default values (`tests/testing_structs.cpp`)
- [x] `MinMax<T>` basic behavior (`tests/testing_structs.cpp`)
- [x] `utils::arrayAverage()` range averaging (`tests/test_utils.cpp`)
- [x] `utils::arrayAverageWithoutZero()` zero filtering (`tests/test_utils.cpp`)
- [x] `utils::getCurrentTimeMicroseconds()` monotonic behavior (`tests/test_utils.cpp`)
- [x] `utils::convert` polar/cartesian round trip conversions (`tests/test_utils.cpp`)
- [x] `utils::convert` unit conversion helpers (`tests/test_utils.cpp`)
- [x] `utils::bitShiftArrayL()` shift register behavior (`tests/test_utils.cpp`)
- [x] `BlobDetector` core detection and max-blob behavior (`tests/testing_blobDetector.cpp`)
- [x] `BlobDetector` previous blob tracking (`tests/testing_blobDetector.cpp`)
- [x] `LeakyIntegrator` basic leak integration behavior (`tests/testing_leakyintegrator.cpp`)
- [x] `LeakyIntegrator` timer branch behavior (`tests/testing_leakyintegrator.cpp`)
- [x] `utils::Calibration` raw data recording and empty-input edge case (`tests/testing_magnetometerCalibration.cpp`)
- [x] `utils::Calibration` calibration matrix generation on synthetic data (`tests/testing_magnetometerCalibration.cpp`)
- [x] `utils::Calibration` hard-iron / soft-iron application behavior (`tests/testing_magnetometerCalibration.cpp`)

### Partially covered / existing but not fully exercised
- [x] `utils::convert` has partial coverage; only selected round-trips and unit conversions are tested
- [x] `utils::chrono::getCurrentTimeMicroseconds()` is covered only for monotonicity, not for edge or platform-specific behavior
- [ ] `Calibration` has substantial coverage, but additional branch tests could be added for `enforceRadialEqualization` and edge-case data patterns

### Missing tests / not covered yet

#### `include/puara/structs.h`
- [ ] `Simple_Orientation`
- [ ] `Quaternion`
- [ ] `Imu6Axis`
- [ ] `Imu9Axis`

#### `include/puara/utils`
- [x] `utils::CircularBuffer`
- [x] `utils::Discretizer`
- [x] `utils::MapRange`
- [x] `utils::Smooth`
- [x] `utils::Threshold`
- [x] `utils::Unwrap`
- [x] `utils::Wrap`
- [ ] `utils::Calibration` additional branches and failure modes
- [x] `utils::RollingMinMax` behavior in more scenarios
- [x] `utils::CircularBuffer` size/rotation behavior
- [x] `utils::Discretizer` first-value and repeated-value behavior
- [x] `utils::MapRange` boundary mapping and zero-span handling
- [x] `utils::Smooth` smoothing history and clear behavior
- [x] `utils::Threshold` clamp behavior at thresholds
- [x] `utils::Unwrap` wrap-around transitions
- [x] `utils::Wrap` modulo and boundary handling

#### `include/puara/descriptors`
- [ ] `Button`
- [ ] `Jab`
- [ ] `Jab2D`
- [ ] `Jab3D`
- [ ] `Roll`
- [ ] `Shake`
- [ ] `Shake2D`
- [ ] `Shake3D`
- [ ] `Tilt`
- [ ] `Tilt_Roll`
- [ ] `ValueIntegrator` / `Brush` / `Rub`
- [ ] `BrushRubDetector`
- [ ] `TouchArrayGestureDetector`

#### Umbrella / higher-level integration
- [ ] `gestures.h` integration tests for combined descriptor usage
- [ ] `include/puara/utils.h` header-level utility integration

## Notes
- `tests/testing_roll.cpp`, `tests/testing_tilt.cpp`, and `tests/testing_touch.cpp` are currently standalone example programs, not Catch2 unit tests.
- `tests/CMakeLists.txt` already creates executable targets for several tests; new tests should be added there for automatic `ctest` execution.
- The current checklist is based on the headers present in `include/puara` and the test files currently in `tests/`.

## Suggested next tests
- [ ] Unit tests for `Button` state transitions: press, release, tap, double-tap, hold
- [ ] Unit tests for `Roll` and `Tilt` using synthetic IMU data
- [ ] Unit tests for `TouchArrayGestureDetector` on several touch-array scenarios
- [ ] Unit tests for `Shake`, `Shake2D`, `Shake3D` with example inputs
- [ ] Unit tests for `utils::Threshold`, `utils::Wrap`, `utils::Unwrap`, `utils::Discretizer`
- [ ] Add Catch2 test coverage for `utils::CircularBuffer` and `utils::MapRange`
