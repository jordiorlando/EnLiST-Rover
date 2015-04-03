#ifndef DEVICEMANAGER_H_
#define DEVICEMANAGER_H_

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
class device;

class deviceManager
{

    public:
        deviceManager(tcpSocket & server);
        ~deviceManager();

        int initDevice(device & d);
        void removeDevice(device & d);
        int geti2cBus();
        void sendData();

    private:
        //file descriptor for the i2c bus
        int i2c_fd;

        //i2cDevice * i2cPorts[BANKSIZE]; //addresses 0x03-0x77
        std::vector<device *> devices;

        tcpSocket * server;

};

#endif //DEVICEMANAGER_H_
