#include "HumiditySensor.h"

HumiditySensor::HumiditySensor(deviceManager & manager)
: i2cDevice(HTU21DF_ADDRESS, manager, 250), state(0) {}

/*
 * Returns the temperature in Celcius.
 */
float HumiditySensor::getTemperature()
{
    float temp = data.temperature;
    temp = temp * 175.72 / 65536 - 46.85;
    return temp;
}

/*
 * Returns the relative humidity in %RH.
 */
float HumiditySensor::getHumidity()
{
    float temp = data.humidity;
    temp = temp * 125 / 65536 - 6;
    return temp;
}

void HumiditySensor::init()
{
    //send reset command
    smbWrite(0xFE);
}

void HumiditySensor::_update()
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
            state = 1;
            break;
        case 1: //read temp
            smbWrite(0xF3);
            state = 2;
            break;
        case 2: //read temp value
            uint8_t buf[3];
            i2cRead(&buf, 3);
            data.temperature = (buf[0] << 8) | buf[1];
            state = 3;
            break;
        case 3: //read humidity
            smbWrite(0xF5);
            state = 4;
            break;
        case 4: //read humidity value
            uint8_t buf[3];
            i2cRead(&buf, 3);
            data.humidity = (buf[0] << 8) | buf[1];
            state = 1;
            break;
    }

    //humidity must be measured before being read
    //it takes at most 50ms for temp and 16ms for humidity
    //command for read temp: 0xF3, read humidity: 0xF5
    //after delay, read 2 bytes

}

int HumiditySensor::_out(uint8_t * buf)
{
    buf[0] = (data.humidity >> 8) & 0xFF;
    buf[1] = (data.humidity >> 0) & 0xFF;
    buf[2] = (data.temperature >> 8) & 0xFF;
    buf[3] = (data.temperature >> 0) & 0xFF;
    return 4;
}
