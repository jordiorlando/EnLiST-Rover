#ifndef RGBCOLORSENSOR_H_
#define RGBCOLORSENSOR_H_

#include "i2cManager.h"
#include "i2cDevice.h"

/*
 * Adafruit TCS34725 Digital Color Sensor
 * Address 0x29
 */

class RGBColorSensor : public i2cDevice
{

    public:
        RGBColorSensor(i2cManager & manager);

    private:
        struct sensordata
        {
            uint16_t r;
            uint16_t g;
            uint16_t b;
            uint16_t c;
        };

        sensordata data;

        int state;

        void init();
        void enable0();
        void enable1();
        void disable();
        void setIntegrationTime(uint8_t integrationtime);
        void setGain(uint8_t gain);

        void _update();
        int  _out(uint8_t * buf);

};

#endif //RGBCOLORSENSOR_H_
