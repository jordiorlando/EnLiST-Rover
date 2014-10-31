#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "i2cManager.h"

#include <fcntl.h>

#include <iostream>

//scl - serial clock
//sda - serial data 

//I2C Bus 3:
//I2C2_SCL - pin 19 
//I2C2_SDL - pin 20

class i2cDevice
{

    public:
        i2cDevice(i2cManager & manager, int address);
        ~i2cDevice();
        
        std::vector<unsigned char> i2cRead(unsigned char reg, size_t length);
        void i2cWrite(unsigned char reg, std::vector<unsigned char> & data);
        
    private:
        //i2cManager & deviceManager;
        int deviceAddress;
        int deviceBus; //currently unused, assumed to be 3
        int i2c_fd;
        
        void setAddress();

};

#endif //I2CDEVICE_H_
