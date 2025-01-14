# Puara Gestures
The `puara-gestures` library provides a set of tools for creating and managing high-level gestural descriptors using data from various sensors such as accelerometers, gyroscopes, and magnetometers. The library includes classes for handling different types of gestures, including shakes, rolls, tilts, jabs, and touches.

## Features

- **Shake Gestures**: Detects shake movements using accelerometer data.
- **Roll Gestures**: Measures roll gestures using 3DoF info from an accelerometer, gyroscope, and magnetometer.
- **Tilt Gestures**: Measures tilt gestures using 3DoF info from an accelerometer, gyroscope, and magnetometer.
- **Jab Gestures**: Detects jab movements using accelerometer data.
- **Touch Gestures**: Interprets touch features based on movement input using a leaky integrator for smoothing.

## Example Projects
The included `exampleProjects` directory contains example projects and is probably the best starting point for new users. See `exampleProjects/README.md` for details.

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
