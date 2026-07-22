# Puara Gestures

Puara Gestures is a lightweight C++ library for turning sensor data into useful motion features.
It is designed for embedded systems and real-time projects that need gesture-style signals from accelerometers, IMUs, touch arrays, and buttons.

## What this library gives you

- `Jab`, `Jab2D`, `Jab3D` — simple motion burst detectors for 1, 2, or 3 axes.
- `Shake`, `Shake2D`, `Shake3D` — smooth motion energy tracking for vibration and shaking.
- `Tilt` and `Roll` — orientation signals from 9DoF IMU data.
- `Tilt_Roll` — fast roll/tilt computation using accelerometer data only.
- `TouchArrayGestureDetector` — brush/rub and swipe-style touch features for sensor arrays.
- `Button` — tap, double-tap, hold and press tracking from digital button input.
- `utils/` — reusable helpers for smoothing, thresholds, mapping, timing, and sensor support.

## Why it is useful

This library is made for people who want meaningful sensor features, not raw numbers.
Instead of reading raw acceleration or touch values, you can get:

- a jab intensity score
- shake energy that grows with movement and decays smoothly
- tilt and roll values ready for gesture use
- touch brush/rub metrics
- button interactions like taps and holds

## Quick examples

### 1D Jab

```cpp
double accel = 0.0;
puara_gestures::Jab jab(&accel);
jab.threshold = 3.0;

accel = readAccelerationX();
jab.update();
double score = jab.current_value();
```

### 2D Jab

```cpp
puara_gestures::Coord2D accel{0.0, 0.0};
puara_gestures::Jab2D jab2d(&accel);
jab2d.threshold(2.0);

accel.x = readAccelerationX();
accel.y = readAccelerationY();
jab2d.update();
auto score = jab2d.current_value();
// score.x and score.y are per-axis jab values
```

### 3D Shake

```cpp
puara_gestures::Coord3D accel{0.0, 0.0, 0.0};
puara_gestures::Shake3D shake3d(&accel);
shake3d.threshold(0.2);
shake3d.frequency(0);

accel.x = readAccelerationX();
accel.y = readAccelerationY();
accel.z = readAccelerationZ();
shake3d.update();
auto energy = shake3d.current_value();
```

### Tilt/Roll from accelerometer only

```cpp
puara_gestures::Tilt_Roll simple;
simple.update(0.0, 0.0, 1.0);
double roll = simple.current_roll_value();
double tilt = simple.current_tilt_value();
```

### Button interaction

```cpp
puara_gestures::Button button;
button.countInterval = 300;
button.holdInterval = 500;

button.update(readDigitalInput());
if (button.tap) {
    // tap detected
}
if (button.hold) {
    // hold detected
}
```

## Utilities

The `include/puara/utils` folder contains small helpers for sensor and data processing tasks.

- `rollingminmax.h` — sliding min/max over a short window
- `leakyintegrator.h` — smooth decay and signal energy tracking
- `maprange.h` — scale one numeric range into another
- `smooth.h` — moving average smoothing
- `threshold.h` — clamp values inside a range
- `schmitttrigger.h` — two-threshold hysteresis gate for chatter-free on/off
- `wrap.h` — angle wrapping utilities
- `discretizer.h` — detect value changes
- `circularbuffer.h` — fixed-size history storage

## Build

### Local build

```sh
git clone https://github.com/Puara/puara-gestures.git
cd puara-gestures
cmake -B build
cmake --build build
```

### PlatformIO

Add this repo to `lib_deps`:

```ini
lib_deps =
  https://github.com/Puara/puara-gestures.git#v0.2.0
```

For IMU sensor fusion support also add:

```ini
lib_deps =
  https://github.com/Puara/puara-gestures.git#v0.2.0
  https://github.com/malloch/IMU_Sensor_Fusion.git
```

> For ESP32 + Arduino use `espressif32` and `arduino`.
> ESP-IDF may require copying `src` and `include` manually.

## Testing

This repo includes host Catch2 tests in `tests/` and an embedded PlatformIO test runner in `tests/platformio/src/main.cpp`.

## Contributing

Add a new gesture header to `include/puara/descriptors` and include it from `include/puara/gestures.h`.

## License

MIT, unless a file says otherwise.
