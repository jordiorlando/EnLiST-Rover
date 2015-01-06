#include "PressureSensor.h"

PressureSensor::PressureSensor(i2cManager & manager)
 : i2cDevice(manager, 0x77), state(0) {}

/*
 * Returns the temperature from in Celcius.
 */
float PressureSensor::getTemperature()
{
    return 0;
}

/*
 * Returns the pressure in kPa.
 */
float PressureSensor::getPressure()
{
    return 0;
}

void PressureSensor::init()
{
    //set sample mode
    setMode(0x2);

    //read factory calibration
    getCalibration();
}

void PressureSensor::setMode(uint8_t mode)
{
    /* Mode options:
     * Ultra low power 0x0 4.5ms
     * Standard        0x1 7.5ms
     * High res        0x2 13.5ms
     * Ultra high res  0x3 25.5ms
     */

    samplemode = mode;
}

void PressureSensor::getCalibration()
{
    calibration.ac1 = smbReadWord(0xAA);
    calibration.ac2 = smbReadWord(0xAC);
    calibration.ac3 = smbReadWord(0xAE);
    calibration.ac4 = smbReadWord(0xB0);
    calibration.ac5 = smbReadWord(0xB2);
    calibration.ac6 = smbReadWord(0xB4);
    calibration.b1  = smbReadWord(0xB6);
    calibration.b2  = smbReadWord(0xB8);
    calibration.mb  = smbReadWord(0xBA);
    calibration.mc  = smbReadWord(0xBC);
    calibration.md  = smbReadWord(0xBE);
}

void PressureSensor::readTemperature0()
{
    //send command to read temperature
}

uint32_t PressureSensor::readTemperature1()
{
    //wait at least 4.5ms before reading
    return 0;
}

void PressureSensor::readPressure0()
{
    //send command to read pressure
}

uint32_t PressureSensor::readPressure1()
{
    //wait depending on mode before reading
    return 0;
}


void PressureSensor::_update()
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
            init();
            readTemperature0();
            state = 1;
            break;
        case 1:
            readTemperature1();
            readPressure0();
            state = 2;
            break;
        case 2:
            readPressure1();
            readTemperature0();
            state = 1;
            break;
    }
}

int PressureSensor::_out(uint8_t * buf)
{
    //prepare data for sending
    buf[0] = (data.temperature >> 24) & 0xFF;
    buf[1] = (data.temperature >> 16) & 0xFF;
    buf[2] = (data.temperature >> 8 ) & 0xFF;
    buf[3] = (data.temperature >> 0 ) & 0xFF;
    buf[4] = (data.pressure >> 24) & 0xFF;
    buf[5] = (data.pressure >> 16) & 0xFF;
    buf[6] = (data.pressure >> 8 ) & 0xFF;
    buf[7] = (data.pressure >> 0 ) & 0xFF;
    return 8;
}
