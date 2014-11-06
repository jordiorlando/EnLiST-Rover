#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "i2cManager.h"

//scl - serial clock
//sda - serial data 

//I2C Bus (BeagleBone Black):
//I2C2_SCL - pin 19 
//I2C2_SDA - pin 20

class i2cDevice
{

    public:
        i2cDevice(i2cManager & manager, int address);
        ~i2cDevice();
        
        ssize_t i2cRead(uint8_t * data, size_t length);
        ssize_t i2cWrite(uint8_t * data, size_t length);
        
        int smbWrite(uint8_t value);
        int smbRead();
        int smbWriteByte(uint8_t reg, uint8_t value);
        int smbReadByte(uint8_t reg);
        int smbWriteWord(uint8_t reg, uint16_t value);
        int smbReadWord(uint8_t reg);
        int smbWriteBlock(uint8_t reg, uint8_t * data, size_t length);
        int smbReadBlock(uint8_t reg, uint8_t * data);
        
    private:
        //i2cManager & deviceManager;
        //int deviceBus; //currently unused, assumed to be 1
        int deviceAddress;
        int i2c_fd;
        
        void setAddress();

};


#endif //I2CDEVICE_H_
