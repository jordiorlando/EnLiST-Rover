#ifndef HUMIDITYSENSOR_H_
#define HUMIDITYSENSOR_H_

#include "deviceManager.h"
#include "i2cDevice.h"

/*
 * HTU21DF Humidity & Temp Sensor
 * Address 0x40
 */

 #define HTU21DF_ADDRESS 0x40

class HumiditySensor : public i2cDevice
{

  public:
    HumiditySensor(deviceManager & manager);

    float getTemperature();
    float getHumidity();

  private:
    struct devicedata
    {
        uint16_t temperature;
        uint16_t humidity;
    };

    devicedata data;

    int state;

    void init();

    void _update();
    int  _out(uint8_t * buf);


};

#endif //HUMIDITYSENSOR_H_
