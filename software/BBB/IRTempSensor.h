#ifndef IRTEMPSENSOR_H_
#define IRTEMPSENSOR_H_

#include "i2cDevice.h"

/*
 * Adafruit MLX90614 IR temperature sensor
 */

class IRTempSensor : public i2cDevice
{

    public:
        IRTempSensor(i2cManager & manager);
        
        float getAmbientTemp();
        float getObjectTemp();
        
        float CtoF(float C);

};

#endif //IRTEMPSENSOR_H_
