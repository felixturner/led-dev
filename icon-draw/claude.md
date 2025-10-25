# ESP32 Trinity LED Matrix Project - Session Notes

## 🔴 IMPORTANT RULES

1. **DO NOT commit or push to git without asking user first!**
   - ALWAYS ask before running `git commit` or `git push`
   - User wants to review changes before they are committed
   - No exceptions - even for documentation or small fixes

2. **DO NOT run `pio run -t upload` or `pio run` commands without asking user first!**
   - User prefers to run compile and upload commands himself to see the output directly.

3. **DO NOT modify user's code edits without discussing first!**
   - If user has manually edited values (timing constants, configuration, etc.), respect those changes.
   - DO NOT revert or "fix" user edits unless explicitly asked.
   - If you think a value should be different, ASK first, don't just change it.

4. **RESPECT original line endings when editing files!**
   - Windows project uses CRLF line endings
   - Do NOT change line endings to LF when editing
   - This causes git warnings about line ending changes

5. **DO NOT add Claude credits to git commits!**
   - No "Generated with Claude Code" footer
   - No "Co-Authored-By: Claude" signature
   - Keep commit messages clean and professional

## Current Status (2025-10-23)

### ✅ PROJECT FULLY WORKING!
**All 16 BMP images cycling with fade animations on the LED matrix!**

### Recent Changes
- ✅ Refactored code: moved BMP handling to separate files (bmp_handler.h/cpp)
- ✅ Removed WiFi/OTA code (not needed for now)
- ✅ Updated to display all 16 BMP files in alphabetical order
- ✅ Filesystem uploaded with `pio run -t uploadfs`

### Current Animation
- 16 images cycling: i0.bmp → i01.bmp → i10.bmp → i10a.bmp → i11.bmp → i12.bmp → i13.bmp → i14.bmp → i15.bmp → i16.bmp → i2.bmp → i3.bmp → i4.bmp → i5.bmp → i9.bmp → i9b.bmp → (repeat)
- Each image: 0.5s fade in → 2s display → 0.5s fade out → next image

### What We've Done
1. ✅ Converted Arduino project to PlatformIO
2. ✅ Cleaned up project structure (removed old Arduino files, XBM/embedded image code)
3. ✅ Fixed COM port detection (ESP32 Trinity uses CH340, not CP210x)
4. ✅ Added WiFi/OTA support with timeout (doesn't block if WiFi unavailable)
5. ✅ Successfully uploaded code via USB on COM5
6. ✅ Successfully uploaded filesystem (skull.bmp, star.bmp) via USB
7. ✅ WiFi connected to network (IP: 192.168.254.29)
8. ✅ Tested image switching (skull ↔ star) via USB uploads
9. ✅ **VERIFIED: Both images display correctly on 64x64 LED matrix panel**

### Current Project State
- **Display**: 64x64 HUB75 LED Matrix Panel via ESP32 Trinity board
- **Images**: Two BMP files in `data/` folder (skull.bmp 64x64, star.bmp 64x64)
- **Current Display**: Star image (static, no animations)
- **COM Port**: COM5 (USB-Serial CH340 chip)
- **WiFi**: Connected to network at 192.168.254.29
- **Upload Method**: USB (via COM5) - reliable and working
- **Status**: **WORKING - Star visible on LED panel**

### Issue RESOLVED
✅ **COM port identification fixed**
- Initial assumption that ESP32 Trinity uses CP210x was incorrect
- **ESP32 Trinity actually uses CH340 USB-to-UART chip**
- Correct port is **COM5** (shows as "USB-Serial CH340" in Device Manager)
- Updated platformio.ini to use COM5

**Important Notes:**
- If you have multiple LED devices, unplug others to identify which is which
- ESP32 Trinity will show as "USB-Serial CH340" NOT "Silicon Labs CP210x"
- The find_esp32_port.ps1 script was looking for CP210x, so it didn't detect the Trinity

### OTA WiFi Setup
✅ WiFi/OTA code has been removed from the project (not currently needed)

**Note:**
- WiFi/OTA support can be added back if needed for wireless updates
- Current workflow uses USB uploads via COM5 which is fast and reliable

**To enable OTA uploads (optional):**
1. Edit [main.cpp:8-9](src/main.cpp#L8-L9) with correct WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_ACTUAL_WIFI_SSID";
   const char* password = "YOUR_ACTUAL_WIFI_PASSWORD";
   ```

2. Upload updated code via USB:
   ```bash
   cd c:\dev\led-dev\png-draw
   pio run -t upload
   ```

3. Monitor serial output to get IP address:
   ```bash
   pio device monitor --baud 115200 --port COM5
   ```

4. Update [platformio.ini](platformio.ini#L21-L28) for OTA:
   ```ini
   upload_protocol = espota
   upload_port = ESP32-Trinity.local  ; or use IP address
   ; upload_port = COM5  ; comment out USB upload
   ```

### Files Modified Today (2025-10-23)
- `src/main.cpp` - Simplified to static skull display, added WiFi timeout
- `platformio.ini` - Already configured for COM5
- `CLAUDE.md` - Updated with success status

### Quick Commands
**Upload code via USB:**
```bash
cd c:\dev\led-dev\png-draw
pio run -t upload
```

**Upload filesystem (images) via USB:**
```bash
pio run -t uploadfs
```

**Monitor serial output:**
```bash
pio device monitor --baud 115200 --port COM5
```

**Find ESP32 port:**
```powershell
powershell.exe -ExecutionPolicy Bypass -File find_esp32_port.ps1
```

### Important Files
- **Code**: `src/main.cpp`
- **Config**: `platformio.ini`
- **Images**: `data/skull.bmp`, `data/star.bmp`
- **Docs**: `README.md`

### Display Configuration
Current setup in [main.cpp](src/main.cpp):
- **Image**: Static display of skull.bmp (64x64 pixels)
- **Brightness**: Set to 255 (full brightness)
- **Rotation**: 90 degrees counter-clockwise
- **Refresh Rate**: ~100 Hz (automatic)

**To switch between images or add animations:**
- Modify the `loop()` function in [main.cpp:288-305](src/main.cpp#L288-L305)
- See the git history for the previous fade transition code

---

## Project Features
- ✅ Displays 24-bit BMP images from LittleFS filesystem
- ✅ Supports 64x64 pixel BMP files (can store multiple in data/ folder)
- ✅ WiFi/OTA support built-in (works offline if WiFi unavailable)
- ✅ Fast USB uploads via COM5 (CH340 driver)
- ✅ Ready for wireless OTA updates (once WiFi configured)
- ✅ Automatic FM6126A LED driver initialization
- ✅ No external SD card needed - uses ESP32 internal flash storage
