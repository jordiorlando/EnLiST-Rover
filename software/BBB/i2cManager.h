#ifndef I2CMANAGER_H_
#define I2CMANAGER_H_

#include <vector>
#include <iostream>

#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>      //IMPORTANT: need the libi2c-dev version, not the kernel version
#include <unistd.h>
#include <sys/ioctl.h>

#include <ctime>

#include "tcpSocket.h"          //sockets for sending data stream

#define BANKSIZE 0x8*0x10

//forward declaration of i2cDevice
class i2cDevice;

class i2cManager
{

    public:
        i2cManager(tcpSocket & server);
        ~i2cManager();

        int initDevice(i2cDevice & device);
        void removeDevice(i2cDevice & device);
        void sendData();

    private:
        int         i2c_fd;
        i2cDevice * connectedDevices[BANKSIZE]; //addresses 0x03-0x77

        tcpSocket * server;

};

#endif //I2CMANAGER_H_
