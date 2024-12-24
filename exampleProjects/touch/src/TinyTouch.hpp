#include <algorithm>

#include <puara/gestures.h>

#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ST7789
#include <SPI.h>

/**
 * @class TinyTouch
 * @brief A class to handle touchscreen interactions using the Adafruit ILI9341 and FT6206 libraries.
 *
 * This class is designed to test the Puara gesture framework by providing a simple interface
 * for initializing the screen, displaying introductory messages, detecting touch events, and
 * drawing rectangles on the screen. It uses the Puara library to handle gesture recognition.
 */
class TinyTouch
{
public:
  //======================== TinyPICO pins =======================
  static constexpr int PIN_CS{14};  ///< Chip select pin for the display
  static constexpr int PIN_DC{27};  ///< Data/command pin for the display
  static constexpr int PIN_SDA{25}; ///< Serial data pin for the touchscreen
  static constexpr int PIN_SCL{26}; ///< Serial clock pin for the touchscreen

  /**
   * @brief Initializes the touchscreen and display.
   */
  void initScreen()
  {
    tft.begin();
    screenHeight = tft.height();
    screenWidth = tft.width();
    rectangleHeight = screenHeight / RECT_COUNT;
    Serial.println(F("Touchscreen is initialized"));

    if(!ctp.begin())
    {
      Serial.println("Couldn't start touchscreen controller");
      while(1)
        ;
    }
    Serial.println("touchscreen started");
  }

  /**
   * @brief Prints an introductory message to the screen.
   */
  void printIntro()
  {
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 100);
    tft.println("Puara Gesture Test!");
    tft.setTextSize(1);
    tft.setCursor(10, 130);
    tft.println("Touch the screen to begin.");
  }

  /**
   * @brief Checks if the touchscreen is currently being touched.
   * @return True if the touchscreen is being touched, false otherwise.
   */
  bool touched() { return ctp.touched(); }

  /**
   * @brief Draws rectangles on the screen.
   *
   * This function divides the screen into a series of rectangles and draws them. These
   * discrete rectangles will be used by puara to calculate various touch gestures.
   */
  void drawRectangles()
  {
    // Draw rectangles
    tft.fillScreen(ILI9341_BLACK);
    for(int i = 0; i < RECT_COUNT; i++)
      tft.drawRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
  }

  /**
   * @brief Updates the touch state of rectangles and redraws them on the screen.
   *
   * This function resets the current touch states, checks for new touch inputs,
   * and updates the screen and internal state to reflect changes. Touched
   * rectangles are filled with white, while untapped ones are filled with black
   * and outlined in white.
   *
   * @pre `ctp` must be initialized, and rectangle dimensions must match screen setup.
   * @post The screen reflects the updated touch states.
   */
  void updateRectangles()
  {
    // Reset the current touch array
    std::fill_n(touchedRectangles, RECT_COUNT, 0);

    // Check for touches
    if(ctp.touched())
    {
      TS_Point p{ctp.getPoint()};
      p.x = map(p.x, 0, 240, 240, 0);
      p.y = map(p.y, 0, 320, 320, 0);

      // Determine which rectangle is touched
      const int rectIndex{p.y / rectangleHeight};
      if(rectIndex >= 0 && rectIndex < RECT_COUNT)
        touchedRectangles[rectIndex] = 1;
    }

    // Update the screen based on touch states
    for(int i = 0; i < RECT_COUNT; i++)
    {
      if(touchedRectangles[i] == 1 && previouslyTouchedRectangles[i] == 0)
      {
        // Rectangle is touched - make it white
        tft.fillRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
      }
      else if(touchedRectangles[i] == 0 && previouslyTouchedRectangles[i] == 1)
      {
        // Rectangle no longer touched - make it black
        tft.fillRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_BLACK);
        tft.drawRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
      }
    }

    // Calculate current touch values and cache them for the next loop
    touch.updateTouchArray(touchedRectangles, RECT_COUNT);
    std::copy(touchedRectangles, touchedRectangles + RECT_COUNT, previouslyTouchedRectangles);
  }

  /**
   * @brief Prints the current touch values to the serial monitor.
   */
  void printUpdate()
  {
    if(touch.brush != prev_brush || touch.rub != prev_rub)
    {
      Serial.println("brush;" + String(touch.brush, 6) + ";rub;" + String(touch.rub, 6));
      prev_brush = touch.brush;
      prev_rub = touch.rub;
    }
  }

private:
  //======================== setup screen =======================
  int screenHeight{-1};
  int screenWidth{-1};
  int rectangleHeight{-1};

  Adafruit_ILI9341 tft{TinyTouch::PIN_CS, TinyTouch::PIN_DC};
  Adafruit_FT6206 ctp;

  //======================== setup puara =======================
  static constexpr int RECT_COUNT{16};
  int touchedRectangles[RECT_COUNT] = {0};
  puara_gestures::Touch touch;

  // Keep track of previous values
  int previouslyTouchedRectangles[RECT_COUNT] = {0};
  float prev_brush{-1.0f};
  float prev_rub{-1.0f};
};