#include "AmbientTempSensor.h"

AmbientTempSensor::AmbientTempSensor(deviceManager & manager)
 : i2cDevice(MCP9808_ADDRESS, manager, 500) {}

/*
 * Returns the ambient temperature in Celcius.
 */
float AmbientTempSensor::getAmbientTemp()
{
    float temp = ((data.temperature & 0x0F) << 8) | (data.temperature >> 8);
    temp /= 16.0;
    if (data.temperature & 0x0010) temp -= 256;

    return temp;
}

/*
 * Converts degrees Celcius to Fahrenheit.
 */
float AmbientTempSensor::CtoF(float C)
{
    return (C * 9 / 5) + 32;
}

void AmbientTempSensor::_update()
{
    //check connectivity
    connected = (smbRead() >= 0);
    if (!connected) return;

    //update data
    data.temperature = smbReadWord(0x05);
}

int AmbientTempSensor::_out(uint8_t * buf)
{
    buf[0] = (data.temperature >> 8) & 0xFF;
    buf[1] = (data.temperature >> 0) & 0xFF;
    return 2;
}
