# ESP32 Trinity Alien Clock - Session Notes

## Current Status (2025-10-24)

### ✅ PROJECT CREATED!
**Minimal black and white grid pattern display with glitching animation**

### What We've Done
1. ✅ Created PlatformIO project structure (src/, scripts/)
2. ✅ Created platformio.ini configuration file with ESP32 HUB75 library dependency
3. ✅ Implemented animated 2×2 grid of 7×7 cell patterns
4. ✅ Copied utility scripts from icon-draw project
5. ✅ Created comprehensive README.md with usage instructions
6. ✅ Created this session notes file

### Current Project State
- **Display**: 64x64 HUB75 LED Matrix Panel via ESP32 Trinity board
- **Effect**: 2×2 grid of animated patterns with glitching Matrix aesthetic
- **Pattern**: 9 fixed white dots + randomized cells per pattern
- **Animation**: One random pattern changes every 200ms
- **COM Port**: COM5 (adjust in platformio.ini for your setup)
- **Status**: Ready to build and upload

### Next Steps
1. Open project in VSCode with PlatformIO extension
2. Update COM port in platformio.ini if needed
3. Build and upload to ESP32 Trinity board

## Project Features
- ✅ 2×2 grid of 7×7 cell patterns (each cell is 4×4 pixels)
- ✅ 9 fixed white dots per pattern at odd positions
- ✅ Random cell glitching effect (200ms interval)
- ✅ Deterministic randomization using coordinate-based hashing
- ✅ Minimal black and white aesthetic
- ✅ Custom pin mapping for ESP32 Trinity
- ✅ FM6126A driver support

## Important Files
- **Code**: [src/main.cpp](src/main.cpp)
- **Config**: [platformio.ini](platformio.ini)
- **Docs**: [README.md](README.md)
- **Scripts**: [scripts/find_esp32_port.ps1](scripts/find_esp32_port.ps1)

## Quick Commands

**Build project:**
```bash
cd c:\dev\led-dev\alien-clock
pio run
```

**Upload code via USB:**
```bash
pio run -t upload
```

**Find ESP32 port:**
```powershell
powershell -ExecutionPolicy Bypass -File scripts/find_esp32_port.ps1
```

## Technical Details

### Libraries Used
- ESP32 HUB75 LED Matrix Panel DMA Display (v3.0.0+)

### Hardware Configuration
- **Panel**: 64x64 HUB75 (FM6126A driver)
- **Board**: ESP32 Trinity (CH340 USB-to-UART)
- **Pin E**: GPIO 18 (required for 1/32 scan on 64x64 panels)
- **Brightness**: 128/255 (approximately 50%)
- **Display rotation**: Physically rotated 90°, X/Y swapped in code

### Pattern Structure
Each 7×7 cell pattern has:
- **Outer border** (cells 0, 6): Always black (provides spacing)
- **9 white dots**: Odd positions (1,1), (1,3), (1,5), (3,1), (3,3), (3,5), (5,1), (5,3), (5,5)
- **4 diagonal masks**: (2,2), (2,4), (4,2), (4,4) - Always black
- **Random cells**: Remaining even positions - filled based on deterministic hash

### Animation Logic
- Every 200ms: One random pattern from the 2×2 grid is selected
- That pattern's random cells re-randomize with a new seed
- Other 3 patterns remain frozen
- Creates a glitching Matrix-like effect

### Hash Function
Uses sine-based deterministic randomization:
```cpp
sin(x * 12.9898 + y * 78.233 + seed * 45.164)
```

## Customization Options

Users can easily customize:
- **Pattern update speed**: Adjust interval in main.cpp (default 200ms)
- **Fill probability**: Change hash threshold in main.cpp (default 0.5 = 50%)
- **Brightness**: Adjust in main.cpp (default 128/255)
- **COM Port**: Update in platformio.ini

## Notes

This project displays a minimal black and white grid pattern with a glitching Matrix aesthetic. The pattern concept is inspired by Matrix digital rain and alien glyphs.

The project structure follows the same pattern as the icon-draw project in this repository, making it easy to maintain multiple ESP32 LED projects in the same workspace.
