#ifndef I2CMANAGER_H_
#define I2CMANAGER_H_

//#include "i2cDevice.h"

#include <vector>

#include <iostream>

#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/ioctl.h>

//forward declaration of i2cDevice
class i2cDevice;

class i2cManager
{

    public:
        i2cManager();
        ~i2cManager();
        
        int initDevice(i2cDevice & newDevice);

    private:
        int i2c_fd;
};

#endif //I2CMANAGER_H_
