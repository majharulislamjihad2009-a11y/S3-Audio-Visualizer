#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <arduinoFFT.h>
#include "driver/i2s.h"

#define MIC_WS 4
#define MIC_BCK 5
#define MIC_SD 6
#define OLED_SDA 41
#define OLED_SCL 42

#define SAMPLES 128
#define SAMPLE_RATE 20000   
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

double vReal[SAMPLES];
double vImag[SAMPLES];
int peak[64]; 

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLE_RATE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Wire.begin(OLED_SDA, OLED_SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    i2s_config_t mic_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 4,
        .dma_buf_len = SAMPLES
    };
    i2s_pin_config_t mic_pins = { .bck_io_num = MIC_BCK, .ws_io_num = MIC_WS, .data_out_num = -1, .data_in_num = MIC_SD };
    i2s_driver_install(I2S_NUM_0, &mic_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &mic_pins);
}

void loop() {
    size_t bytesRead;
    int32_t raw_samples[SAMPLES];
    i2s_read(I2S_NUM_0, &raw_samples, sizeof(raw_samples), &bytesRead, portMAX_DELAY);

    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = (double)(raw_samples[i] >> 11); // Sensitivity remains same as original
        vImag[i] = 0;
    }

    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    display.clearDisplay();
    
    int centerY = SCREEN_HEIGHT / 2;
    int numBars = 30;
    
    for (int i = 0; i < numBars; i++) {
        // বাম পাশের বারগুলোর (i < 10) জন্য ডিভাইডার বাড়িয়ে সেন্সিটিভিটি কমানো হয়েছে
        int currentDivider = (i < 10) ? 700 : 450; 
        
        int barHeight = (int)vReal[i + 2] / currentDivider;
        if (barHeight > centerY) barHeight = centerY;

        // Smooth decay logic
        if (barHeight < peak[i]) peak[i] -= 2;
        else peak[i] = barHeight;

        int x = i * 4; // Spacing between bars
        int h = peak[i];

        if (h > 0) {
            // ১. মূল মাঝখানের লাইন থেকে উপরে এবং নিচে ড্রয়িং
            display.drawLine(x, centerY - h, x, centerY + h, WHITE);
            
            // ২. স্টিকের মাথায় ছোট "Floating" ডট
            display.drawPixel(x, centerY - h - 2, WHITE);
            display.drawPixel(x, centerY + h + 2, WHITE);
            
            // ৩. এক্সট্রা ডিটেইল: বেসের দিকে মোটা কাঠি
            if(i < 8) {
                display.drawLine(x, centerY - h, x, centerY + h, WHITE);
            }
        }
    }
    
    // স্ক্রিনের একদম মাঝখানে একটা হালকা হরাইজন্টাল লাইন
    display.drawFastHLine(0, centerY, SCREEN_WIDTH, WHITE);
    
    display.display();
}