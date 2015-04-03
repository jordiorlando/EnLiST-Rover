#include "OxygenSensor.h"

OxygenSensor::OxygenSensor(deviceManager & manager) :
device(manager, VO2SENSOR_ADDRESS, 1000)
{
  //this device is always connected
  connected = true;

  //seed rng
  srand(time(NULL));

  //20.9, for example, is a typical value for partial pressure %
  //  of oxygen on Earth at sea level
  data.oxygenlevel = 20.9;

}

/*
 * Template:
 * Primitive function to changethe oxygen level directly. If there
 * is any calculating to do for simulation, such as altitude, etc., this class
 * should do it and have a function e.g. setAltitude(float height)
 */
void OxygenSensor::changeO2Level(float level)
{
  data.oxygenlevel += level;
}

void OxygenSensor::_update()
{
  //TODO: something perhaps...
}

int OxygenSensor::_out(uint8_t * buf)
{
  float measurement = data.oxygenlevel;

  //add a random error to the measurement
  measurement += measurement * 0.001 * (float)((rand() % 20) - 10);

  //puts a 32 bit float into the buffer (little endian)
  //0     1      2       3
  //[7:0] [15:8] [23:16] [31:24]
  //only necessary for the reciever (otherwise it will cleanly cast back to float)
  *(float *)buf = measurement;

  return 4;
}
