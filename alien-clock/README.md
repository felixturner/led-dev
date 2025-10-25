# Alien Clock

A minimal b/w grid pattern display for 64x64 HUB75 LED panels using ESP32 Trinity.

## Overview

Displays a 2×2 grid of animated 7×7 cell patterns with a glitching Matrix aesthetic. Each pattern contains 9 fixed white dots with randomized cells that change individually every 200ms, creating an alien digital clock effect.

## Hardware

- **Board**: ESP32 Trinity
- **Display**: 64×64 HUB75 LED Matrix Panel (FM6126A driver)
- **Interface**: I2S DMA
- **USB**: CH340 USB-to-UART

## Features

- 2×2 grid of 7×7 cell patterns (each cell is 4×4 pixels)
- 9 fixed white dots per pattern at odd cell positions
- 4 masked diagonal cells (always black)
- Random pattern glitching effect (one pattern changes every 200ms)
- Deterministic randomization using coordinate-based hashing

## Quick Start

### Prerequisites
- VSCode with PlatformIO extension
- ESP32 Trinity board with 64×64 LED matrix panel

### Upload

1. Update COM port in [platformio.ini](platformio.ini)
2. Build: `Ctrl+Alt+B`
3. Upload: `Ctrl+Alt+U`

Or via command line:
```bash
pio run -t upload
```

## Customization

**Change pattern update speed** - Edit [src/main.cpp:81](src/main.cpp#L81):
```cpp
if (millis() - last_pattern_change >= 200)  // Change 200 to desired ms
```

**Adjust fill probability** - Edit [src/main.cpp:33](src/main.cpp#L33):
```cpp
return hash >= 0.5;  // 0.5 = 50% fill, 0.3 = 70% fill, etc.
```

**Modify brightness** - Edit [src/main.cpp:62](src/main.cpp#L62):
```cpp
dma_display->setBrightness8(128);  // 0-255
```

## How It Works

### Pattern Structure
Each 7×7 cell pattern has:
- **Outer border** (cells 0, 6): Always black (provides spacing)
- **9 white dots**: Odd positions (1,1), (1,3), (1,5), (3,1), (3,3), (3,5), (5,1), (5,3), (5,5)
- **4 diagonal masks**: (2,2), (2,4), (4,2), (4,4) - Always black
- **Random cells**: Remaining even positions - filled based on deterministic hash

### Animation
- Every 200ms: One random pattern from the 2×2 grid is selected
- That pattern's random cells re-randomize with a new seed
- Other 3 patterns remain frozen
- Creates a glitching Matrix-like effect

## Project Structure

```
alien-clock/
├── src/
│   └── main.cpp          # Main application code
├── platformio.ini        # PlatformIO configuration
├── CLAUDE.md            # Development session notes
└── README.md            # This file
```

## Technical Details

- **Display rotation**: Physically rotated 90°, X/Y swapped in code
- **Cell size**: 4×4 pixels per cell
- **Pattern size**: 28×28 pixels per pattern (7 cells × 4px)
- **Grid**: 2×2 patterns = 56×56 content + 4px padding = 64×64 total
- **Hash function**: `sin(x * 12.9898 + y * 78.233 + seed * 45.164)`

## Troubleshooting

**COM Port Issues**: Close serial monitor before uploading. Check Device Manager for "USB-Serial CH340".

**Flickering pixels**: Known hardware issue with 3.3V ESP32 signals on 5V HUB75 panels. Reduced I2S speed to 8MHz helps. For best results, use a level shifter.

## Credits

Pattern concept inspired by Matrix digital rain and alien glyphs.
