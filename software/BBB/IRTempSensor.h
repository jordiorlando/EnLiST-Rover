#ifndef IRTEMPSENSOR_H_
#define IRTEMPSENSOR_H_

#include "i2cManager.h"
#include "i2cDevice.h"

/*
 * MLX90614 IR Temperature Sensor
 * Address 0x5A
 */

class IRTempSensor : public i2cDevice
{

    public:
        IRTempSensor(i2cManager & manager);

        float getAmbientTemp();
        float getObjectTemp();

        static float CtoF(float C);

    private:
        struct sensordata
        {
            uint16_t ambient;
            uint16_t object;
        };

        sensordata data;

        void _update();
        int  _out(uint8_t * buf);

};

#endif //IRTEMPSENSOR_H_
