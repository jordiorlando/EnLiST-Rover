#include "LCD.h"

LCD::LCD(i2cManager & manager) : i2cDevice(manager, 0x3f), backlight(0x8) {}

//high level commands

void LCD::init()
{
    uint8_t _displayfunction = 0, _displaycontrol = 0, _displaymode = 0;

    // Now we pull both RS and R/W low to begin commands
    expanderWrite(backlight);	// reset expanderand turn backlight off (Bit 8 =1)
    usleep(1000 * 1000);

    //put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    usleep(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    usleep(4500); // wait min 4.1ms

    // third go!
    write4bits(0x03);
    usleep(150);

    // finally, set to 4-bit interface
    write4bits(0x02);

    // set # lines, font size, etc.
    _displayfunction |= 0x08; //2 line mode
    command(0x20 | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = 0x04 | 0x00 | 0x00;
    command(0x08 | _displaycontrol);

    // clear the LCD
    clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = 0x02 | 0x00;
    command(0x04 | _displaymode);

    home();

}

void LCD::clear()
{
    command(0x01);
    usleep(2000);
}

void LCD::home()
{
    command(0x02);
    usleep(2000);
}

void LCD::setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    command(0x80 | (col + row_offsets[row]));
}

void LCD::setBacklight(bool val)
{
    backlight = val ? 0x8 : 0x0;
    expanderWrite(0);
}

void LCD::print(std::string data)
{
    for(int i=0; i<data.size(); i++)
    {
        write(data[i]);
    }
}

//low level commands

void LCD::command(uint8_t value)
{
    send(value, 0x00);
}

void LCD::write(uint8_t value)
{
    send(value, 0x01);
}

void LCD::send(uint8_t value, uint8_t mode)
{
    uint8_t high = value & 0xf0;
    uint8_t low = (value << 4) & 0xf0;

    write4bits(high | mode);
    write4bits(low  | mode);
}

void LCD::write4bits(uint8_t value)
{
    expanderWrite(value);
    pulseEnable(value);
}

void LCD::expanderWrite(uint8_t _data)
{
    smbWrite(_data | backlight);
}

void LCD::pulseEnable(uint8_t _data)
{
    expanderWrite(_data | 0x04);
    usleep(1);

    expanderWrite(_data & ~0x04);
    usleep(50);
}
