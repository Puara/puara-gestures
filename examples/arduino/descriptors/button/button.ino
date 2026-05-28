// Puara Gestures - Button example
// Shows how to detect taps, double taps, triple taps, holds, and press counts
// from a physical button using the puara_gestures::Button class.
//
// Two usage patterns are available:
//   - Tied mode   : the Button reads its input from an external variable automatically
//   - Direct mode : call update(value) with your input value each loop
//
// Wiring: connect a button between BUTTON_PIN and GND.
// INPUT_PULLUP means the pin reads HIGH when released and LOW when pressed,
// so we invert the reading with (1 - digitalRead()) below.
//
// Open the Arduino Serial Plotter to see all outputs as live curves.

#include <Arduino.h>
#include <puara-gestures.h>

#define BUTTON_PIN 2

// --- Tied mode ---
// button_data is updated each loop; the Button object reads it automatically.
int button_data = 0;
puara_gestures::Button button{&button_data};

// --- Direct mode (alternative, no tied variable) ---
// puara_gestures::Button button;
// Then call button.update(value) in loop() instead of button.update().

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Optional: tune the detection windows (values are in milliseconds)
  button.threshold     = 1;     // minimum value to count as a press  (default: 1)
  button.countInterval = 200;   // time window to collect tap counts   (default: 200 ms)
  button.holdInterval  = 5000;  // hold duration before hold triggers  (default: 5000 ms)

  Serial.println("press,tap,doubleTap,tripleTap,hold,count,pressTime");
}

void loop() {
  // Read the hardware pin and store it in button_data.
  // INPUT_PULLUP is active-low, so we invert: 1 = pressed, 0 = released.
  button_data = 1 - digitalRead(BUTTON_PIN);
  button.update();  // reads from button_data automatically (tied mode)

  // --- Direct mode equivalent ---
  // button.update(1 - digitalRead(BUTTON_PIN));

  // press      : true while the button is currently held down
  // tap        : 1 for one beat after a single tap is confirmed
  // doubleTap  : 1 for one beat after a double tap is confirmed
  // tripleTap  : 1 for one beat after a triple tap is confirmed
  // hold       : true while the button is held past holdInterval
  // count      : number of presses detected in the current countInterval window
  // pressTime  : duration of the last press in milliseconds
  Serial.print(button.press);      Serial.print(",");
  Serial.print(button.tap);        Serial.print(",");
  Serial.print(button.doubleTap);  Serial.print(",");
  Serial.print(button.tripleTap);  Serial.print(",");
  Serial.print(button.hold);       Serial.print(",");
  Serial.print(button.count);      Serial.print(",");
  Serial.println(button.pressTime);

  delay(10);  // ~100 Hz polling rate
}
