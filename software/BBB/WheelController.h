#ifndef WHEELCONTROLLER_H_
#define WHEELCONTROLLER_H_

#include "deviceManager.h"
#include "i2cDevice.h"

//for math functions
#include <cmath>
//for float macros
#include <cfloat>

/*
 * Custom uC motor controller.
 * Address 0x40 + wheel#
 */

#define WHEEL_BASE_ADDRESS 0x42

#define STATIONARY  false
#define STEERABLE   true
#define DIR_NORMAL  false
#define DIR_REVERSE true

#define DRIVEMODE_NORMAL 0
#define DRIVEMODE_ROTATE 1

#define ROVER_WIDTH  250
#define ROVER_HEIGHT 500

#define WHEEL_MAX_SPEED 0xFF
#define RADIUS_MULTIPLIER 500

#define WHEEL_DIRECTION_FORWARD  0x0
#define WHEEL_DIRECTION_BACKWARD 0x1

#define WHEEL_REG_SPEEDCTRL  0x0
#define WHEEL_REG_REQ_SPEED  0x1
#define WHEEL_REG_STATUS     0x2
#define WHEEL_REG_CURR_SPEED 0x3
#define WEEEL_REG_SERVO_ANG  0x4
#define WEEEL_REG_BRIGHTNESS 0x5

#define PI 3.141592654
#define HALF_PI (PI/2)
#define TWO_PI (PI*2)

#define signum(x) ((x) < 0) ? -1 : ((x) > 0)

class WheelController : public i2cDevice
{

  public:
    struct rovermotion
    {
      float radius;
      float velocity;
      float rotation;
      float maxradius;
      int   drivemode;
    };

    WheelController(deviceManager & manager, int wheel, rovermotion & rm);

    void setLamp(uint8_t brightness);

    void updateMotionRadius();
    void updateMotionVelocity();

    //wheel motion data
    //NOTE: this should be private but is public for debugging
    struct wheelmotion
    {
      float angle;
      float radius;
      float velocity;

      //center of circle wheel travels around
      //relative to the wheel
      float centerx;
      float centery;
    };

    wheelmotion   wmotion;

  private:

    struct devicedata
    {
      uint8_t direction;
      uint8_t speed;

      uint8_t lampbrightness;

      uint8_t servoangle;
    };

    //wheel attributes (static)
    struct wheelattr
    {
      int wheelnum;

      bool steerable;
      bool reverse;

      float xpos;
      float ypos;
      float centerangle;

      float width;
      float height;
    };



    devicedata    data;

    wheelattr     wattr;

    rovermotion * rmotion;

    void _update();
    int  _out(uint8_t * buf);

    void compute_wheelVelocity();
    void compute_servoAngle();

    void compute_center();
    void compute_angle();
    void compute_radius();
    void compute_velocity();

};

#endif //WHEELCONTROLLER_H_
