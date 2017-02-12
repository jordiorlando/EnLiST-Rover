// Defines
public static final boolean STATIONARY  = false;
public static final boolean STEERABLE   = true;
public static final boolean NORMAL      = false;
public static final boolean REVERSE     = true;

public class Rover {
  // Wheel array
  Wheel[] wheels = new Wheel[6];
  Mast mast;
  private float bodyWidth, bodyHeight;

  Rover(float w, float h) {
    bodyWidth = w / 2;
    bodyHeight = h / 2;

    // Wheel declarations
    wheels[0] = new Wheel(STEERABLE,  REVERSE, -bodyWidth * 1.5, bodyHeight);
    wheels[1] = new Wheel(STEERABLE,  NORMAL,   bodyWidth * 1.5, bodyHeight);
    wheels[2] = new Wheel(STATIONARY, REVERSE, -bodyWidth * 1.5, 0);
    wheels[3] = new Wheel(STATIONARY, NORMAL,   bodyWidth * 1.5, 0);
    wheels[4] = new Wheel(STEERABLE,  REVERSE, -bodyWidth * 1.5, -bodyHeight);
    wheels[5] = new Wheel(STEERABLE,  NORMAL,   bodyWidth * 1.5, -bodyHeight);

    mast = new Mast(0, bodyHeight * 0.875);
  }

  void draw() {
    // Center the matrix (from now on, the origin is at (640, 360))
    pushMatrix();
    translate(width / 2, height / 2);

    for (Rover.Wheel wheel : wheels) {
      // Update all wheels
      wheel.update();

      if (wheel.isPressed) {
        wheel.drawRadius();
      }
    }

    if (driveMode) {
      drawBody();
      mast.draw();
      if (drawRoverRotation.pressed()) {
        drawRotation();
      }
    } else {
      if (drawRoverRadius.pressed()) {
        drawRadius();
      }
      drawBody();
      mast.draw();
      if (drawRoverVelocity.pressed()) {
        drawVelocity();
      }
    }

    // Draw all wheels
    for (Rover.Wheel wheel : wheels) {
      wheel.draw();
    }

    popMatrix();
  }

  boolean over() {
    if ((mouseX > width / 2 - bodyWidth) && (mouseX < width / 2 + bodyWidth) &&
        (mouseY > height / 2 - bodyHeight) && (mouseY < height / 2 + bodyHeight)) {
      return true;
    } else {
      return false;
    }
  }

  // Draws a circle representing the path that the center of the rover will take
  private void drawRadius() {
    noFill();
    stroke(0, 0, 0, 200);
    ellipseMode(RADIUS);
    point(0, 0);

    if (X1Stick == 0) {
      // If we're going straight, just draw a line
      line(0, -height / 2, 0, height / 2);
    } else {
      // If not, draw a circle, and a point at the center of that circle
      point(-roverRadius, 0);
      ellipse(-roverRadius, 0, abs(roverRadius), abs(roverRadius));
    }
  }

  // Draws a vector representing the velocity of the center of the rover
  private void drawVelocity() {
    // Don't draw anything if the rover isn't moving
    if (roverVelocity != 0) {
      stroke(0, 0, 0, 255);
      line(0, 0, 0, -roverVelocity * bodyHeight * 0.75);
      line(-10, -roverVelocity * bodyHeight * 0.75, 10, -roverVelocity * bodyHeight * 0.75);
    }
  }

  // Draws a curved vector representing the rover's rotation about its center
  private void drawRotation() {
    noFill();
    stroke(0, 0, 0, 255);
    ellipseMode(RADIUS);

    point(0, 0);

    // Only necessary because Processing does not allow negative angles in arcs
    if (roverRotation < 0) {
      arc(0, 0, 50, 50, 0, -roverRotation * TWO_PI);
    } else {
      arc(0, 0, 50, 50, TWO_PI - roverRotation * TWO_PI, TWO_PI);
    }

    // Only draw the tip of the arrow if the rover is actually turning
    if (roverRotation != 0) {
      pushMatrix();
      rotate(-roverRotation * TWO_PI);
      line(45, 0, 55, 0);
      popMatrix();
    }
  }

