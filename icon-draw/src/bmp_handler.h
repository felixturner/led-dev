#ifndef BMP_HANDLER_H
#define BMP_HANDLER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Structure to hold framebuffer data for glitch effects
struct GlitchFramebuffer {
  uint8_t **rData;
  uint8_t **gData;
  uint8_t **bData;
  int16_t width;
  int16_t height;
  bool allocated;
};

// Draw a 24-bit BMP file from LittleFS to the display
bool drawBMP(MatrixPanel_I2S_DMA *display, const char *filename, int16_t x, int16_t y);

// Draw a BMP from embedded PROGMEM array
bool drawEmbeddedBMP(MatrixPanel_I2S_DMA *display, const unsigned char *bmp_data, int16_t x, int16_t y);

// Load BMP into framebuffer for glitch effects
bool loadBMPToFramebuffer(const char *filename, GlitchFramebuffer *fb);

// Free framebuffer memory
void freeFramebuffer(GlitchFramebuffer *fb);

// Draw framebuffer with random glitch effect applied
void drawFramebufferGlitched(MatrixPanel_I2S_DMA *display, GlitchFramebuffer *fb, int16_t x, int16_t y);

#endif
