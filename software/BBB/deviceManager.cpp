#include "deviceManager.h"
#include "device.h"

deviceManager::deviceManager(tcpSocket & server)
{
    this->server = &server;

    // for (int i=0; i<BANKSIZE; i++)
    //     connectedDevices[i] = NULL;

    //open i2c file
    //hardcoded 1 can be changed/dynamic to use other i2c buses
    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        //open() error
        std::cout << "deviceManager open" << std::endl;
    }

}

deviceManager::~deviceManager()
{
    close(i2c_fd);
}

/*
 * Add a device to the connected devices.
 * This is only called internally by i2cDevices.
 */
int deviceManager::initDevice(device & d)
{
    devices.push_back(&d);
    return 0;
}

int deviceManager::geti2cBus()
{
  return i2c_fd;
}

/*
 * Remove a device from the connected devices.
 * This is only called internally by i2cDevices.
 */
void deviceManager::removeDevice(device & d)
{
    for (size_t i=0; i<devices.size(); i++)
    {
        if (devices[i] == &d)
        {
            devices.erase(devices.begin() + i);
            break;
        }
    }
}

/*
 * Send all updated device data to the server.
 */
void deviceManager::sendData()
{
  //1000 bytes of buffer + 1 connection byte + 1 address byte
  unsigned char buf[1002];

  /*
   * Performance considerations
   *
   * Writing each device's data straight to the wire (which tends to happen
   * if latency is low enough) has a lot of overhead and is all around a bad
   * idea.
   *
   * The current implementation uses tht TCP_CORK option to postpone send calls
   * until the socket is uncorked (or the buffer is full). This still isn't
   * totally ideal due to the heavy use of system calls, but is far better than
   * before.
   *
   * Ideally a chunk of memory would be allocated and managed by the program
   * or each device and then all written at once (or writev() for separate
   * buffers). The option TCP_NODELAY disables Nagle's algorithm and sends
   * data immediately, which, aside from packet boundary alignment, is nearly
   * ideal if done correctly.
   *
   * But for a reasonable number of devices, this should probably be fine.
   * However it is worth noting that data gets sent to the server and then
   * reflected to all of the websocket clients, so optimization is noticable.
   */

  //cork tcp socket
  int state = 1;
  setsockopt(server->hostfd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));

  int len;

  for (int i=0; i<devices.size(); i++)
  {
    if (devices[i]->needsUpdate())
    {
        //update the device's data
        if ((len = devices[i]->update(buf + 2)) < 0) continue;

        //first bit is the connected flag
        buf[0] = devices[i]->isConnected() << 7;

        //remaining 15 bits of the first two bytes is the device id
        uint16_t id = devices[i]->getDeviceID();
        buf[0] |= (id >> 8) & 0x7F;
        buf[1] = id & 0xFF;

        //send data to the server
        send(server->hostfd, buf, (size_t)len + 2, 0);
    }
  }

  //uncork tcp socket (flush send buffer)
  state = 0;
  setsockopt(server->hostfd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
}
