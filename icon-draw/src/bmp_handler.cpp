#include "bmp_handler.h"

/*--------------------- DEBUG -------------------------*/
#define Sprintln(a) (Serial.println(a))
#define Sprint(a) (Serial.print(a))

// BMP decoder function for 24-bit BMP files
bool drawBMP(MatrixPanel_I2S_DMA *display, const char *filename, int16_t x, int16_t y)
{
  File bmpFile = LittleFS.open(filename, "r");
  if (!bmpFile)
  {
    Sprintln("Failed to open BMP file");
    return false;
  }

  // Read BMP header
  uint16_t signature = bmpFile.read() | (bmpFile.read() << 8);
  if (signature != 0x4D42) // "BM" in little-endian
  {
    Sprintln("Not a valid BMP file");
    bmpFile.close();
    return false;
  }

  // Skip file size (4 bytes) and reserved fields (4 bytes)
  bmpFile.seek(10);
  uint32_t dataOffset = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);

  // Read DIB header
  bmpFile.seek(18);
  int32_t width = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  int32_t height = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);

  bmpFile.seek(28);
  uint16_t bitsPerPixel = bmpFile.read() | (bmpFile.read() << 8);

  Sprint("BMP: ");
  Sprint(width);
  Sprint("x");
  Sprint(height);
  Sprint(" @ ");
  Sprint(bitsPerPixel);
  Sprintln("bpp");

  // We only support 24-bit BMP
  if (bitsPerPixel != 24)
  {
    Sprintln("Only 24-bit BMP supported");
    bmpFile.close();
    return false;
  }

  // Calculate row size (must be multiple of 4 bytes)
  uint32_t rowSize = ((width * 3 + 3) / 4) * 4;
  uint8_t row[rowSize];

  // BMP images are stored bottom-to-top
  for (int16_t row_idx = height - 1; row_idx >= 0; row_idx--)
  {
    bmpFile.seek(dataOffset + row_idx * rowSize);
    bmpFile.read(row, rowSize);

    for (int16_t col = 0; col < width; col++)
    {
      // BMP stores pixels as BGR
      uint8_t b = row[col * 3];
      uint8_t g = row[col * 3 + 1];
      uint8_t r = row[col * 3 + 2];

      // Convert RGB888 to RGB565
      uint16_t color565 = display->color565(r, g, b);
      display->drawPixel(x + col, y + (height - 1 - row_idx), color565);
    }
  }

  bmpFile.close();
  Sprintln("BMP loaded successfully");
  return true;
}

// Draw BMP from embedded PROGMEM array (no LittleFS needed)
bool drawEmbeddedBMP(MatrixPanel_I2S_DMA *display, const unsigned char *bmp_data, int16_t x, int16_t y)
{
  // Read BMP header from PROGMEM
  uint16_t signature = pgm_read_byte(bmp_data) | (pgm_read_byte(bmp_data + 1) << 8);
  if (signature != 0x4D42) // "BM" in little-endian
  {
    Sprintln("Not a valid BMP file");
    return false;
  }

  // Get data offset
  uint32_t dataOffset = pgm_read_byte(bmp_data + 10) |
                        (pgm_read_byte(bmp_data + 11) << 8) |
                        (pgm_read_byte(bmp_data + 12) << 16) |
                        (pgm_read_byte(bmp_data + 13) << 24);

  // Get width and height
  int32_t width = pgm_read_byte(bmp_data + 18) |
                  (pgm_read_byte(bmp_data + 19) << 8) |
                  (pgm_read_byte(bmp_data + 20) << 16) |
                  (pgm_read_byte(bmp_data + 21) << 24);

  int32_t height = pgm_read_byte(bmp_data + 22) |
                   (pgm_read_byte(bmp_data + 23) << 8) |
                   (pgm_read_byte(bmp_data + 24) << 16) |
                   (pgm_read_byte(bmp_data + 25) << 24);

  // Get bits per pixel
  uint16_t bitsPerPixel = pgm_read_byte(bmp_data + 28) | (pgm_read_byte(bmp_data + 29) << 8);

  Sprint("Embedded BMP: ");
  Sprint(width);
  Sprint("x");
  Sprint(height);
  Sprint(" @ ");
  Sprint(bitsPerPixel);
  Sprintln("bpp");

  // We only support 24-bit BMP
  if (bitsPerPixel != 24)
  {
    Sprintln("Only 24-bit BMP supported");
    return false;
  }

  // Calculate row size (must be multiple of 4 bytes)
  uint32_t rowSize = ((width * 3 + 3) / 4) * 4;

  // BMP images are stored bottom-to-top
  for (int16_t row_idx = height - 1; row_idx >= 0; row_idx--)
  {
    uint32_t rowStart = dataOffset + row_idx * rowSize;

    for (int16_t col = 0; col < width; col++)
    {
      // BMP stores pixels as BGR
      uint8_t b = pgm_read_byte(bmp_data + rowStart + col * 3);
      uint8_t g = pgm_read_byte(bmp_data + rowStart + col * 3 + 1);
      uint8_t r = pgm_read_byte(bmp_data + rowStart + col * 3 + 2);

      // Convert RGB888 to RGB565
      uint16_t color565 = display->color565(r, g, b);
      display->drawPixel(x + col, y + (height - 1 - row_idx), color565);
    }
  }

  Sprintln("Embedded BMP loaded successfully");
  return true;
}

