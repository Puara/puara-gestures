#include <algorithm>

#include <puara/gestures.h>

#include <Adafruit_FT6206.h>  // Touchscreen library
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ILI9341.h> // Display library
#include <SPI.h>

/**
 * @class TinyTouch
 * @brief A class to demonstrate how to detect gestures on a touch-array like device (here using
 *        a capacitive touchscreen available in Wokwi) ran on a TinyPICO board.
 *
 * This class is designed to demonstrate how to use the Puara gesture framework by providing a
 * simple interface for displaying an introductory message, drawing rectangles on the screen to
 * simulate a touch array, and detecting touch events on this array. It uses the
 * TouchArrayGestureDetector to recognise gestures.
 *
 * The associated wokwi diagram has one `board-ili9341-cap-touch` touch screen device, which is
 * split into its display and touch components in class members `Adafruit_ILI9341 display`
 * and `Adafruit_FT6206 touchScreen`.
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
    //initialize the display
    display.begin();
    screenHeight = display.height();
    screenWidth = display.width();
    rectangleHeight = screenHeight / RECT_COUNT;
    Serial.println(F("Touchscreen is initialized"));

    //initialize the touchscreen
    if(!touchScreen.begin())
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
    display.fillScreen(ILI9341_BLACK);
    display.setTextColor(ILI9341_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 100);
    display.println("Puara Gesture Test!");
    display.setTextSize(1);
    display.setCursor(10, 130);
    display.println("Touch the screen to begin.");
  }

  /**
   * @brief Checks if the touchscreen is currently being touched.
   * @return True if the touchscreen is being touched, false otherwise.
   */
  bool touched() { return touchScreen.touched(); }

  /**
   * @brief Draws rectangles on the screen.
   *
   * This function divides the screen into a series of rectangles and draws them. These
   * discrete rectangles will simulate stripes in a touch array, which will be used 
   * by the TouchArrayGestureDetector to calculate various gestures.
   */
  void drawRectangles()
  {
    // Draw rectangles
    display.fillScreen(ILI9341_BLACK);
    for(int i = 0; i < RECT_COUNT; i++)
      display.drawRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
  }

  /**
   * @brief Updates the touch state of rectangles and the gestures in the TouchArrayGestureDetector.
   *
   * This function resets the current touched rectangles, checks for new touch inputs,
   * and updates the screen and internal state to reflect changes. Touched
   * rectangles are filled with white, while untapped ones are filled with black
   * and outlined in white.
   */
  void update()
  {
    // Reset the current touch array
    std::fill_n(touchedRectangles, RECT_COUNT, 0);

    // Check for touches
    if(touchScreen.touched())
    {
      TS_Point p{touchScreen.getPoint()};
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
        // Rectangle was just touched - make it white
        display.fillRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
      }
      else if(touchedRectangles[i] == 0 && previouslyTouchedRectangles[i] == 1)
      {
        // Rectangle no longer touched - make it black
        display.fillRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_BLACK);
        display.drawRect(0, i * rectangleHeight, screenWidth, rectangleHeight, ILI9341_WHITE);
      }
    }

    // Update touch array gestures and cache touchedRectangles for the next loop
    touchArrayGD.update(touchedRectangles, RECT_COUNT);
    std::copy(touchedRectangles, touchedRectangles + RECT_COUNT, previouslyTouchedRectangles);
  }

  /**
   * @brief Prints the current touch values to the serial monitor.
   */
  void printUpdate()
  {
    if(touchArrayGD.totalBrush != prev_brush || touchArrayGD.totalRub != prev_rub)
    {
      Serial.println("totalBrush;" + String(touchArrayGD.totalBrush, 6) + ";totalRub;"
                     + String(touchArrayGD.totalRub, 6));
      prev_brush = touchArrayGD.totalBrush;
      prev_rub = touchArrayGD.totalRub;
    }
  }

private:
  //======================== setup screen =======================
  int screenHeight{-1};
  int screenWidth{-1};
  int rectangleHeight{-1};

  Adafruit_ILI9341 display{TinyTouch::PIN_CS, TinyTouch::PIN_DC};
  Adafruit_FT6206 touchScreen;

  //======================== setup puara =======================
  static constexpr int RECT_COUNT{16};
  int touchedRectangles[RECT_COUNT] = {0};
  static constexpr int maxNumBlobs{4};
  static constexpr int touchSizeEdge{4};
  puara_gestures::TouchArrayGestureDetector<maxNumBlobs, touchSizeEdge> touchArrayGD;

  // Keep track of previous values
  int previouslyTouchedRectangles[RECT_COUNT] = {0};
  float prev_brush{-1.0f};
  float prev_rub{-1.0f};
};