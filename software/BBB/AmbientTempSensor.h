#ifndef AMBIENTTEMPSENSOR_H_
#define AMBIENTTEMPSENSOR_H_

#include "i2cManager.h"
#include "i2cDevice.h"

/*
 * MCP9808 High Accuracy I2C Temperature Sensor
 * Address 0x18 + A2 A1 A0
 */

class AmbientTempSensor : public i2cDevice
{

    public:
        AmbientTempSensor(i2cManager & manager);

        float getAmbientTemp();

        static float CtoF(float C);

    private:
        struct sensordata
        {
            uint16_t temperature;
        };

        sensordata data;

        void _update();
        int  _out(uint8_t * buf);

};

#endif //AMBIENTTEMPSENSOR_H_
