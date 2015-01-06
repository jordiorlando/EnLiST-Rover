#ifndef PRESSURESENSOR_H_
#define PRESSURESENSOR_H_

#include "i2cManager.h"
#include "i2cDevice.h"

/*
 * BMP085 Barometric Pressure Sensor
 * Address 0x77
 */

class PressureSensor : public i2cDevice
{

    public:
        PressureSensor(i2cManager & manager);

        float getTemperature();
        float getPressure();

    private:
        struct sensordata
        {
            uint32_t temperature;
            uint32_t pressure;
        };

        sensordata data;

        struct caldata
        {
            int16_t ac2;
            int16_t ac1;
            int16_t ac3;
            int16_t ac4;
            int16_t ac5;
            int16_t ac6;
            int16_t b1;
            int16_t b2;
            int16_t mb;
            int16_t mc;
            int16_t md;
        };

        caldata calibration;

        uint8_t samplemode;

        int state;

        void init();
        void setMode(uint8_t mode);
        void getCalibration();

        void readTemperature0();
        uint32_t readTemperature1();
        void readPressure0();
        uint32_t readPressure1();


        void _update();
        int  _out(uint8_t * buf);


};

#endif //PRESSURESENSOR_H_
