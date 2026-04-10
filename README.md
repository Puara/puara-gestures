# Puara Gestures

The `puara-gestures` library provides lightweight gesture and feature extraction tools for motion sensors and touch inputs. It is designed for embedded and real-time applications, with a focus on high-level gesture descriptors and supporting utility helpers.

## What the library includes

- **Gesture descriptors** for motion and touch event detection: shakes, jabs, rolls, tilts, touch brush/rub, and button interactions.
- **Button detection** to classify discrete press/release behavior and user interactions from a digital input.
- **Utility helpers** for smoothing, range mapping, thresholding, wrapping, timing, and sensor fusion support.

## Gestures (descriptors) and Features

- **Shake**: Detects shake movements for accelerometer or general motion data.
- **Jab**: Detects jab-style gestures using acceleration-based motion.
- **Roll**: Measures roll gestures from 9DoF IMU input.
- **Tilt**: Measures tilt gestures from 9DoF IMU input.
- **Touch (Brush/Rub)**: Extracts features for brush/rub and swipe-style touch movement.
- **Button**: Detects presses, releases, and other discrete button interactions.

## Utility helpers and helper functions

The library also includes a set of lightweight utility headers under `include/puara/utils` for common data processing tasks and sensor helpers.

### Included utility headers

- `circularbuffer.h` — fixed-size circular buffer wrapper for embedded-friendly history storage
- `discretizer.h` — detects when a value changes and suppresses repeated identical values
- `leakyintegrator.h` — exponential smoothing and attenuation for noisy sensor signals
- `maprange.h` — scales values from one numeric range to another
- `rollingminmax.h` — computes sliding minimum and maximum values over the last N samples
- `smooth.h` — moving-average smoothing helper for noisy inputs
- `threshold.h` — clamps values to a minimum/maximum range
- `wrap.h` — angle wrapping and unwrapping utilities for periodic data
- `chrono.h` — simple cross-platform timing helpers
- `magnetometerCalibration.h` — magnetometer hard-iron and soft-iron calibration helpers
- `blobDetector.h` — 1D blob detection helper for binary sensor streams

### Useful helper functions in `puara::utils`

- `arrayAverage()` — average a range of elements
- `arrayAverageWithoutZero()` — average non-zero elements only
- `bitShiftArrayL()` — legacy bit-shift helper for older feature extraction code

### Coordinate and unit conversion helpers

The `puara::utils::convert` namespace includes:

- `g_to_ms2()` / `ms2_to_g()`
- `dps_to_rads()` / `rads_to_dps()`
- `gauss_to_utesla()` / `utesla_to_gauss()`
- `spheric_to_cartesian()` / `cartesian_to_spheric()`
- `phased_cartesian_to_spheric()` / `phased_spheric_to_cartesian()`

## Example Projects

A basic example of puara-gestures can be found on the [puara-module-templates](https://github.com/Puara/puara-module-templates/tree/main/basic-gestures) repository.
The included `exampleProjects` directory also contains example projects and is a good starting point for new users. 
See [exampleProjects](/exampleProjects) for details.

## Building the Library

### Prerequisites

- CMake 3.24 or higher
- A compatible C++ compiler
- Arduino IDE (for running the example project on hardware)

### Building

To build the `puara-gestures` library, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/Puara/puara-gestures.git
    cd puara-gestures
    ```
2. Configure the project using CMake:
    ```sh
    cmake -B build
    ```
3. Build the project:
    ```sh
    cmake --build build
    ```

## Usage in PlatformIO

Puara-gestures can be easily included in PlatformIo projects. The recommended method is to manually edit the `platformio.ini` file of the project and add this repository to `lib_deps`. It is recommended to specify the puara-gestures version (in the example below, we will use version 0.2.0): 

```
lib_deps = https://github.com/Puara/puara-gestures.git#v0.2.0
```

If planning to use the 3rd-party library for sensor fusion, it also needs to be manually added:

```
lib_deps = 
	https://github.com/Puara/puara-gestures.git#v0.2.0
	https://github.com/malloch/IMU_Sensor_Fusion.git
```

When using PlatformIO, it is also important to use the **espressif32** platform and **arduino** framework if using an ESP32. 
To use esp-idf, it is recommended to manually copy the `src` and `include` folders into your project rather than using lib_deps.
The reason is that esp-idf will try to compile all cpp files in this project, resulting in include errors.

## Contributing with new gestures

It is easy to contribute new gestures for puara-gestures.
Gestures are organized in single-header files that contain the feature class.
We only need to add a class (header) in the [descriptors folder](include/puara/descriptors) and include it in [gestures.h](include/puara/gestures.h).


## More Info on the research related to [Puara](https://github.com/Puara)

- [SAT](https://www.sat.qc.ca)
- [IDMIL](https://www.idmil.org)

## Licensing

The code in this project is licensed under the MIT license, unless otherwise specified within the file.
