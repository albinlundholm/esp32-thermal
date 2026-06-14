#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <cstdio>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_BAUD 115200

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    delay(2000);
    Serial.begin(OLED_BAUD);
    Wire.begin();
    for (int addr = 0x08; addr <= 0x77; addr++) {
        Wire.beginTransmission(addr);
        int ack = Wire.endTransmission();

        if (ack == 0) {
            Serial.printf("Found device at 0x%02X\n", addr);
        }
    }
}

void loop() {}
