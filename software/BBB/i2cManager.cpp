#include "i2cManager.h"
#include "i2cDevice.h"

i2cManager::i2cManager(tcpSocket & server)
{
    this->server = &server;

    for (int i=0; i<BANKSIZE; i++)
        connectedDevices[i] = NULL;

    //open i2c file
    //hardcoded 1 can be changed/dynamic to use other i2c buses
    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        //open() error
        std::cout << "i2cManager open" << std::endl;
    }

}

i2cManager::~i2cManager()
{
    close(i2c_fd);
}

/*
 * Add a device to the connected devices.
 * This is only called internally by i2cDevices.
 */
int i2cManager::initDevice(i2cDevice & device)
{
    connectedDevices[device.getAddress()] = &device;

    return i2c_fd;
}

/*
 * Remove a device from the connected devices.
 * This is only called internally by i2cDevices.
 */
void i2cManager::removeDevice(i2cDevice & device)
{
    connectedDevices[device.getAddress()] = NULL;
}

/*
 * Send all updated device data to the server.
 */
void i2cManager::sendData()
{
    for (int i=0; i<BANKSIZE; i++)
    {
        if (connectedDevices[i] != NULL && connectedDevices[i]->dataReady())
        {
            //1000 bytes of buffer + 1 connection byte + 1 address byte
            unsigned char buf[1002];

            //first byte is the bank # (for future use) and 0xF0 connected bit flag
            buf[0] = connectedDevices[i]->isConnected() << 7;

            //second byte is the i2c address, indicating the type of device
            buf[1] = (unsigned char)i;

            //send status data to the server
            size_t len = connectedDevices[i]->dataOut(buf + 2);
            send(server->hostfd, buf, len + 2, 0);
        }
    }
}
