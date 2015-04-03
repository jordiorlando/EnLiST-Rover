#include <iostream>

//usleep
#include <unistd.h>

//tcp
#include "tcpSocket.h"      //for connecting to the server

//device manager
#include "deviceManager.h"
#include "device.h"
#include "i2cDevice.h"

//devices
#include "AmbientTempSensor.h"
#include "RGBColorSensor.h"
#include "IRTempSensor.h"
#include "OxygenSensor.h"

#include "WheelController.h"

//controller input
#include "controllerInput.h"

//signal handler
#include <signal.h>

/*
 * Main application intended to run on the client device.
 * -Connects to the server.
 * -Uses incoming controller data to control connected devices.
 * -Sends periodic status reports from each connected device.
 */

static int running = 1;
void sig_handler(int sig)
{
  running = 0;
}

int main()
{
  signal(SIGINT, sig_handler);
  std::cout << "starting..." << std::endl;

  //connect to server
  //192.168.7.1
  //10.10.0.1
  tcpSocket server("192.168.7.1", 2222);

  //create device manager
  deviceManager manager(server);

  //create devices
  AmbientTempSensor ats(manager);
  RGBColorSensor rgb(manager);
  IRTempSensor ir(manager);
  OxygenSensor os(manager);

  //set up wheels
  WheelController::rovermotion rmotion;
  rmotion.drivemode = DRIVEMODE_NORMAL;
  rmotion.maxradius = 0;

  WheelController wheels[6]
    {
      {manager, 0, rmotion},
      {manager, 1, rmotion},
      {manager, 2, rmotion},
      {manager, 3, rmotion},
      {manager, 4, rmotion},
      {manager, 5, rmotion}
    };

  //create controller input
  controllerInput ctrl(server);

  while(running)
  {
    //check for new controller values
    ctrl.updateValues();

    //lazy way to set drivemode
    if (ctrl.data.square)
      rmotion.drivemode = DRIVEMODE_NORMAL;
    else if (ctrl.data.triangle)
      rmotion.drivemode = DRIVEMODE_ROTATE;

    //some basic o2 simulation
    os.changeO2Level(ctrl.data.right_x / 10);

    //update wheel data
    switch (rmotion.drivemode)
    {
      case DRIVEMODE_NORMAL:
        // Handle the endpoints of the functions. If the stick is all the way to
        // the left or the right, we have to set the rover radius equal to the
        // minimum positive or minimum negative float value, respectively.
        if (abs(ctrl.data.left_x) == 1)
        {
          rmotion.radius = -FLT_MIN * ctrl.data.left_x;
        }
        else
        {
          rmotion.radius = RADIUS_MULTIPLIER * tan(HALF_PI*(ctrl.data.left_x + 1));
        }

        rmotion.maxradius = 0;
        for (int i=0; i<6; i++) wheels[i].updateMotionRadius();

        rmotion.velocity = ctrl.data.left_y * abs(rmotion.radius) / rmotion.maxradius;

        for (int i=0; i<6; i++) wheels[i].updateMotionVelocity();

        break;
      case DRIVEMODE_ROTATE:
        rmotion.radius = 0;
        rmotion.velocity = 0;
        rmotion.rotation = ctrl.data.left_x;

        rmotion.maxradius = 0;
        for (int i=0; i<6; i++)
        {
          wheels[i].updateMotionRadius();
          wheels[i].updateMotionVelocity();
        }
        break;
    }

    //debug info
    std::cout << "rover rad: " << rmotion.radius / PI * 180
              << " speed: " << rmotion.velocity * 255
              << " maxradius: " << rmotion.maxradius / PI * 180
              << std::endl;
    for (int i=0; i<6; i++)
    {
      std::cout << "wheel" << i << " angle: " << wheels[i].wmotion.angle / PI * 180
                                << " speed: " << wheels[i].wmotion.velocity * 255
                                << std::endl;
      //wheels[i].setLamp(ctrl.data.r1 * 0xFF);
    }

    std::cout << std::endl;

    //send device data to server
    manager.sendData();

    //sleep for 100 ms
    //usleep(100 * 1000);
  }

  std::cout << "stopping..." << std::endl;
  return 0;

}