// Load BMP into framebuffer for glitch effects
bool loadBMPToFramebuffer(const char *filename, GlitchFramebuffer *fb)
{
  File bmpFile = LittleFS.open(filename, "r");
  if (!bmpFile)
  {
    Sprintln("Failed to open BMP file");
    return false;
  }

  // Read BMP header
  uint16_t signature = bmpFile.read() | (bmpFile.read() << 8);
  if (signature != 0x4D42)
  {
    Sprintln("Not a valid BMP file");
    bmpFile.close();
    return false;
  }

  bmpFile.seek(10);
  uint32_t dataOffset = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);

  bmpFile.seek(18);
  int32_t width = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  int32_t height = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);

  bmpFile.seek(28);
  uint16_t bitsPerPixel = bmpFile.read() | (bmpFile.read() << 8);

  if (bitsPerPixel != 24)
  {
    Sprintln("Only 24-bit BMP supported");
    bmpFile.close();
    return false;
  }

  Sprint("Loading to framebuffer: ");
  Sprint(width);
  Sprint("x");
  Sprint(height);
  Sprintln("");

  // Allocate framebuffer memory
  fb->width = width;
  fb->height = height;
  fb->rData = new uint8_t *[height];
  fb->gData = new uint8_t *[height];
  fb->bData = new uint8_t *[height];

  for (int16_t i = 0; i < height; i++)
  {
    fb->rData[i] = new uint8_t[width];
    fb->gData[i] = new uint8_t[width];
    fb->bData[i] = new uint8_t[width];
  }

  // Read BMP data into separate RGB channels
  uint32_t rowSize = ((width * 3 + 3) / 4) * 4;
  uint8_t row[rowSize];

  for (int16_t row_idx = height - 1; row_idx >= 0; row_idx--)
  {
    bmpFile.seek(dataOffset + row_idx * rowSize);
    bmpFile.read(row, rowSize);

    for (int16_t col = 0; col < width; col++)
    {
      int16_t y_pos = height - 1 - row_idx;
      fb->bData[y_pos][col] = row[col * 3];
      fb->gData[y_pos][col] = row[col * 3 + 1];
      fb->rData[y_pos][col] = row[col * 3 + 2];
    }
  }

  bmpFile.close();
  fb->allocated = true;
  Sprintln("Framebuffer loaded");
  return true;
}

// Free framebuffer memory
void freeFramebuffer(GlitchFramebuffer *fb)
{
  if (!fb->allocated)
    return;

  for (int16_t i = 0; i < fb->height; i++)
  {
    delete[] fb->rData[i];
    delete[] fb->gData[i];
    delete[] fb->bData[i];
  }
  delete[] fb->rData;
  delete[] fb->gData;
  delete[] fb->bData;

  fb->allocated = false;
  Sprintln("Framebuffer freed");
}

