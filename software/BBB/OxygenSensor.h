#ifndef OXYGENSENSOR_H_
#define OXYGENSENSOR_H_

#include "deviceManager.h"
#include "device.h"

//for random numbers
#include <time.h>
#include <stdlib.h>



#define VO2SENSOR_ADDRESS 0x101

/*
 * Virtual oxygen sensor.
 */
class OxygenSensor : public device
{

  public:
    OxygenSensor(deviceManager & manager);

    void changeO2Level(float level);

  protected:
    void _update();
    int _out(uint8_t * buf);

  private:

    struct devicedata
    {
      float oxygenlevel;
    };

    devicedata data;

};


#endif //OXYGENSENSOR_H_
