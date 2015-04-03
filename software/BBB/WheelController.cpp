#include "WheelController.h"

WheelController::WheelController(deviceManager & manager, int wheel, rovermotion & rm)
 : i2cDevice(WHEEL_BASE_ADDRESS + wheel, manager, 100), rmotion(&rm)
{

  /*
   * Assumption for wheel scheme (6 wheels):
   *
   *     0     x/y
   *
   * 0 ----- 1   ROVER_HEIGHT
   * |   |   |
   * 2 ----- 3   0
   * |   |   |
   * 5 ----- 6   -ROVER_HEIGHT
   *
   * |-------|
   * 3*ROVER_WIDTH
   *
   */

  wattr.wheelnum = wheel;

  if (wattr.wheelnum > 1 && wattr.wheelnum < 4)
    wattr.steerable = STATIONARY;
  else
    wattr.steerable = STEERABLE;

  //even wheels are reversed
  wattr.reverse = (wattr.wheelnum + 1) & 1;

  //wheels alternate sides
  wattr.xpos = (1.5*ROVER_WIDTH) * (-1 + (wattr.wheelnum & 1) * 2);

  //each pair of wheels advance by ROVER_HEIGHT in -y
  wattr.ypos = (wattr.wheelnum / 2) * -ROVER_HEIGHT + ROVER_HEIGHT;

  wattr.centerangle = atan2(wattr.xpos, wattr.ypos);

  wattr.width  = ROVER_WIDTH / 5;
  wattr.height = ROVER_HEIGHT / 5;

}

/*
 * Set the LED lamp brightness from 0 to 255.
 */
void WheelController::setLamp(uint8_t brightness)
{
  data.lampbrightness = brightness;
}

void WheelController::_update()
{
  connected = true;

  smbWriteByte(WHEEL_REG_SPEEDCTRL, data.direction);
  smbWriteByte(WHEEL_REG_REQ_SPEED, data.speed);
  smbWriteByte(WEEEL_REG_SERVO_ANG, data.servoangle);
  smbWriteByte(WEEEL_REG_BRIGHTNESS, data.lampbrightness);
}

int WheelController::_out(uint8_t * buf)
{
  return -1;
}

/*
 * Called when the rover's radius is updated.
 * Depends on the radius of the rover, updates the rover maxradius.
 */
void WheelController::updateMotionRadius()
{
  compute_center();
  compute_angle();
  compute_radius();

  if (rmotion->maxradius < wmotion.radius)
    rmotion->maxradius = wmotion.radius;

  compute_servoAngle();
}

/*
 * Called when the rover's velocity is updated.
 * Depends on the radius and the velocity (assumes radius is updated).
 */
void WheelController::updateMotionVelocity()
{
  compute_velocity();

  compute_wheelVelocity();
}


// Calculates the horizontal and vertical distances from the center of
// rotation of the wheel to the center of the wheel.
void WheelController::compute_center()
{
  wmotion.centerx = rmotion->radius + wattr.xpos;
  wmotion.centery = wattr.ypos;
}

void WheelController::compute_angle()
{
  switch (rmotion->drivemode)
  {
    case DRIVEMODE_NORMAL:
      if (!wattr.steerable)
      {
        wmotion.angle = HALF_PI;
      }
      else if (rmotion->radius > 0)
      {
        wmotion.angle = atan2(wmotion.centery, wmotion.centerx) + HALF_PI;
      }
      else
      {
        wmotion.angle = atan2(wmotion.centery, wmotion.centerx) - HALF_PI;
      }

      break;
    case DRIVEMODE_ROTATE:
      if (!wattr.steerable)
      {
        wmotion.angle = HALF_PI;
      }
      else if (wattr.xpos > 0)
      {
        wmotion.angle = wattr.centerangle + HALF_PI;
      }
      else
      {
        wmotion.angle = wattr.centerangle - HALF_PI;
      }

      break;
  }

  if (wattr.reverse) wmotion.angle += PI;

  // Bound the angle to +/- 2PI
  wmotion.angle = fmod(wmotion.angle, TWO_PI);

}

void WheelController::compute_radius()
{
  wmotion.radius = sqrt(pow(rmotion->radius + wattr.xpos, 2) +
                        pow(wattr.ypos, 2));
}

void WheelController::compute_velocity()
{
  switch (rmotion->drivemode)
  {
    case DRIVEMODE_NORMAL:
      wmotion.velocity = wmotion.radius * rmotion->velocity / abs(rmotion->radius);

      if (wattr.reverse) wmotion.velocity *= -1;

      // Check for situations where the wheel should turn in the opposite
      // direction than expected. This occurs when the wheel is
      // non-steerable or has a y-position of 0 and its wheel radius is
      // greater than the global rover radius.
      //
      // TODO: make this work with all wheels, not just the non-steerable
      // ones.
      if ( (!wattr.steerable || wattr.ypos == 0) &&
           (signum(rmotion->radius) * (rmotion->radius + wattr.xpos) < 0) )
      {
          wmotion.velocity *= -1;
      }

      break;
    case DRIVEMODE_ROTATE:
      wmotion.velocity = rmotion->rotation * wmotion.radius / rmotion->maxradius;
      break;
  }
}


void WheelController::compute_wheelVelocity()
{
    data.speed     = abs(wmotion.velocity) * WHEEL_MAX_SPEED;
    data.direction = (wmotion.velocity < 0) ? WHEEL_DIRECTION_BACKWARD :
                                              WHEEL_DIRECTION_FORWARD;
}

// Converts the wheel angle to a servo position between -90 and 90 degrees.
void WheelController::compute_servoAngle()
{
  float servoangle = 0;

  if (wattr.steerable) {
    servoangle = wmotion.angle - wattr.centerangle;

    // Bound anything that is too large or too small
    if ((wattr.xpos * wattr.ypos) < 0)
    {
      servoangle -= PI;
      if (servoangle < -PI) servoangle += TWO_PI;
    }
    else if (servoangle > PI)
    {
      servoangle -= TWO_PI;
    }
  }

  servoangle += HALF_PI;
  servoangle = abs(servoangle);

  data.servoangle = servoangle + 90;
}
