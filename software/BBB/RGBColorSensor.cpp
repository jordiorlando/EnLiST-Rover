#include "RGBColorSensor.h"

RGBColorSensor::RGBColorSensor(deviceManager & manager)
 : i2cDevice(TCS34725_ADDRESS, manager, 500), state(0) {}


void RGBColorSensor::init()
{
    //set integration time
    setIntegrationTime(0xC0);

    //set gain
    setGain(0x00);

}

void RGBColorSensor::setIntegrationTime(uint8_t integrationtime)
{
    /* Integration time options:
     * 2.4ms 0xFF
     * 24ms  0xF6
     * 50ms  0xEB
     * 101ms 0xD5
     * 154ms 0xC0
     * 700ms 0x00
     */

    /*
    switch (integrationtime)
    {
        case 0xFF: updateRate = 3;
        case 0xF6: updateRate = 24;
        case 0xEB: updateRate = 50;
        case 0xD5: updateRate = 101;
        case 0xC0: updateRate = 154;
        case 0x00: updateRate = 700;
    }
    */

    smbWriteByte(0x81, integrationtime);
}

void RGBColorSensor::setGain(uint8_t gain)
{
    /* Gain options:
     * 1x  0x00
     * 4x  0x01
     * 16x 0x02
     * 60x 0x03
     */

    smbWriteByte(0x8F, gain);
}

void RGBColorSensor::enable0()
{
    //enable internal oscillator
    smbWriteByte(0x80, 0x01);
}

void RGBColorSensor::enable1()
{
    //must wait at least 3ms after enable0

    //enable ADC
    smbWriteByte(0x80, 0x02 | 0x01);
}

void RGBColorSensor::disable()
{
    //turn off oscillator and ADC
    uint8_t en = smbReadByte(0x80);
    smbWriteByte(0x80, en & 0xFC);
}

void RGBColorSensor::_update()
{
    //check connectivity
    connected = (smbRead() >= 0);
    if (!connected)
    {
        state = 0;
        return;
    }

    switch (state)
    {
        case 0:	//not initialized
            init();
            enable0();
            state = 1;
            break;
        case 1: //enable ADC
            enable1();
            state = 2;
            break;
        case 2: //read color values
            data.c = smbReadWord(0x94);
            data.r = smbReadWord(0x96);
            data.g = smbReadWord(0x98);
            data.b = smbReadWord(0x9A);
            break;
    }

}

int RGBColorSensor::_out(uint8_t * buf)
{
    //prepare data for sending
    buf[0] = (data.r >> 8) & 0xFF;
    buf[1] = (data.r >> 0) & 0xFF;
    buf[2] = (data.g >> 8) & 0xFF;
    buf[3] = (data.g >> 0) & 0xFF;
    buf[4] = (data.b >> 8) & 0xFF;
    buf[5] = (data.b >> 0) & 0xFF;
    buf[6] = (data.c >> 8) & 0xFF;
    buf[7] = (data.c >> 0) & 0xFF;
    return 8;
}
