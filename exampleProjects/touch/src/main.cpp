//======================== setup pins =======================
constexpr int TFT_CS{14};
constexpr int TFT_DC{27};
constexpr int pico_SDA{25};
constexpr int pico_SCL{26};

//========================= setup screen =======================
#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ST7789
#include <SPI.h>

Adafruit_ILI9341 tft{TFT_CS, TFT_DC};
Adafruit_FT6206 ctp;

constexpr int RECT_COUNT = 16;
int SCREEN_HEIGHT = -1;
int SCREEN_WIDTH = -1;
int RECT_HEIGHT = -1;

//======================== setup puara-gestures =======================
#include <puara/gestures.h>
puara_gestures::Touch touch;
int discrete_touch[RECT_COUNT] = {0};

//======================== main code =======================
void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("Touch Puara Gesture Test!"));

  Wire.setPins(pico_SDA, pico_SCL);
  Wire.begin();

  tft.begin();
  SCREEN_HEIGHT = tft.height();
  Serial.println(F("Screen is Initialized"));
  SCREEN_WIDTH = tft.width();
  RECT_HEIGHT = SCREEN_HEIGHT / RECT_COUNT;

  Serial.println(F("Touchscreen is initialized"));

  // pass in 'sensitivity' coefficient
  if(!ctp.begin(40))
  {
    Serial.println("Couldn't start touchscreen controller");
    while(1)
      ;
  }
  Serial.println("touchscreen started");

  // Display introductory text
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 100);
  tft.println("Puara Gesture Test!");
  tft.setTextSize(1);
  tft.setCursor(10, 130);
  tft.println("Touch the screen to begin.");

  // Wait for a touch before proceeding
  while(!ctp.touched())
    delay(50);

  // Draw rectangles
  tft.fillScreen(ILI9341_BLACK);
  for(int i = 0; i < RECT_COUNT; i++)
    tft.drawRect(0, i * RECT_HEIGHT, SCREEN_WIDTH, RECT_HEIGHT, ILI9341_WHITE);
}

void updateAndPrintTouch()
{
  static float prev_touchAll = -1.0f; // Initialize with an invalid value
  static float prev_brush = -1.0f;
  static float prev_rub = -1.0f;

  touch.updateTouchArray(discrete_touch, RECT_COUNT);
  if(touch.brush != prev_brush || touch.rub != prev_rub)
  {
    Serial.println("brush;" + String(touch.brush, 6) + ";rub;" + String(touch.rub, 6));
    prev_brush = touch.brush;
    prev_rub = touch.rub;
  }
}

void loop()
{
  static int prev_touch[RECT_COUNT] = {0}; // Keep track of previous touch states

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

  updateAndPrintTouch();
}
