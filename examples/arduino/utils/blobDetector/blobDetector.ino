// Puara Gestures - BlobDetector example
// Detects contiguous groups of active (1) sensors in a 1D binary array.
// In loop(), the touch pattern cycles through a set of predefined states so
// you can watch the blob information change in the Serial Monitor.

#include <Arduino.h>
#include <ArduinoEigen.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

static const int DATA_SIZE = 10;
static const int MAX_BLOBS = 4;

// A few touch patterns to cycle through
static const int patterns[][DATA_SIZE] = {
  {0, 1, 1, 1, 0, 0, 1, 1, 0, 0},  // two blobs
  {1, 1, 0, 0, 0, 0, 0, 1, 1, 1},  // two blobs, different positions
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0},  // two single-sensor blobs
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // one large blob covering everything
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // no touch
};
static constexpr int NUM_PATTERNS = sizeof(patterns) / sizeof(patterns[0]);

static puara_gestures::BlobDetector<MAX_BLOBS> detector;
static int patternIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("BlobDetector example - cycling touch patterns");
}

void loop() {
  // Copy the current pattern into a mutable array
  // You could input real sensor/button data here instead of cycling through patterns
  int touchData[DATA_SIZE];
  for (int i = 0; i < DATA_SIZE; i++) {
    touchData[i] = patterns[patternIndex][i];
  }

  // Print the raw touch array
  Serial.print("Pattern [");
  Serial.print(patternIndex);
  Serial.print("] data: [");
  for (int i = 0; i < DATA_SIZE; i++) {
    Serial.print(touchData[i]);
    if (i < DATA_SIZE - 1) Serial.print(", ");
  }
  Serial.println("]");

  // Run blob detection
  detector.detect1D(touchData, DATA_SIZE);

  // Print results
  Serial.print("  Blob count: ");
  Serial.println(detector.blobCount);
  for (int i = 0; i < detector.blobCount; i++) {
    Serial.print("    Blob ");
    Serial.print(i);
    Serial.print(": start=");
    Serial.print(detector.blobStartPos[i]);
    Serial.print(", size=");
    Serial.print(detector.blobSize[i]);
    Serial.print(", center=");
    Serial.println(detector.blobCenter[i]);
  }
  Serial.println();

  // Advance to the next pattern
  patternIndex = (patternIndex + 1) % NUM_PATTERNS;
  delay(1500);
}
