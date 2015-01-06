#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

#include "i2cManager.h"
#include "i2cDevice.h"

/*
 * Custom uC motor controller.
 * Address 0x40 + wheel#
 */

class MotorController : public i2cDevice
{

    public:
        MotorController(i2cManager & manager, int wheel);

        void setSpeed(int speed);
        void setLamp(uint8_t brightness);
        void setStall(bool stall);

    private:
        int wheelnum;

        struct sensordata
        {
            uint16_t speed;
            uint8_t brightness;
        };

        sensordata data;

        /*
        void _update();
        int  _out(uint8_t * buf);
        */
};

#endif //MOTORCONTROLLER_H_
