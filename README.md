# LED Matrix Demos

A collection of PlatformIO projects for ESP32-based LED matrix displays.

## Projects

### png-draw
ESP32 Trinity LED Matrix display project that shows BMP images with smooth fade transitions.
- 64x64 HUB75 LED Matrix Panel
- WiFi + OTA updates
- LittleFS filesystem for image storage
- Fade-in/fade-out animations

[View png-draw README](png-draw/README.md)

## Hardware

- **Board**: ESP32 Trinity
- **Display**: 64x64 HUB75 LED Matrix Panel (FM6126A driver)

## Development

All projects use PlatformIO for building and uploading.

**Requirements:**
- VSCode with PlatformIO extension
- ESP32 drivers (Silicon Labs CP210x)

**Common Commands:**
```bash
pio run              # Build
pio run -t upload    # Upload code
pio run -t uploadfs  # Upload filesystem
```

## License

MIT
