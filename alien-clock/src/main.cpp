
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Configure for your panel(s) as appropriate!
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 64 // Panel height of 64 will required PIN_E to be defined.
#define PANELS_NUMBER 1 // Number of chained panels, if just a single panel, obviously set to 1
#define PIN_E 18

#define PANE_WIDTH PANEL_WIDTH *PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

unsigned long last_pattern_change = 0;
int randomization_seed = 0;
int pattern_seeds[2][2] = {{0, 0}, {0, 0}};
int target_pattern_x = 0;
int target_pattern_y = 0;

// Simple hash function to generate deterministic random bool from coordinates + seed
bool isEvenCellFilled(int globalX, int globalY, int seed)
{
  // Use a simple hash function based on global cell coordinates and seed
  float hash = sin(globalX * 12.9898 + globalY * 78.233 + seed * 45.164) * 43758.5453;
  hash = hash - floor(hash); // Get fractional part (0.0 to 1.0)
  return hash >= 0.5;        // 50% chance to be filled
}

void setup()
{

  Serial.begin(115200);

  Serial.println(F("*****************************************************"));
  Serial.println(F("*        ESP32-HUB75-MatrixPanel-I2S-DMA DEMO       *"));
  Serial.println(F("*****************************************************"));

  /*
    For example we have two 64x64 panels chained, so we need to customize our setup like this

  */
  HUB75_I2S_CFG mxconfig;
  mxconfig.mx_height = PANEL_HEIGHT;        // we have 64 pix heigh panels
  mxconfig.chain_length = PANELS_NUMBER;    // we have 2 panels chained
  mxconfig.gpio.e = PIN_E;                  // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
  mxconfig.driver = HUB75_I2S_CFG::FM6126A; // in case that we use panels based on FM6126A chip, we can change that

  // Try slower I2S speed to reduce signal integrity issues with 3.3V ESP32 signals
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_8M; // Default is HZ_10M, try slower: HZ_8M

  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // Reduce brightness to 50% - can help with signal integrity issues
  dma_display->setBrightness8(128); // range is 0-255, trying 50% instead of 75%

  // Allocate memory and start DMA display
  if (not dma_display->begin())
    Serial.println("****** !KABOOM! I2S memory allocation failed ***********");

  // FM6126A panels need extra initialization time
  Serial.println("Initializing FM6126A driver...");
  delay(100); // Give FM6126A time to initialize properly

  // Clear screen multiple times to flush any corrupt data
  dma_display->clearScreen();
  delay(50);
  dma_display->fillScreenRGB888(0, 0, 0);
  delay(50);
  dma_display->clearScreen();

  Serial.println("Starting letter pattern effect...");
}

void loop()
{

  // Every 200ms, pick a random pattern and randomize it once
  if (millis() - last_pattern_change >= 100)
  {
    target_pattern_x = random(2);
    target_pattern_y = random(2);
    randomization_seed++;
    pattern_seeds[target_pattern_x][target_pattern_y] = randomization_seed;
    last_pattern_change = millis();
  }

  // Configuration: 2×2 grid of 7×7 cell patterns centered in 64×64 display
  // Each pattern: 7 cells × 4px per cell = 28px
  // 2 patterns: 2 × 28px = 56px
  // Remaining: 64 - 56 = 8px → 4px padding on each side to center
  const int CELL_SIZE = 4;
  const int PATTERN_SIZE = 28;
  const int OUTER_PADDING = 4;

  for (int x = 0; x < PANE_WIDTH; x++)
  {
    for (int y = 0; y < PANE_HEIGHT; y++)
    {
      // Check if we're in outer padding (to center the 56×56 grid in 64×64 display)
      if (x < OUTER_PADDING || x >= (PANE_WIDTH - OUTER_PADDING) ||
          y < OUTER_PADDING || y >= (PANE_HEIGHT - OUTER_PADDING))
      {
        // Outer padding - draw black
        dma_display->drawPixelRGB888(x, y, 0, 0, 0);
        continue;
      }

      // NOTE: Display is physically rotated, so swap X and Y
      int contentX = y - OUTER_PADDING; // Use Y for horizontal, remove padding
      int contentY = x - OUTER_PADDING; // Use X for vertical, remove padding

      // Which pattern in the 2×2 grid? (0-1 on each axis)
      int gridX = contentX / PATTERN_SIZE;
      int gridY = contentY / PATTERN_SIZE;

      // Position within the pattern
      int patternLocalX = contentX % PATTERN_SIZE;
      int patternLocalY = contentY % PATTERN_SIZE;

      // Which cell of the 7×7 grid within this pattern?
      int cellX = patternLocalX / CELL_SIZE;
      int cellY = patternLocalY / CELL_SIZE;

      uint8_t brightness = 0;

      // Outer border cells are always black
      if (cellX == 0 || cellX == 6 || cellY == 0 || cellY == 6)
      {
        brightness = 0;
      }
      else
      {
        // Inner 5×5 grid
        bool isOddX = (cellX % 2) == 1;
        bool isOddY = (cellY % 2) == 1;

        if (isOddX && isOddY)
        {
          // 9 dots at odd positions - always white
          brightness = 255;
        }
        else if ((cellX == 2 || cellX == 4) && (cellY == 2 || cellY == 4))
        {
          // 4 diagonal cells around center - always black
          brightness = 0;
        }
        else
        {
          // Remaining even cells - random fill based on pattern seed
          int globalCellX = gridX * 7 + cellX;
          int globalCellY = gridY * 7 + cellY;
          int seed = pattern_seeds[gridX][gridY];

          if (isEvenCellFilled(globalCellX, globalCellY, seed))
          {
            brightness = 255;
          }
        }
      }

      // Draw pixel
      dma_display->drawPixelRGB888(x, y, brightness, brightness, brightness);
    }
  }
}
