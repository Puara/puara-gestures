// Puara Gestures - Embedded_Magnetometer_Calibration example
//
// Why calibrate a magnetometer?
// -----------------------------
// A magnetometer measures the Earth's magnetic field to provide an absolute
// heading (yaw) reference — something accelerometers and gyroscopes alone
// cannot supply.  Without it, any orientation filter (Madgwick, Mahony, …)
// will drift in the yaw axis over time with no way to correct itself.
//
// The problem is that the raw readings are almost never a clean measure of
// the Earth's field.  Two categories of distortion corrupt the signal:
//
//   Hard-iron distortion — permanent magnetic fields fixed to the PCB
//     (battery, speaker, ferrite beads, DC motors).  They add a constant
//     DC offset to every reading, shifting the sphere of readings off-centre.
//
//   Soft-iron distortion — nearby ferromagnetic materials (steel enclosures,
//     screws) that bend the field lines.  They deform the sphere of readings
//     into an ellipsoid, stretching or squashing individual axes.
//
// Calibration estimates and removes both effects:
//   1. A hard-iron bias vector is subtracted to re-centre the data.
//   2. A soft-iron scale matrix is applied to restore spherical symmetry.
//
// After calibration the sensor output is a consistent unit sphere regardless
// of the device's orientation, which is what AHRS filters expect.  In
// practice this turns a compass that is 20-40° off into one that is within
// 1-3° — a significant difference for heading-sensitive applications.
//
// Workflow
// --------
//  1. On startup the device reads the magnetometer every loop iteration.
//  2. A long button press (or Serial command 'c') triggers calibration mode.
//  3. While in calibration mode the user slowly rotates the device through
//     all orientations. Raw samples are stored in a fixed array.
//  4. Once MAX_CAL_SAMPLES are collected the calibration matrices are computed
//     (hard-iron bias + soft-iron scale) and stored.
//  5. From that point on every magnetometer reading is corrected before being
//     used - e.g. fed into an AHRS filter.
//
// Hardware wiring (adapt pin numbers to your board)
// --------------------------------------------------
//  - Magnetometer connected via I2C (SDA/SCL)          <- real sensor
//  - Calibration trigger: pin BUTTON_PIN pulled HIGH    <- button to GND
//
// NOTE: The IMU read block is left as a clearly marked stub so you can drop
// in your sensor library (LSM9DS1, BNO080, ICM-42688, …) without restructuring
// the example.  The simulated data block shows what kind of values you would
// substitute with real sensor reads.
//
// Functions defined in puara/utils/calibration.h

#include <Arduino.h>
#include <puara-gestures.h>

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

// Maximum number of magnetometer samples to collect during calibration.
// 512 is a comfortable upper bound for most ESP32/ARM-M4 boards; reduce if
// RAM is tight (e.g. 128 on an AVR Uno).
static constexpr size_t MAX_CAL_SAMPLES = 512;

// Pin that triggers calibration when pulled LOW (connect a push-button
// between this pin and GND; enable the internal pull-up below).
static constexpr int BUTTON_PIN = 0;  // GPIO0 / BOOT button on most ESP32 devkits

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static puara_gestures::utils::Embedded_Magnetometer_Calibration magCal{MAX_CAL_SAMPLES};
static puara_gestures::Coord3D calSamples[MAX_CAL_SAMPLES];
static size_t calSampleCount  = 0;
static bool   calibrationMode = false;
static bool   calibrated      = false;

// Current IMU data (filled either by a real sensor or the simulation stub)
static puara_gestures::Imu9Axis imu;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void startCalibration() {
  calSampleCount  = 0;
  calibrationMode = true;
  Serial.println();
  Serial.println("=== MAGNETOMETER CALIBRATION START ===");
  Serial.println("Slowly rotate the device through all orientations.");
  Serial.print("Collecting up to ");
  Serial.print(MAX_CAL_SAMPLES);
  Serial.println(" samples...");
}

