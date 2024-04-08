#pragma once

#include "Arduino.h"

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class AltimeterAerosonic
{
public:
    AltimeterAerosonic(uint8_t addressDisplaySetting, uint8_t addressDisplayAltitude);
    void begin();
    void attach();
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C *_displayAltimeterSetting;
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C *_displayAltimeterAltitude;
    bool    _initialised;
    uint8_t _addressDisplaySetting, _addressDisplayAltitude;
    long lastUpdateInMs = 0;
    void setAltimeterSetting(const char *value);
    void setAltimeterAltitude(const char *value);
    void blankSettingsArea();
    void clipSettingsArea();
    void clipAltitudeArea(uint8_t fontSize, uint8_t baseline);
    float calculateDigitOffset(float value, uint8_t digit);
    void drawSettingsDigit(const char *value, uint8_t digit, uint8_t fontSize, uint8_t x, uint8_t y);
    void drawAltitudeDigit(const char *value, uint8_t digit, uint8_t fontSize, uint8_t x, uint8_t y);
};