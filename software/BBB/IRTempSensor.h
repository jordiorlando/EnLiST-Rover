#ifndef IRTEMPSENSOR_H_
#define IRTEMPSENSOR_H_

#include "deviceManager.h"
#include "i2cDevice.h"

/*
 * MLX90614 IR Temperature Sensor
 * Address 0x5A
 */

#define MLX90614_ADDRESS 0x5A

class IRTempSensor : public i2cDevice
{

    public:
        IRTempSensor(deviceManager & manager);

        float getAmbientTemp();
        float getObjectTemp();

        static float CtoF(float C);

    private:
        struct devicedata
        {
            uint16_t ambient;
            uint16_t object;
        };

        devicedata data;

        void _update();
        int  _out(uint8_t * buf);

};

#endif //IRTEMPSENSOR_H_