// Called once per loop while calibrationMode == true.
// Stores the current raw magnetometer reading and finishes when the buffer
// is full.
void processCalibration() {
  if (calSampleCount < MAX_CAL_SAMPLES) {
    calSamples[calSampleCount++] = {imu.magn.x, imu.magn.y, imu.magn.z};
    return;  // keep collecting
  }

  // Buffer full - compute the calibration matrices
  Serial.println("Sample collection complete, computing calibration matrices...");

  int result = magCal.generateMagnetometerMatrices(calSamples, calSampleCount);
  calibrationMode = false;

  if (result == 1) {
    calibrated = true;
    Serial.println("Calibration successful.");
    Serial.print("Hard-iron bias: ");
    Serial.print(magCal.hardIronBias(0), 4); Serial.print(", ");
    Serial.print(magCal.hardIronBias(1), 4); Serial.print(", ");
    Serial.println(magCal.hardIronBias(2), 4);
    Serial.println("Soft-iron matrix:");
    for (int r = 0; r < 3; r++) {
      Serial.print("  ");
      for (int c = 0; c < 3; c++) {
        Serial.print(magCal.softIronMatrix(r, c), 4);
        if (c < 2) Serial.print(", ");
      }
      Serial.println();
    }
  } else {
    Serial.println("Calibration FAILED. Retry - ensure you cover all orientations.");
  }
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // -------------------------------------------------------------------------
  // REAL SENSOR INIT - replace this block with your IMU library's begin() call
  // e.g.:
  //   if (!imuSensor.begin()) { Serial.println("IMU not found!"); while(1); }
  // -------------------------------------------------------------------------
  Serial.println("(stub) IMU initialised.");

  Serial.println("Press button or send 'c' over Serial to start calibration.");
}

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------

void loop() {

  // -------------------------------------------------------------------------
  // REAL SENSOR READ - replace this block with actual sensor reads, e.g.:
  //   imu.accl.x = imuSensor.getAccelX();
  //   imu.gyro.x = imuSensor.getGyroX();
  //   imu.magn.x = imuSensor.getMagX();
  //   ...
  //
  // SIMULATED DATA (used here because no physical sensor is connected):
  // We generate a noisy unit-sphere rotation to mimic what a real sensor
  // would produce.  The hard-iron offset (+0.3, -0.2, +0.1) represents
  // the kind of DC bias a real sensor would have.
  // -------------------------------------------------------------------------
  static float angle = 0.0f;
  angle += 0.05f;
  imu.magn.x = cos(angle)        + 0.3 + ((random(0, 100) - 50) * 0.002);
  imu.magn.y = sin(angle)        - 0.2 + ((random(0, 100) - 50) * 0.002);
  imu.magn.z = sin(angle * 0.7f) + 0.1 + ((random(0, 100) - 50) * 0.002);
  // -------------------------------------------------------------------------

  // Trigger calibration via button (active LOW) or 'c' over Serial
  if (digitalRead(BUTTON_PIN) == LOW || (Serial.available() && Serial.read() == 'c')) {
    if (!calibrationMode) startCalibration();
  }

  // While calibrating: collect samples
  if (calibrationMode) {
    processCalibration();
    delay(20);  // ~50 Hz sample rate during calibration
    return;     // skip normal processing until calibration finishes
  }

  // Apply calibration to the raw reading (once we have valid matrices)
  if (calibrated) {
    magCal.applyMagnetometerCalibration(imu);
    imu.magn = magCal.myCalIMU.magn;  // replace raw with corrected value
  }

  // -------------------------------------------------------------------------
  // USE THE (POSSIBLY CALIBRATED) IMU DATA HERE
  // e.g. feed into a filter:
  //   madgwickFilter.update(imu);
  //   madgwickFilter.getEulerDegrees(roll, pitch, yaw);
  // -------------------------------------------------------------------------

  // Print current magnetometer reading at ~2 Hz so the Serial Monitor is readable
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print(calibrated ? "[calibrated]" : "[raw]");
    Serial.print(" mag x="); Serial.print(imu.magn.x, 3);
    Serial.print(" y=");     Serial.print(imu.magn.y, 3);
    Serial.print(" z=");     Serial.println(imu.magn.z, 3);
  }
}
