#include "MyCustomClass.h"
#include "allocateMem.h"
#include "commandmessenger.h"

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

AltimeterAerosonic::AltimeterAerosonic(uint8_t addressSettingDisplay, uint8_t addressAltitudeDisplay)
{
    _addressDisplaySetting   = addressSettingDisplay;
    _addressDisplayAltitude  = addressAltitudeDisplay;
}

void AltimeterAerosonic::begin()
{
    _displayAltimeterSetting->begin();
    _displayAltimeterSetting->clearBuffer();
    setAltimeterSetting("2899.9");
    _displayAltimeterSetting->sendBuffer();
}

void AltimeterAerosonic::attach()
{
     /* **********************************************************************************
        Next call the constructor of your custom device
        adapt it to the needs of your constructor
    ********************************************************************************** */
    if (!FitInMemory(sizeof(U8G2_SSD1306_128X64_NONAME_F_HW_I2C))) {
        // Error Message to Connector
        cmdMessenger.sendCmd(kStatus, F("Custom Device does not fit in Memory"));
        return;
    }

    _displayAltimeterSetting = new (allocateMemory(sizeof(U8G2_SSD1306_128X64_NONAME_F_HW_I2C))) U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
    // _displayAltimeterSetting->setI2CAddress(0x3C);
    //_displayAltimeterAltitude = new (allocateMemory(sizeof(U8G2_SSD1306_128X64_NONAME_F_HW_I2C))) U8G2_SSD1306_128X64_NONAME_F_HW_I2C  (U8G2_R0);
    //_displayAltimeterAltitude->setI2CAddress(_addressDisplayAltitude);
}

void AltimeterAerosonic::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void AltimeterAerosonic::set(int16_t messageID, char *rawData)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -1 will be send from the connector when Mobiflight is closed
        Put in your code to shut down your custom device (e.g. clear a display)
        MessageID == -2 will be send from the connector when PowerSavingMode is entered
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
        blankSettingsArea();
        break;
    case -2:
        // tbd., get's called when PowerSavingMode is entered
        blankSettingsArea();
        break;
    case 0:
        setAltimeterSetting(rawData);
        break;
    case 1:
        /* code */
        setAltimeterAltitude(rawData);
        break;
    case 2:
        /* code */
        break;
    default:
        break;
    }
}

void AltimeterAerosonic::update()
{
    // Do something which is required regulary
}

void AltimeterAerosonic::blankSettingsArea()
{
    _displayAltimeterSetting->setDrawColor(0);
    _displayAltimeterSetting->drawBox(0, 0, 128, 32);
    _displayAltimeterSetting->setDrawColor(1);
}

void AltimeterAerosonic::clipSettingsArea()
{
    _displayAltimeterSetting->setDrawColor(0);
    _displayAltimeterSetting->drawBox(0, 0, 128, 5);
    _displayAltimeterSetting->drawBox(0, 32, 128, 32);
    _displayAltimeterSetting->setDrawColor(1);
}

float AltimeterAerosonic::calculateDigitOffset(float value, uint8_t digit)
{
    // the float value holds the value of the altimeter setting
    // we are only displaying the first 4 digits, no decimals
    // but the digits will be displaced slight according to the value
    // to simulate a mechanical altimeter
    // the digit parameter is the digit we are calculating the offset for
    // 1 is the least significant digit, 4 is the most significant digit
    // the offset is calculated in pixels
    float offset = ((int16_t)(value*10) % 10) / 10.0f;
    if (digit == 2 && (int16_t)value % 10 < 9) return 0;
    if (digit == 1 && (int16_t)value % 100 < 99) return 0;
    if (digit == 0 && (int16_t)value % 1000 < 999) return 0;
    return offset;
}

void AltimeterAerosonic::drawSettingsDigit(const char *value, uint8_t digit, uint8_t fontSize, uint8_t x, uint8_t y) {
    // first digit    
    float data = atof(value);
    char buffer[2];
    buffer[1] = '\0';
    buffer[0] = value[digit];
    uint8_t nextDigit = (atoi(buffer) + 1)%10;
    float digitOffset = calculateDigitOffset(data, digit);

    _displayAltimeterSetting->drawStr(18 * (digit+1), 32 - digitOffset * fontSize, buffer);
    itoa(nextDigit, buffer, 10);
    _displayAltimeterSetting->drawStr(18 * (digit+1), 32 + (1-digitOffset) * fontSize + 2, buffer);
}

void AltimeterAerosonic::setAltimeterSetting(const char *value)
{
    float data = atof(value);
    cmdMessenger.sendCmd(kStatus,data);
    _displayAltimeterSetting->clearBuffer();
    blankSettingsArea();
    // copy the first character from value to a local buffer
    char buffer[2];
    buffer[1] = '\0';

    _displayAltimeterSetting->setFont(u8g2_font_logisoso26_tn);
    uint8_t fontSize = 26;
    
    // first digit    
    drawSettingsDigit(value, 0, fontSize, 0, 0);
    drawSettingsDigit(value, 1, fontSize, 0, 0);
    drawSettingsDigit(value, 2, fontSize, 0, 0);
    drawSettingsDigit(value, 3, fontSize, 0, 0);

    // // buffer[0] = value[0];
    // // uint8_t nextDigit = (atoi(buffer) + 1)%10;
    // // _displayAltimeterSetting->drawStr(18, 32 - calculateDigitOffset(data, 4) * fontSize, buffer);
    // // itoa(nextDigit, buffer, 10);
    // // _displayAltimeterSetting->drawStr(18, 32 + (1-calculateDigitOffset(data, 4)) * fontSize + 2, buffer);
    
    // // second digit
    // buffer[0] = value[1];
    // nextDigit = atoi(buffer) + 1;
    // _displayAltimeterSetting->drawStr(36, 32 - calculateDigitOffset(data, 3) * fontSize, buffer);
    // itoa(nextDigit, buffer, 10);
    // _displayAltimeterSetting->drawStr(36, 32 + (1-calculateDigitOffset(data, 3)) * fontSize + 2, buffer);

    // // third digit
    // buffer[0] = value[2];
    // _displayAltimeterSetting->drawStr(54, 32 - calculateDigitOffset(data, 2) * fontSize, buffer);
    
    // // third digit (next)
    // itoa(int16_t(data/10+1)%10, buffer, 10);
    // _displayAltimeterSetting->drawStr(54, 32 + (1-calculateDigitOffset(data, 2)) * fontSize + 2, buffer);

    // // fourth digit
    // buffer[0] = value[3];
    // _displayAltimeterSetting->drawStr(72, 32 - calculateDigitOffset(data, 1) * fontSize, buffer);
    // // fourth digit
    // itoa(int16_t(data+1)%10, buffer, 10);
    // _displayAltimeterSetting->drawStr(72, 32 + (1-calculateDigitOffset(data, 1)) * fontSize + 2, buffer);

    clipSettingsArea();

    _displayAltimeterSetting->sendBuffer();
}

void AltimeterAerosonic::setAltimeterAltitude(const char *value)
{
    _displayAltimeterAltitude->setFont(u8g2_font_logisoso26_tn);
    _displayAltimeterAltitude->drawStr(0, 0, value);
    _displayAltimeterAltitude->sendBuffer();
}
