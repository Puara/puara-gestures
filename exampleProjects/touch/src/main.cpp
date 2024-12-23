#include <puara/gestures.h>

#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ST7789
#include <SPI.h>

struct TouchExample
{

  //======================== setup pins =======================
  static constexpr int TFT_CS{14};
  static constexpr int TFT_DC{27};
  static constexpr int pico_SDA{25};
  static constexpr int pico_SCL{26};

  //======================== setup screen =======================
  static constexpr int RECT_COUNT = 16;
  int SCREEN_HEIGHT = -1;
  int SCREEN_WIDTH = -1;
  int RECT_HEIGHT = -1;

  Adafruit_ILI9341 tft{TouchExample::TFT_CS, TouchExample::TFT_DC};
  Adafruit_FT6206 ctp;

  //setup puara
  int discrete_touch[RECT_COUNT] = {0};
  puara_gestures::Touch touch;

  // Keep track of previous touch states
  int prev_touch[RECT_COUNT] = {0};
  float prev_touchAll{-1.0f}; // Initialize with an invalid value
  float prev_brush{-1.0f};
  float prev_rub{-1.0f};

  void initScreen()
  {
    SCREEN_HEIGHT = tft.height();
    SCREEN_WIDTH = tft.width();
    RECT_HEIGHT = SCREEN_HEIGHT / RECT_COUNT;

    Serial.println(F("Touchscreen is initialized"));
  }

  void updateDiscreteTouch()
  {
    // Reset the current touch array
    std::fill_n(discrete_touch, RECT_COUNT, 0);

    // Check for touches
    if(ctp.touched())
    {
      TS_Point p{ctp.getPoint()};
      p.x = map(p.x, 0, 240, 240, 0);
      p.y = map(p.y, 0, 320, 320, 0);

      // Determine which rectangle is touched
      const int rectIndex{p.y / RECT_HEIGHT};
      if(rectIndex >= 0 && rectIndex < RECT_COUNT)
        discrete_touch[rectIndex] = 1;
    }

    // Update the screen based on touch states
    for(int i = 0; i < RECT_COUNT; i++)
    {
      if(discrete_touch[i] == 1 && prev_touch[i] == 0)
      {
        // Rectangle is touched - make it white
        tft.fillRect(0, i * RECT_HEIGHT, SCREEN_WIDTH, RECT_HEIGHT, ILI9341_WHITE);
      }
      else if(discrete_touch[i] == 0 && prev_touch[i] == 1)
      {
        // Rectangle no longer touched - make it black
        tft.fillRect(0, i * RECT_HEIGHT, SCREEN_WIDTH, RECT_HEIGHT, ILI9341_BLACK);
        tft.drawRect(0, i * RECT_HEIGHT, SCREEN_WIDTH, RECT_HEIGHT, ILI9341_WHITE);
      }
    }

    // Update previous touch state
    std::copy(discrete_touch, discrete_touch + RECT_COUNT, prev_touch);
  }

  void updateAndPrintTouch()
  {
    touch.updateTouchArray(discrete_touch, RECT_COUNT);
    if(touch.brush != prev_brush || touch.rub != prev_rub)
    {
      Serial.println("brush;" + String(touch.brush, 6) + ";rub;" + String(touch.rub, 6));
      prev_brush = touch.brush;
      prev_rub = touch.rub;
    }
  }
};

TouchExample touchExample;

//======================== main code =======================
void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("Touch Puara Gesture Test!"));

  Wire.setPins(TouchExample::pico_SDA, TouchExample::pico_SCL);
  Wire.begin();

  touchExample.tft.begin();

  //TODO: make a function
  touchExample.initScreen();

  // pass in 'sensitivity' coefficient
  if(!touchExample.ctp.begin(40))
  {
    Serial.println("Couldn't start touchscreen controller");
    while(1)
      ;
  }
  Serial.println("touchscreen started");

  // Display introductory text
  touchExample.tft.fillScreen(ILI9341_BLACK);
  touchExample.tft.setTextColor(ILI9341_WHITE);
  touchExample.tft.setTextSize(2);
  touchExample.tft.setCursor(10, 100);
  touchExample.tft.println("Puara Gesture Test!");
  touchExample.tft.setTextSize(1);
  touchExample.tft.setCursor(10, 130);
  touchExample.tft.println("Touch the screen to begin.");

  // Wait for a touch before proceeding
  while(!touchExample.ctp.touched())
    delay(50);

  // Draw rectangles
  touchExample.tft.fillScreen(ILI9341_BLACK);
  for(int i = 0; i < touchExample.RECT_COUNT; i++)
    touchExample.tft.drawRect(
        0, i * touchExample.RECT_HEIGHT, touchExample.SCREEN_WIDTH,
        touchExample.RECT_HEIGHT, ILI9341_WHITE);
}

void loop()
{
  touchExample.updateDiscreteTouch();
  touchExample.updateAndPrintTouch();
}
