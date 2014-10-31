#include "i2cManager.h"

i2cManager::i2cManager()
{

    //open i2c file
    //hardcoded 3 can be changed/dynamic to use other i2c buses
    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        //open() error
        std::cout << "open" << std::endl;
    }

}

i2cManager::~i2cManager()
{
    close(i2c_fd);
}

int i2cManager::initDevice(i2cDevice & newDevice)
{
    //maybe do something for tracking or error checking
    
    return i2c_fd;
}