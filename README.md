# ESP32-S3 I2S Music Visualizer

A high-performance real-time audio spectrum visualizer for the ESP32-S3 platform. This project processes ambient sound using an I2S digital microphone and renders a smooth, mirrored bar-style visualization on an SSD1306 OLED display.

## Hardware Configuration

| Component | Pin (ESP32-S3) |
| :--- | :--- |
| OLED SDA | 41 |
| OLED SCL | 42 |
| I2S MIC WS | 4 |
| I2S MIC BCK | 5 |
| I2S MIC SD | 6 |

## Dependencies

The following libraries are required:
* **Adafruit SSD1306** & **Adafruit GFX**: For display and graphics.
* **arduinoFFT** (v1.6 or higher): For frequency analysis.

## Features

* **FFT Analysis**: Real-time processing of 128 samples at a 20kHz sampling rate.
* **Smooth Animation**: Optimized peak decay algorithm to ensure fluid motion.
* **Mirrored Design**: Symmetrical bar visualization centered on the display.
* **S3 Optimized**: Configured to utilize ESP32-S3 PSRAM for high stability.

## License
MIT
