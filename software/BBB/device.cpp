#include "deviceManager.h"
#include "device.h"

device::device(deviceManager & manager, int id, int updateRate)
{
  updaterate = updateRate;
  deviceid  = id;

  _manager = &manager;
  _manager->initDevice(*this);
}

device::~device()
{
  _manager->removeDevice(*this);
}

/*
 * Returns if the device is actually connected.
 */
bool device::isConnected()
{
    return connected;
}

/*
 * Returns if the 16 bit device identifier.
 */
uint16_t device::getDeviceID()
{
    return deviceid;
}

/*
 * Returns if there is new data to send.
 */
bool device::needsUpdate()
{
    time_t t = time(0);   // get time now
    return (difftime(t, lastUpdate) * 1000 >= updaterate);
}

/*
 * Updates internal device data by calling _update and resets the update
 * timer.
 */
void device::performUpdate()
{
    lastUpdate = time(0);
    _update();
}

/*
 * This should be implemented by all derived classes to update
 * device data and connectivity status. The following is implemented
 * to allow for the use of a generic i2c object.
 */
void device::_update()
{
    connected = true;
}

/*
 * Writes a buffer with the status to send and returns the length
 * of the data written.
 */
int device::update(uint8_t * buf)
{
    performUpdate();
    return _out(buf);
}

/*
 * This should be implemented by all derived classes to output
 * the data to be sent to the server. The function should return
 * the length of the data written to the buffer (0 if none) or
 * -1 if nothing should be sent to the server (including status).
 */
int device::_out(uint8_t * buf)
{
    return 0;
}
