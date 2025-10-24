#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <LittleFS.h>
#include "bmp_handler.h"
// #include "ota_handler.h"  // Disabled for now

/*--------------------- DEBUG  -------------------------*/
#define Sprintln(a) (Serial.println(a))
#define SprintlnDEC(a, x) (Serial.println(a, x))
#define Sprint(a) (Serial.print(a))
#define SprintDEC(a, x) (Serial.print(a, x))

/*--------------------- RGB SHOWING PINS -------------------------*/
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19 // Changed from library default
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

/*--------------------- MATRIX LILBRARY CONFIG -------------------------*/
#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1  // Total number of panels chained one to another

MatrixPanel_I2S_DMA *dma_display = nullptr;

/*
//Another way of creating config structure
//Custom pin mapping for all pins
HUB75_I2S_CFG::i2s_pins _pins={R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};
HUB75_I2S_CFG mxconfig(
            64,   // width
            64,   // height
             4,   // chain length
           _pins,   // pin mapping
  HUB75_I2S_CFG::FM6126A      // driver chip
);

*/

// mxconfig.gpio.e = -1; // Assign a pin if you have a 64x64 panel
// mxconfig.clkphase = false; // Change this if you have issues with ghosting.
// mxconfig.driver = HUB75_I2S_CFG::FM6126A; // Change this according to your pane.

void setup()
{

  // Module configuration
  HUB75_I2S_CFG mxconfig(
      PANEL_RES_X, // module width
      PANEL_RES_Y, // module height
      PANEL_CHAIN  // Chain length
  );
  mxconfig.gpio.e = 18;                     // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
  mxconfig.driver = HUB75_I2S_CFG::FM6126A; // in case that we use panels based on FM6126A chip, we can change that
  mxconfig.clkphase = false;

  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);
  delay(200);

  /************** LITTLEFS **************/
  Sprintln("...Mounting LittleFS");
  if (!LittleFS.begin(true))
  {
    Sprintln("LittleFS Mount Failed");
    return;
  }
  Sprintln("LittleFS Mounted Successfully");

  // /************** WIFI & OTA **************/
  // if (setupWiFi())
  // {
  //   setupOTA();
  // }

  /************** SHOWING **************/
  Sprintln("...Starting Display");
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(128); // 0-255
  dma_display->clearScreen();
  dma_display->setRotation(3); // 90 degrees counter-clockwise
}

// Enum for animation state machine
enum AnimState
{
  FADE_IN,
  SHOWING,
  FADE_OUT,
  BLACK
};

// Array of BMP filenames in alphabetical order
const char *imageFiles[] = {
    "/i0.bmp",
    "/i01.bmp",
    "/i10.bmp",
    "/i10a.bmp",
    "/i11.bmp",
    "/i12.bmp",
    "/i13.bmp",
    "/i14.bmp",
    "/i15.bmp",
    "/i16.bmp",
    "/i2.bmp",
    "/i3.bmp",
    "/i4.bmp",
    "/i5.bmp",
    "/i9.bmp",
    "/i9b.bmp"};
const int numImages = 16;

// Framebuffer for glitch effects
GlitchFramebuffer framebuffer = {nullptr, nullptr, nullptr, 0, 0, false};

void loop()
{
  // // Handle OTA updates
  // handleOTA();

  // State machine for fading between images
  static int currentImage = 0;
  static unsigned long lastTransitionTime = 0;
  static AnimState state = FADE_IN;
  static bool imageLoaded = false;

  // Timing constants (in milliseconds)
  const unsigned long SHOWING_TIME = 100; // Hold image for 2 seconds
  const unsigned long FADE_TIME = 10;     // Fade to black for 0.5 seconds

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTransitionTime;

  switch (state)
  {
  case FADE_IN:
    if (!imageLoaded)
    {
      // Load the current image into framebuffer
      Sprint("Loading image: ");
      Sprintln(imageFiles[currentImage]);

      loadBMPToFramebuffer(imageFiles[currentImage], &framebuffer);
      imageLoaded = true;
      lastTransitionTime = currentTime;
    }

    // Draw glitched frame (new glitch every frame during fade in)
    drawFramebufferGlitched(dma_display, &framebuffer, 0, 0);

    // Fade in from black (0 -> 255) with easing
    if (elapsedTime < FADE_TIME)
    {
      float progress = (float)elapsedTime / FADE_TIME;
      // Ease-in: slower start, faster end
      progress = progress * progress;
      uint8_t brightness = (uint8_t)(progress * 255);
      dma_display->setBrightness8(brightness);
    }
    else
    {
      dma_display->setBrightness8(255);
      state = SHOWING;
      lastTransitionTime = currentTime;
    }
    break;

  case SHOWING:
    // Draw glitched frame every frame (animated glitch effect!)
    drawFramebufferGlitched(dma_display, &framebuffer, 0, 0);

    // Hold at full brightness
    if (elapsedTime >= SHOWING_TIME)
    {
      state = FADE_OUT;
      lastTransitionTime = currentTime;
    }
    break;

  case FADE_OUT:
    // Draw glitched frame (new glitch every frame during fade out)
    drawFramebufferGlitched(dma_display, &framebuffer, 0, 0);

    // Fade out to black (255 -> 0) with easing
    if (elapsedTime < FADE_TIME)
    {
      float progress = (float)elapsedTime / FADE_TIME;
      // Ease-out: faster start, slower end
      progress = 1.0 - ((1.0 - progress) * (1.0 - progress));
      uint8_t brightness = (uint8_t)((1.0 - progress) * 255);
      dma_display->setBrightness8(brightness);
    }
    else
    {
      dma_display->setBrightness8(0);
      state = BLACK;
      lastTransitionTime = currentTime;
    }
    break;

  case BLACK:
    // Free the current framebuffer
    freeFramebuffer(&framebuffer);

    // Stay black briefly, then switch to random image
    dma_display->clearScreen();
    currentImage = random(0, numImages); // Pick random image
    imageLoaded = false;
    state = FADE_IN;
    lastTransitionTime = currentTime;
    break;
  }

  // Short delay - 60 FPS (1000ms / 60 = ~16.67ms)
  delay(16);
}
