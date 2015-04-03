#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "deviceManager.h"
#include "device.h"

//scl - serial clock
//sda - serial data

//I2C Bus (BeagleBone Black):
//I2C2_SCL - pin 19
//I2C2_SDA - pin 20

//forward declaration of deviceManager
class deviceManager;

/*
 * i2cDevice - type of device that uses the i2c bus to communicate.
 * Adds additional i2c communication functions.
 * Must implement the usual abstract device functions.
 */

class i2cDevice : public device
{

    public:
        i2cDevice(int address, deviceManager & manager, int updateRate = 1000);

        //debatable: move these to protected?

        //i2c functions
        ssize_t i2cRead(uint8_t * data, size_t length);
        ssize_t i2cWrite(uint8_t * data, size_t length);

        //smbus functions
        int smbWrite(uint8_t value);
        int smbRead();
        int smbWriteByte(uint8_t reg, uint8_t value);
        int smbReadByte(uint8_t reg);
        int smbWriteWord(uint8_t reg, uint16_t value);
        int smbReadWord(uint8_t reg);
        int smbWriteBlock(uint8_t reg, uint8_t * data, size_t length);
        int smbReadBlock(uint8_t reg, uint8_t * data);

        int getAddress();

    protected:
        //implemented for each i2cDevice (from device):
        //  called when a device's i/o should be updated
        virtual void _update();
        //  called when a device's status/data should be placed in buf
        virtual int _out(uint8_t * buf);

    private:

        //int deviceBus; //currently unused, assumed to be 1
        int i2c_address;
        int i2c_fd;

        void setAddress();
};


#endif //I2CDEVICE_H_
