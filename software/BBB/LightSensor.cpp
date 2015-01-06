#include "LightSensor.h"

LightSensor::LightSensor(i2cManager & manager)
 : i2cDevice(manager, 0x60, 500), state(0) {}

/*
 * Returns the UV index.
 */
float LightSensor::getUV()
{
    return data.uv / 100;
}

/*
 * Returns visible + IR light level.
 */
float LightSensor::getVisible()
{
    return 0;
}

/*
 * Returns IR light level.
 */
float LightSensor::getIR()
{
    return 0;
}

void LightSensor::reset0()
{

    //delay 10ms
}

void LightSensor::reset1()
{

    //delay 10ms
}

void LightSensor::init()
{

}

void LightSensor::_update()
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
        case 0:
            reset0();
            state = 1;
            break;
        case 1:
            reset1();
            state = 2;
            break;
        case 2:
            init();
            state = 3;
            break;
        case 3:
            data.uv      = smbReadWord(0x2C);
            data.visible = smbReadWord(0x22);
            data.ir      = smbReadWord(0x24);
            break;
    }

}

int LightSensor::_out(uint8_t * buf)
{
    //prepare data for sending
    buf[0] = (data.uv >> 8) & 0xFF;
    buf[1] = (data.uv >> 0) & 0xFF;
    buf[2] = (data.visible >> 8) & 0xFF;
    buf[3] = (data.visible >> 0) & 0xFF;
    buf[4] = (data.ir >> 8) & 0xFF;
    buf[5] = (data.ir >> 0) & 0xFF;
    return 6;
}
