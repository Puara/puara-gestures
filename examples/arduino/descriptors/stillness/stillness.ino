// Puara Gestures - Stillness example
// Detects when the device is held still and for how long, using the
// puara_gestures::Stillness descriptor - the inverse of a motion detector.
//
// Feed it an activity signal (e.g. a Shake output or |gyro|). It reports
// when stillness begins, when motion resumes, and flags `settled` once the
// device has been quiet for at least settleTime - handy to freeze a
// parameter, snapshot a pose, or arm a calibration.
//
// Two usage patterns are available:
//   - Tied mode   : the Stillness reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Open the Arduino Serial Plotter to see activity and stillness state.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

// --- Tied mode ---
double activity = 0.0;
puara_gestures::Stillness stillness{&activity};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Stillness stillness;
// Then call stillness.update(value) in loop() instead of stillness.update().

void setup() {
  Serial.begin(115200);

  stillness.threshold  = 0.05;  // activity below this counts as still
  stillness.settleTime = 500;   // ms of quiet before `settled`

  Serial.println("activity,still,settled,onset,moved,stillTime");
}

void loop() {
  activity = readActivity();  // replace with your energy/activity source
  stillness.update();         // reads from activity automatically (tied mode)

  // --- Direct mode equivalent ---
  // stillness.update(readActivity());

  Serial.print(activity);            Serial.print(",");
  Serial.print(stillness.still);     Serial.print(",");
  Serial.print(stillness.settled);   Serial.print(",");
  Serial.print(stillness.onset);     Serial.print(",");
  Serial.print(stillness.moved);     Serial.print(",");
  Serial.println(stillness.stillTime);

  delay(10);  // ~100 Hz
}

// Placeholder: replace with your real activity signal.
double readActivity() {
  return 0.0;
}