// Draw framebuffer with random glitch effect applied
void drawFramebufferGlitched(MatrixPanel_I2S_DMA *display, GlitchFramebuffer *fb, int16_t x, int16_t y)
{
  if (!fb->allocated)
    return;

  int16_t width = fb->width;
  int16_t height = fb->height;

  // Create temporary working copies for this frame's glitch
  uint8_t **rTemp = new uint8_t *[height];
  uint8_t **gTemp = new uint8_t *[height];
  uint8_t **bTemp = new uint8_t *[height];

  for (int16_t i = 0; i < height; i++)
  {
    rTemp[i] = new uint8_t[width];
    gTemp[i] = new uint8_t[width];
    bTemp[i] = new uint8_t[width];

    // Copy original data
    memcpy(rTemp[i], fb->rData[i], width);
    memcpy(gTemp[i], fb->gData[i], width);
    memcpy(bTemp[i], fb->bData[i], width);
  }

  // Generate 3 random glitch boxes
  for (int glitchBox = 0; glitchBox < 4; glitchBox++)
  {
    // Random box dimensions and position (can go off edge)
    int16_t boxX = random(-width / 2, width);
    int16_t boxY = random(-height / 2, height);
    int16_t boxW = random(width / 4, width);
    int16_t boxH = random(height / 4, height);

    // 50% chance to shift whole image chunk (all RGB), 50% chance to shift single channel
    bool shiftAllChannels = random(0, 2) == 0;

    // Random channel to offset (0=R, 1=G, 2=B) - only used if not shifting all
    int channel = random(0, 3);

    // Random offset amount (10-40 pixels)
    int16_t offsetX = random(0, 4) * (random(0, 2) == 0 ? 1 : -1);
    int16_t offsetY = random(0, 4) * (random(0, 2) == 0 ? 1 : -1);

    // Apply offset to the selected channel(s) within the box
    for (int16_t by = 0; by < boxH; by++)
    {
      for (int16_t bx = 0; bx < boxW; bx++)
      {
        int16_t srcX = boxX + bx;
        int16_t srcY = boxY + by;
        int16_t dstX = srcX + offsetX;
        int16_t dstY = srcY + offsetY;

        // Wrap coordinates around image boundaries (toroidal wrapping)
        // Handle negative values properly with modulo
        srcX = ((srcX % width) + width) % width;
        srcY = ((srcY % height) + height) % height;
        dstX = ((dstX % width) + width) % width;
        dstY = ((dstY % height) + height) % height;

        if (shiftAllChannels)
        {
          // Shift entire image chunk (all RGB channels together)
          rTemp[srcY][srcX] = fb->rData[dstY][dstX];
          gTemp[srcY][srcX] = fb->gData[dstY][dstX];
          bTemp[srcY][srcX] = fb->bData[dstY][dstX];
        }
        else
        {
          // Shift only one color channel (chromatic aberration effect)
          if (channel == 0) // Red channel
          {
            rTemp[srcY][srcX] = fb->rData[dstY][dstX];
          }
          else if (channel == 1) // Green channel
          {
            gTemp[srcY][srcX] = fb->gData[dstY][dstX];
          }
          else // Blue channel
          {
            bTemp[srcY][srcX] = fb->bData[dstY][dstX];
          }
        }
      }
    }
  }

  // Draw the glitched frame to the display
  for (int16_t py = 0; py < height; py++)
  {
    for (int16_t px = 0; px < width; px++)
    {
      uint16_t color565 = display->color565(rTemp[py][px], gTemp[py][px], bTemp[py][px]);
      display->drawPixel(x + px, y + py, color565);
    }
  }

  // Clean up temporary buffers
  for (int16_t i = 0; i < height; i++)
  {
    delete[] rTemp[i];
    delete[] gTemp[i];
    delete[] bTemp[i];
  }
  delete[] rTemp;
  delete[] gTemp;
  delete[] bTemp;
}
