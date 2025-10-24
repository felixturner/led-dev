# icon-draw - ESP32 LED Matrix Icons

A PlatformIO project for displaying BMP icon images on a 64x64 HUB75 LED matrix panel using an ESP32 Trinity board.

## Project Summary

This project displays 24-bit BMP images on a 64x64 HUB75 LED matrix panel using an ESP32 Trinity board. Images are loaded from the LittleFS filesystem and displayed with smooth fade transitions between each image.

### Hardware
- **Board**: ESP32 Trinity
- **Display**: 64x64 HUB75 LED Matrix Panel (FM6126A driver)
- **Interface**: I2S DMA for flicker-free display
- **USB Chip**: CH340 USB-to-UART

### Features
- 24-bit BMP image decoding and display
- LittleFS filesystem for loading images from flash
- Automatic cycling through multiple images in alphabetical order
- Smooth fade-in/fade-out transitions with easing curves
- Custom pin mapping for ESP32 Trinity board
- Clean code structure with separated BMP handling

### Key Libraries
- ESP32 HUB75 LED Matrix Panel DMA Display
- Adafruit GFX Library
- LittleFS (filesystem)

## Getting Started

### Prerequisites
- [VSCode](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 Trinity board with 64x64 LED matrix panel
- USB cable for programming

### Finding Your ESP32 COM Port

**Important**: The ESP32 Trinity uses a **CH340** USB-to-UART chip.

In Device Manager (Win + X → Device Manager), look for:
- **"USB-Serial CH340"** under "Ports (COM & LPT)"
- Note the COM port number (e.g., COM5)

Update `platformio.ini` with your COM port:
```ini
upload_port = COM5  ; Use your actual COM port number
```

**Tip**: If you have multiple USB devices, unplug other devices to identify which is the ESP32 Trinity.

### Building and Uploading

#### Using PlatformIO Extension (Recommended)

1. Open the project in VSCode
2. Open PlatformIO sidebar (click the alien icon on the left)
3. Under **PROJECT TASKS → esp32dev**:
   - **General → Build** - Compile the code (or click ✓ in bottom status bar)
   - **General → Upload** - Upload code to ESP32 (or click → in bottom status bar)
   - **Platform → Upload Filesystem Image** - Upload BMP files to ESP32
   - **General → Monitor** - View serial output (or click plug icon in bottom status bar)

**Keyboard Shortcuts:**
- Build: `Ctrl+Alt+B`
- Upload: `Ctrl+Alt+U`

#### Using Command Line

```bash
# Build the project
pio run

# Upload code to ESP32
pio run -t upload

# Upload filesystem (BMP files in data/ folder)
pio run -t uploadfs

# Monitor serial output
pio device monitor --baud 115200 --port COM5
```

**Note**: Close the serial monitor before uploading code or filesystem, as only one program can access the COM port at a time.

#### Utility Scripts

Located in `scripts/` folder:

- **check_bmp_info.ps1** - Validate BMP files (dimensions, bit depth, file size)
  ```bash
  powershell -ExecutionPolicy Bypass -File scripts/check_bmp_info.ps1
  ```

- **find_esp32_port.ps1** - Auto-detect ESP32 COM port
  ```bash
  powershell -ExecutionPolicy Bypass -File scripts/find_esp32_port.ps1
  ```

## Adding Your Own Images

### Step 1: Create 24-bit BMP Files

Create BMP files with these specifications:
- **Size**: 64x64 pixels (or smaller)
- **Color Depth**: 24-bit RGB (no alpha channel)
- **Format**: Windows Bitmap (.bmp)

You can use tools like GIMP, Photoshop, or online converters to create BMPs.

### Step 2: Add BMP Files to Data Folder

Place your BMP files in the `data/` folder. Example:
```
data/
  ├── image1.bmp
  ├── image2.bmp
  └── image3.bmp
```

### Step 3: Upload Filesystem to ESP32

**Using PlatformIO Extension**:
- PlatformIO sidebar → PROJECT TASKS → esp32dev → Platform → **Upload Filesystem Image**

**Using Command Line**:
```bash
pio run -t uploadfs
```

**Important**:
- The serial monitor must be closed during upload
- This uploads ALL files in the `data/` folder to the ESP32's flash memory
- You only need to do this when you add/change BMP files

### Step 4: Update Code to Display Your Images

Edit [src/main.cpp](src/main.cpp) around line 100 to add your image filenames:

```cpp
const char* imageFiles[] = {
  "/image1.bmp",
  "/image2.bmp",
  "/image3.bmp"
  // Add more images here
};
const int numImages = 3; // Update this count
```

The images will display in the order listed in the array.

### Step 5: Adjust Timing (Optional)

Edit [src/main.cpp](src/main.cpp) around line 129-130:

```cpp
const unsigned long SHOWING_TIME = 2000; // How long each image displays (ms)
const unsigned long FADE_TIME = 500;     // Fade transition duration (ms)
```

## Project Structure

```
icon-draw/
├── src/
│   ├── main.cpp          # Main application code
│   ├── bmp_handler.h     # BMP function declarations
│   └── bmp_handler.cpp   # BMP decoding implementation
├── data/                 # BMP files to upload to ESP32
│   ├── i0.bmp
│   ├── i01.bmp
│   └── ...
├── scripts/              # Utility PowerShell scripts
│   ├── check_bmp_info.ps1      # Validate BMP files
│   └── find_esp32_port.ps1     # Auto-detect COM port
├── platformio.ini        # PlatformIO configuration
├── CLAUDE.md            # Session notes and project history
└── README.md            # This file
```

## How It Works

1. **Setup**:
   - Initializes serial communication (115200 baud)
   - Mounts LittleFS filesystem
   - Configures LED matrix display (64x64, FM6126A driver)
   - Sets initial brightness and rotation

2. **Animation Loop**:
   - **FADE_IN**: Fade from black to full brightness (0.5s, ease-in curve)
   - **SHOWING**: Display image at full brightness (2s)
   - **FADE_OUT**: Fade to black (0.5s, ease-out curve)
   - **BLACK**: Brief black screen, then load next image
   - Repeat for all images in the array

3. **BMP Loading**:
   - Reads BMP header to get dimensions and color depth
   - Only supports 24-bit BMPs
   - Converts BGR pixel data to RGB565 format
   - Handles BMP bottom-to-top row order
   - Accounts for 4-byte row padding

## Troubleshooting

### COM Port Issues (Access Denied / Port Busy)

If you get "Access Denied" or "Port is busy" errors:

1. **Close serial monitor** - Only one program can use the COM port at a time
2. **Unplug and replug USB cable** - Try uploading immediately after plugging in
3. **Check Device Manager** - Verify ESP32 shows as "USB-Serial CH340"
4. **Restart VSCode** - Sometimes helps clear port locks

If issues persist, reinstall the COM port driver:
- Open Device Manager (Win + X → Device Manager)
- Expand "Ports (COM & LPT)"
- Find "USB-Serial CH340"
- Right-click → Uninstall device (check "delete driver")
- Unplug ESP32, then plug back in
- Windows will reinstall the driver
- Check Device Manager for new COM port number
- Update `platformio.ini` with new port

### Upload Fails / No Response

- Ensure ESP32 is powered (LED on board should be lit)
- Try a different USB cable (some cables are charge-only)
- Press the BOOT button on ESP32 when upload starts (if auto-reset fails)

### Images Not Displaying

- Verify BMP files are 24-bit (not 8-bit or 32-bit)
- Check that filesystem was uploaded (`pio run -t uploadfs`)
- Monitor serial output to see if images are loading
- Verify filenames in code match actual files (case-sensitive, include "/" prefix)

### Display Shows Garbage/Flickering

- Check panel type matches code (FM6126A driver is set in main.cpp)
- Verify all ribbon cable connections are secure
- Try adjusting `mxconfig.clkphase` (line 65 in main.cpp)

## Technical Details

### Pin Mapping (ESP32 Trinity)

```cpp
R1: 25   G1: 26   B1: 27
R2: 14   G2: 12   B2: 13
A: 23    B: 19    C: 5    D: 17    E: 18
LAT: 4   OE: 15   CLK: 16
```

### Display Configuration

- **Panel Resolution**: 64x64 pixels
- **Panel Chain**: 1 (single panel)
- **Driver**: HUB75 FM6126A
- **Scan Rate**: 1/32
- **Color Depth**: RGB565 (16-bit)
- **Default Brightness**: 128/255

### Memory Usage

- **RAM**: ~6.6% (21KB / 320KB)
- **Flash**: ~26.5% (347KB / 1.3MB)
- **Filesystem**: Remaining flash space for BMP storage

## License

This project uses open-source libraries. Check individual library licenses for details.

## Acknowledgments

- [ESP32 HUB75 LED Matrix Panel DMA Display](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA) library
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
