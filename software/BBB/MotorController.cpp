#include "MotorController.h"

MotorController::MotorController(i2cManager & manager, int wheel)
 : i2cDevice(manager, 0x42 + wheel), wheelnum(wheel) {}

//TODO: only write values to i2c on poll

/*
 * Set the motor speed from -255 to 255.
 */
void MotorController::setSpeed(int speed)
{
    uint8_t reg0 = 0, reg1 = 0;

    if (speed < 0)
    {
        reg0 = 1;
        speed *= -1;
    }
    reg1 = speed & 0xFF;

    smbWriteByte(0x0, reg0);
    smbWriteByte(0x1, reg1);
}

/*
 * Set the LED lamp brightness from 0 to 255.
 */
void MotorController::setLamp(uint8_t brightness)
{
    smbWriteByte(0x5, brightness);
}

void MotorController::setStall(bool stall)
{
    smbWriteByte(0x2, 0x1 * stall);
}

/*
void MotorController::_update()
{

}

int MotorController::_out(uint8_t * buf)
{
    return 0;
}
*/
