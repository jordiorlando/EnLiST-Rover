#ifndef RGBCOLORSENSOR_H_
#define RGBCOLORSENSOR_H_

#include "deviceManager.h"
#include "i2cDevice.h"

/*
 * Adafruit TCS34725 Digital Color Sensor
 * Address 0x29
 */

#define TCS34725_ADDRESS 0x29

class RGBColorSensor : public i2cDevice
{

    public:
        RGBColorSensor(deviceManager & manager);

    private:
        struct devicedata
        {
            uint16_t r;
            uint16_t g;
            uint16_t b;
            uint16_t c;
        };

        devicedata data;

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
