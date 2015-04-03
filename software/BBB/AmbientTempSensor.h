#ifndef AMBIENTTEMPSENSOR_H_
#define AMBIENTTEMPSENSOR_H_

#include "deviceManager.h"
#include "i2cDevice.h"

/*
 * MCP9808 High Accuracy I2C Temperature Sensor
 * Address 0x18 + A2 A1 A0
 */

#define MCP9808_ADDRESS 0x18

class AmbientTempSensor : public i2cDevice
{

  public:
    AmbientTempSensor(deviceManager & manager);

    float getAmbientTemp();

    static float CtoF(float C);

  private:
    struct devicedata
    {
        uint16_t temperature;
    };

    devicedata data;

    void _update();
    int  _out(uint8_t * buf);

};

#endif //AMBIENTTEMPSENSOR_H_