  // Draws the body of the rover as a stationary, semi-translucent rectangle
  private void drawBody() {
    fill(255, 255, 255, 127);
    if (over()) {
      stroke(0, 0, 0, 255);
    } else {
      stroke(0, 0, 0, 127);
    }

    rectMode(RADIUS);
    rect(0, 0, bodyWidth, bodyHeight, 5);
  }

  // Wheel class. Stores data about each wheel, and is also responsible for
  // calculating all required numbers. It also takes care of drawing itself.
  class Wheel {
    // Determines whether this wheel is capable of rotating or not and whether
    // or not it is mounted backwards.
    private boolean isSteerable, isReverse;
    // The horizontal and vertical offsets from the center of the rover and the
    // angle the wheel is mounted at.
    private float xPos, yPos, centerAngle;
    // The center of the circle around which the wheel will travel, relative to
    // the wheel.
    private float centerX, centerY;
    // The three variables that define the motion of the wheel
    private float _angle, _radius, _velocity;
    // Flag to set whether or not the wheel has been pressed
    private boolean isPressed = true;

    private float _width, _height;

    // Constructor. Takes inputs to determine whether or not it is steerable as
    // well as its horizontal and vertical offsets from the center of the rover.
    Wheel(boolean s, boolean r, float x, float y) {
      isSteerable = s;
      isReverse = r;
      xPos = x;
      yPos = y;
      _width = bodyWidth / 5;
      _height = bodyHeight / 5;

      // Calculate the angle of the wheel's home position. This corresponds to
      // 0 degrees on the servo.
      centerAngle = atan2(yPos, xPos);
    }

    // Updates all the wheel variables
    void update() {
      center();
      angle();
      radius();
      velocity();
    }

    // Calls all the required draw functions
    void draw() {
      pushMatrix();
      translate(xPos, -yPos);
      rotate(-_angle);

      drawWheel();
      stroke(0, 0, 0);
      if (isPressed) {
        point(0, 0);
        drawVelocity();
      }

      popMatrix();

      if (isPressed) {
        fill(48, 48, 48);
        textAlign(CENTER, CENTER);

        text(String.format("%.0f", degrees(servoAngle())) + (char)0x00B0, xPos, -(yPos + _height + 20));
        text(String.format("%.0f", wheelVelocity()), xPos, -yPos + _height + 16);

        textAlign(LEFT, CENTER);
      }
    }

    void set(boolean bState) {
      isPressed = bState;
    }

    void toggle() {
      isPressed = !isPressed;
    }


    // Returns a boolean telling whether or not the mouse is over the wheel
    boolean over() {
      radius();  // Make sure the angle calculation is up-to-date

      // Do some fancy-ass math
      float xDistance  = float(mouseX) - (width / 2) - xPos;
      float yDistance  = float(mouseY) - (height / 2) + yPos;
      float hypotenuse = sqrt(sq(xDistance) + sq(yDistance));
      float theta      = _angle + atan2(yDistance, xDistance);

      if ((abs(hypotenuse * cos(theta)) < _height) &&
          (abs(hypotenuse * sin(theta)) < _width)) {
        return true;
      } else {
        return false;
      }
    }

    float wheelVelocity() {
      float wv = _velocity * maxSpeedValue;
      if (wv == -0) {
        wv = 0;
      }

      return wv;
    }

    // Converts the wheel angle to a servo position between -90 and 90 degrees
    float servoAngle() {
      float servoPosition = 0;
      if (isSteerable) {
        servoPosition = _angle - centerAngle;

        // Bound anything that is too large or too small
        if ((xPos * yPos) < 0) {
          servoPosition -= PI;
          if (servoPosition < -PI) {
            servoPosition += TWO_PI;
          }
        } else if (servoPosition > PI){
          servoPosition -= TWO_PI;
        }
      }

      servoPosition += HALF_PI;
      servoPosition = abs(servoPosition);

      return servoPosition;
    }

    // Calculates the horizontal and vertical distances from the center of
    // rotation of the wheel to the center of the wheel.
    private void center() {
      centerX = roverRadius + xPos;
      centerY = yPos;
    }

