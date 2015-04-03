#ifndef LCD_H_
#define LCD_H_

#include "i2cDevice.h"

/*
 * SainSmart LCD2004 20x4 LCD display
 */

class LCD : public i2cDevice
{

    public:
        LCD(i2cManager & manager);

        void init();
        void clear();
        void home();
        void setCursor(uint8_t col, uint8_t row);
        void setBacklight(bool val);
        void print(std::string data);

    private:
        void command(uint8_t value);
        void write(uint8_t value);
        void send(uint8_t value, uint8_t mode);
        void write4bits(uint8_t value);
        void expanderWrite(uint8_t _data);
        void pulseEnable(uint8_t _data);

        uint8_t backlight;

};

#endif //LCD_H_
