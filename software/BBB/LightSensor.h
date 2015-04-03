#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_

#include "deviceManager.h"
#include "i2cDevice.h"

/*
 * Si1145 UV/IR/Visible Light Sensor
 * Address 0x60
 */

#define SI1145_ADDRESS 0x60

class LightSensor : public i2cDevice
{

  public:
    LightSensor(deviceManager & manager);

    uint16_t getUV();
    uint16_t getVisible();
    uint16_t getIR();

  private:
    struct devicedata
    {
        uint16_t uv;
        uint16_t visible;
        uint16_t ir;
    };

    devicedata data;

    void init();



    void _update();
    int  _out(uint8_t * buf);


};

#endif //LIGHTSENSOR_H_
