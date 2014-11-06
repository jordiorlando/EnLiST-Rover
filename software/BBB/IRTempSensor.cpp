#include "IRTempSensor.h"

IRTempSensor::IRTempSensor(i2cManager & manager) : i2cDevice(manager, 0x5a) {}

/* 
 * Returns the ambient temperature from the sensor in Celcius.
 */
float IRTempSensor::getAmbientTemp()
{
    float temp = smbReadWord(0x6); //TA
    temp = temp * 0.02 - 273.15;
    return temp;
}

/*
 * Returns object 1 temperature from the sensor in Celcius.
 */
float IRTempSensor::getObjectTemp()
{
    float temp = smbReadWord(0x7); //TOBJ1
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