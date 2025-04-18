#include "Arduino.h"

#include <TinyTouch.hpp>

TinyTouch tinyTouch;

void setup(void)
{
  //initialize serial monitoring
  Serial.begin(9600);
  Serial.println(F("Touch Puara Gesture Test!"));

  //initialize i2c
  Wire.setPins(TinyTouch::PIN_SDA, TinyTouch::PIN_SCL);
  Wire.begin();

  tinyTouch.initScreen();
  tinyTouch.printIntro();

  // Wait for a touch before proceeding
  while(!tinyTouch.touched())
    delay(50);

  tinyTouch.drawRectangles();
}

void loop()
{
  tinyTouch.update();
  tinyTouch.printUpdate();
}