    // Calculates the angle for the wheel in radians using the global variables
    private float angle() {
      if (driveMode) {
        if (!isSteerable) {
          _angle = HALF_PI;
        } else if (xPos > 0) {
          _angle = centerAngle + HALF_PI;
        } else {
          _angle = centerAngle - HALF_PI;
        }
      } else {
        if (!isSteerable) {
          _angle = HALF_PI;
        } else if (roverRadius > 0) {
          _angle = atan2(centerY, centerX) + HALF_PI;
        } else {
          _angle = atan2(centerY, centerX) - HALF_PI;
        }
      }

      // If the wheel is reversed, reverse the angle
      if (isReverse) {
        _angle += PI;
      }

      // Bound the angle to +/- 2PI
      _angle %= TWO_PI; // TODO: fix this

      return _angle;
    }

    // Calculates the wheel radius using an arbitrary/hypothetical input value
    private float radius(float r) {
      return sqrt(sq(r + xPos) + sq(yPos));
    }

    // Calculates the wheel radius using the global variables
    private float radius() {
      _radius = sqrt(sq(roverRadius + xPos) + sq(yPos));

      return _radius;
    }

    // Calculates the wheel velocity using the global variables
    private float velocity() {
      if (driveMode) {
        _velocity = roverRotation * _radius / maxRadius(roverRadius);
      } else {
        _velocity = _radius * roverVelocity / abs(roverRadius);

        if (isReverse) {
          _velocity *= -1;
        }

        // Check for situations where the wheel should turn in the opposite
        // direction than expected. This occurs when the wheel is
        // non-steerable or has a y-position of 0 and its wheel radius is
        // greater than the global rover radius.
        //
        // TODO: make this work with all wheels, not just non-steerable ones
        if ((!isSteerable || (yPos == 0)) &&
            (Math.signum(roverRadius) * (roverRadius + xPos) < 0)) {
          _velocity *= -1;
        }
      }

      return _velocity;
    }

    // Draws a circle representing the actual path this wheel will take
    void drawRadius() {
      noFill();
      stroke(127, 127, 127, 127);
      ellipseMode(RADIUS);

      if (!driveMode && X1Stick == 0) {
        // If we're going straight, just draw a line
        line(xPos, -height / 2, xPos, height / 2);
      } else {
        // If not, draw a circle, and a point at the center of that circle
        point(-roverRadius, 0);
        ellipse(-roverRadius, 0, _radius, _radius);
      }
    }

    // Draws a vector that represents the velocity of the wheel
    private void drawVelocity() {
      // Don't draw anything if the wheel isn't turning
      if (_velocity != 0) {
        stroke(255, 255, 255);
        line(0, 0, _velocity * _height * 0.75, 0);
        line(_velocity * _height * 0.75, -2, _velocity * _height * 0.75, 2);
      }
    }

    // Draws the actual wheel itself
    private void drawWheel() {
      fill(48, 48, 48, 239);
      if (over()) {
        stroke(0, 0, 0, 255);
      } else {
        stroke(32, 32, 32, 239);
      }
      rectMode(RADIUS);
      rect(0, 0, _height, _width, _width / 2);

      // Show an indicator on the outside of the wheel
      if (drawWheelIndicators.pressed()) {
        stroke(16, 16, 16, 239);
        line(_height - 10, _width + 5, -_height + 10, _width + 5);
      }
    }
  }

  // Draws the camera mast on the front of the rover
  class Mast {
    // The horizontal and vertical offsets from the center of the rover
    private float xPos, yPos;

    Mast(float x, float y) {
      xPos = x;
      yPos = y;
    }

    void draw() {
      pushMatrix();
      translate(xPos, -yPos);

      if (mastMode) {
        float increment = X2Stick * PI / 100;
        if (abs(mastPanAngle + increment) <= PI) {
          mastPanAngle += increment;
        }
      } else {
        mastPanAngle = X2Stick * PI;
      }
      rotate(mastPanAngle);

      fill(48, 48, 48, 255);
      noStroke();
      rectMode(RADIUS);
      rect(0, 0, bodyWidth / 2, bodyWidth / 8, bodyWidth / 16);
      rect(0, bodyWidth / 8, bodyWidth / 4, bodyWidth / 16, bodyWidth / 16);

      popMatrix();
    }
  }
}
