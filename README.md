# Puara Gestures

The `puara-gestures` library provides a set of tools for creating and managing high-level gestural descriptors using data from various sensors such as accelerometers, gyroscopes, and magnetometers. The library includes classes for handling different types of gestures, including shakes, rolls, tilts, jabs, and touches.

## Gestures (descriptors) and Features

- **Shake**: Detects "shake" movements. It is often used on accelerometer data, but it can be applied to any 1 to 3DoF data flow.
- **Jab**: Detects jab movements using accelerometer data.
- **Roll**: Measures roll gestures using 9DoF (IMU data) info from an accelerometer, gyroscope, and magnetometer.
- **Tilt**: Measures tilt gestures using 9DoF (IMU data) info from an accelerometer, gyroscope, and magnetometer.
- **Touch (Brush/Rub)**: Extract features for 1 to 3DoF agnostic data. It is often used to interpret standard touch features (swipe/brush, rub, positional, area, etc.) based on movement input.
- **Button**: Interaction (feature) extraction using a discrete input. It can be used for buttons or any other discrete binary data.

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
