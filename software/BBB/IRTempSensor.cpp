#include "IRTempSensor.h"

IRTempSensor::IRTempSensor(i2cManager & manager)
 : i2cDevice(manager, 0x5a, 1000) {}

/*
 * Returns the ambient temperature from in Celcius.
 */
float IRTempSensor::getAmbientTemp()
{
    float temp = data.ambient;
    temp = temp * 0.02 - 273.15;
    return temp;
}

/*
 * Returns object temperature in Celcius.
 */
float IRTempSensor::getObjectTemp()
{
    float temp = data.object;
    temp = temp * 0.02 - 273.15;
    return temp;
}

/*
 * Converts degrees Celcius to Fahrenheit.
 */
float IRTempSensor::CtoF(float C)
{
    return (C * 9 / 5) + 32;
}

void IRTempSensor::_update()
{
    //check connectivity
    connected = (smbRead() >= 0);
    if (!connected) return;

    //update data
    data.ambient = smbReadWord(0x6); //TA
    data.object  = smbReadWord(0x7); //TOBJ1
}

int IRTempSensor::_out(uint8_t * buf)
{
    //prepare data for sending
    buf[0] = (data.ambient >> 8) & 0xFF;
    buf[1] = (data.ambient >> 0) & 0xFF;
    buf[2] = (data.object >> 8) & 0xFF;
    buf[3] = (data.object >> 0) & 0xFF;
    return 4;
}
