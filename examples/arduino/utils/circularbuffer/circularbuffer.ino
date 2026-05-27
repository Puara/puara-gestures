// Puara Gestures - CircularBuffer example
//
// Why use a circular buffer?
// --------------------------
// On a microcontroller, heap memory is scarce and heap fragmentation is a
// real problem: repeatedly pushing/popping from a std::vector or std::deque
// allocates and frees small blocks, which can corrupt the heap over time.
//
// CircularBuffer allocates its storage exactly once at construction and never
// grows beyond that capacity.  When it is full, the oldest value is silently
// overwritten, giving you a constant-memory "sliding window" of the most
// recent N samples — no allocations, no copies, O(1) push.
//
// This makes it the right building block for anything that needs a rolling
// history: smoothing (Smooth), sliding min/max (RollingMinMax), onset
// detection, or any feature that looks at "the last N readings".
// Both Smooth and RollingMinMax in puara-gestures use CircularBuffer
// internally for exactly this reason.

#include <Arduino.h>
#include <boost-embedded-190.h>
#include <puara-gestures.h>

void setup() {
  Serial.begin(115200);

  // Capacity of 4; a fifth push evicts the oldest element
  puara_gestures::utils::CircularBuffer<double> buf(4);

  constexpr double values[] = {10.0, 20.0, 30.0, 40.0, 50.0};
  for (double v : values) {
    buf.add(v);
    Serial.print("Added ");
    Serial.print(v);
    Serial.print(" -> buffer (newest first): ");
    for (size_t i = 0; i < buf.buffer.size(); i++) {
      Serial.print(buf.buffer[i]);
      if (i + 1 < buf.buffer.size()) Serial.print(", ");
    }
    Serial.println();
  }
}

void loop() {}
