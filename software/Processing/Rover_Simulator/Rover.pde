// Defines
public static final boolean STATIONARY = false;
public static final boolean STEERABLE = true;
public static final boolean NORMAL = false;
public static final boolean REVERSE = true;

public class Rover {
  // Wheel array
  Wheel[] wheels = new Wheel[6];
  Mast mast;
  private float fWidth, fHeight;

  Rover(float fWidthTemp, float fHeightTemp) {
    fWidth = fWidthTemp / 2;
    fHeight = fHeightTemp / 2;

    // Wheel declarations
    wheels[0] = new Wheel(STEERABLE, REVERSE, -(fWidth * 1.5), fHeight);
    wheels[1] = new Wheel(STEERABLE, NORMAL, fWidth * 1.5, fHeight);
    wheels[2] = new Wheel(STATIONARY, REVERSE, -(fWidth * 1.5), 0);
    wheels[3] = new Wheel(STATIONARY, NORMAL, fWidth * 1.5, 0);
    wheels[4] = new Wheel(STEERABLE, REVERSE, -(fWidth * 1.5), -fHeight);
    wheels[5] = new Wheel(STEERABLE, NORMAL, fWidth * 1.5, -fHeight);

    mast = new Mast(0, fHeight * 0.875);
  }

  void draw() {
    // Center the matrix (from now on, the origin is at (640, 360))
    pushMatrix();
    translate(width / 2, height / 2);

    for (Rover.Wheel wheel : wheels) {
      // Update all wheels
      wheel.update();

      if (wheel.bPressed) {
        wheel.drawRadius();
      }
    }

    if (bDriveMode) {
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
    if ((mouseX > width / 2 - fWidth) && (mouseX < width / 2 + fWidth) && (mouseY > height / 2 - fHeight) && (mouseY < height / 2 + fHeight)) {
      return true;
    } else {
      return false;
    }
  }

  // Draws a circle that represents the path that the center of the rover will
  // take.
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
      point(-fRoverRadius, 0);
      ellipse(-fRoverRadius, 0, abs(fRoverRadius), abs(fRoverRadius));
    }
  }

  // Draws a vector that represents the translational velocity of the center of
  // the rover.
  private void drawVelocity() {
    // Don't draw anything if the rover isn't moving
    if (fRoverVelocity != 0) {
      stroke(0, 0, 0, 255);
      line(0, 0, 0, -fRoverVelocity * fHeight * 0.75);
      line(-10, -fRoverVelocity * fHeight * 0.75, 10, -fRoverVelocity * fHeight * 0.75);
    }
  }

  // Draws a nice curvy vector-thing that represents the rover's rotation about
  // its center.
  private void drawRotation() {
    noFill();
    stroke(0, 0, 0, 255);
    ellipseMode(RADIUS);

    point(0, 0);

    // Only necessary because Processing does not allow negative angles in arcs
    if (fRoverRotation < 0) {
      arc(0, 0, 50, 50, 0, -fRoverRotation * TWO_PI);
    } else {
      arc(0, 0, 50, 50, TWO_PI - fRoverRotation * TWO_PI, TWO_PI);
    }

    // Only draw the tip of the arrow if the rover is actually turning
    if (fRoverRotation != 0) {
      pushMatrix();
      rotate(-fRoverRotation * TWO_PI);
      line(45, 0, 55, 0);
      popMatrix();
    }
  }

  // Draws the body of the rover. Just a stationary, semi-translucent rectangle.
  private void drawBody() {
    fill(255, 255, 255, 127);
    if (over()) {
      stroke(0, 0, 0, 255);
    } else {
      stroke(0, 0, 0, 127);
    }

    rectMode(RADIUS);
    rect(0, 0, fWidth, fHeight, 5);
  }

  // Wheel class. Stores permanent data about each wheel, and is also responsible
  // for calculating all the required numbers. It also takes care of drawing
  // itself.
  class Wheel {
    // Determines whether this wheel is capable of rotating or not and whether
    // or not it is mounted backwards.
    private boolean bSteerable, bReverse;
    // The horizontal and vertical offsets from the center of the rover and the
    // angle the wheel is mounted at.
    private float fXPos, fYPos, fCenterAngle;
    // The center of the circle around which the wheel will travel, relative to
    // the wheel.
    private float fCenterX, fCenterY;
    // The three variables that define the motion of the wheel
    private float fAngle, fRadius, fVelocity;
    // Flag to set whether or not the wheel has been pressed
    private boolean bPressed = true;

    private float fWheelWidth, fWheelHeight;

    // Constructor. Takes an input to determine whether or not it is steerable
    // as well as its horizontal and vertical offsets from the center of the
    // rover.
    Wheel(boolean bSteerableTemp, boolean bReverseTemp, float fXTemp, float fYTemp) {
      bSteerable = bSteerableTemp;
      bReverse = bReverseTemp;
      fXPos = fXTemp;
      fYPos = fYTemp;
      fWheelWidth = fWidth / 5;
      fWheelHeight = fHeight / 5;

      // Calculate the angle of the wheel's home position. This corresponds to
      // 0 degrees on the servo.
      fCenterAngle = atan2(fYPos, fXPos);
    }

    // Updates all the wheel variables.
    void update() {
      center();
      angle();
      radius();
      velocity();
    }

    // Calls all the required draw functions.
    void draw() {
      pushMatrix();
      translate(fXPos, -fYPos);
      rotate(-fAngle);

      drawWheel();
      stroke(0, 0, 0);
      if (bPressed) {
        point(0, 0);
        drawVelocity();
      }

      popMatrix();

      if (bPressed) {
        fill(48, 48, 48);
        textAlign(CENTER, CENTER);

        text(String.format("%.0f", degrees(servoAngle())) + (char)0x00B0, fXPos, -(fYPos + fWheelHeight + 20));
        text(String.format("%.0f", wheelVelocity()), fXPos, -fYPos + fWheelHeight + 16);

        textAlign(LEFT, CENTER);
      }
    }

    void set(boolean bState) {
      bPressed = bState;
    }

    void toggle() {
      bPressed = !bPressed;
    }


    // Returns a boolean telling whether or not the mouse is over the wheel.
    boolean over() {
      radius();  // Make sure the angle calculation is up-to-date

      // Do some fancy-ass math
      float fXDistance = float(mouseX) - (width / 2) - fXPos;
      float fYDistance = float(mouseY) - (height / 2) + fYPos;
      float fHypotenuse = sqrt(sq(fXDistance) + sq(fYDistance));
      float fTheta = fAngle + atan2(fYDistance, fXDistance);

      if ((abs(fHypotenuse * cos(fTheta)) < fWheelHeight) && (abs(fHypotenuse * sin(fTheta)) < fWheelWidth)) {
        return true;
      } else {
        return false;
      }
    }

    float wheelVelocity() {
      float fWheelVelocity = fVelocity * nMaxSpeed;
      if (fWheelVelocity == -0) {
        fWheelVelocity = 0;
      }

      return fWheelVelocity;
    }

    // Converts the wheel angle to a servo position between -90 and 90 degrees.
    float servoAngle() {
      float fServoPosition = 0;
      if (bSteerable) {
        fServoPosition = fAngle - fCenterAngle;

        // Bound anything that is too large or too small
        if ((fXPos * fYPos) < 0) {
          fServoPosition -= PI;
          if (fServoPosition < -PI) {
            fServoPosition += TWO_PI;
          }
        } else if (fServoPosition > PI){
          fServoPosition -= TWO_PI;
        }
      }

      fServoPosition += HALF_PI;
      fServoPosition = abs(fServoPosition);

      return fServoPosition;
    }

    // Calculates the horizontal and vertical distances from the center of
    // rotation of the wheel to the center of the wheel.
    private void center() {
      fCenterX = fRoverRadius + fXPos;
      fCenterY = fYPos;
    }

    // Calculates the angle for the wheel in radians using the global variables.
    private float angle() {
      if (bDriveMode) {
        if (!bSteerable) {
          fAngle = HALF_PI;
        } else if (fXPos > 0) {
          fAngle = fCenterAngle + HALF_PI;
        } else {
          fAngle = fCenterAngle - HALF_PI;
        }
      } else {
        if (!bSteerable) {
          fAngle = HALF_PI;
        } else if (fRoverRadius > 0) {
          fAngle = atan2(fCenterY, fCenterX) + HALF_PI;
        } else {
          fAngle = atan2(fCenterY, fCenterX) - HALF_PI;
        }
      }

      // If the wheel is reversed, reverse the angle
      if (bReverse) {
        fAngle += PI;
      }

      // Bound the angle to +/- 2PI
      fAngle %= TWO_PI; // TODO: fix this

      return fAngle;
    }

    // Calculates the wheel radius using an arbitrary/hypothetical input value.
    private float radius(float fRadiusIn) {
      return sqrt(sq(fRadiusIn + fXPos) + sq(fYPos));
    }

    // Calculates the wheel radius using the global variables.
    private float radius() {
      fRadius = sqrt(sq(fRoverRadius + fXPos) + sq(fYPos));

      return fRadius;
    }

    // Calculates the wheel velocity using the global variables.
    private float velocity() {
      if (bDriveMode) {
        fVelocity = fRoverRotation * fRadius / maxRadius(fRoverRadius);
      } else {
        fVelocity = fRadius * fRoverVelocity / abs(fRoverRadius);

        if (bReverse) {
          fVelocity *= -1;
        }

        // Check for situations where the wheel should turn in the opposite
        // direction than expected. This occurs when the wheel is
        // non-steerable or has a y-position of 0 and its wheel radius is
        // greater than the global rover radius.
        //
        // TODO: make this work with all wheels, not just the non-steerable
        // ones.
        if ((!bSteerable || (fYPos == 0)) && (Math.signum(fRoverRadius) * (fRoverRadius + fXPos) < 0)) {
          fVelocity *= -1;
        }
      }

      return fVelocity;
    }

    // Draws a circle representing the actual path this wheel will take.
    void drawRadius() {
      noFill();
      stroke(127, 127, 127, 127);
      ellipseMode(RADIUS);

      if (!bDriveMode && X1Stick == 0) {
        // If we're going straight, just draw a line
        line(fXPos, -height / 2, fXPos, height / 2);
      } else {
        // If not, draw a circle, and a point at the center of that circle
        point(-fRoverRadius, 0);
        ellipse(-fRoverRadius, 0, fRadius, fRadius);
      }
    }

    // Draws a vector that represents the velocity of the wheel.
    private void drawVelocity() {
      // Don't draw anything if the wheel isn't turning
      if (fVelocity != 0) {
        stroke(255, 255, 255);
        line(0, 0, fVelocity * fWheelHeight * 0.75, 0);
        line(fVelocity * fWheelHeight * 0.75, -2, fVelocity * fWheelHeight * 0.75, 2);
      }
    }

    // Draws the actual wheel itself.
    private void drawWheel() {
      fill(48, 48, 48, 239);
      if (over()) {
        stroke(0, 0, 0, 255);
      } else {
        stroke(32, 32, 32, 239);
      }
      rectMode(RADIUS);
      rect(0, 0, fWheelHeight, fWheelWidth, fWheelWidth / 2);

      // Show an indicator on the outside of the wheel
      if (drawWheelIndicators.pressed()) {
        stroke(16, 16, 16, 239);
        line(fWheelHeight - 10, fWheelWidth + 5, -fWheelHeight + 10, fWheelWidth + 5);
      }
    }
  }

  // Draws the camera mast on the front of the rover
  class Mast {
    // The horizontal and vertical offsets from the center of the rover
    private float fXPos, fYPos;

    Mast(float fXTemp, float fYTemp) {
      fXPos = fXTemp;
      fYPos = fYTemp;
    }

    void draw() {
      pushMatrix();
      translate(fXPos, -fYPos);

      if (bMastMode) {
        float fIncrement = X2Stick * PI / 100;
        if (abs(fMastPan + fIncrement) <= PI) {
          fMastPan += fIncrement;
        }
      } else {
        fMastPan = X2Stick * PI;
      }
      rotate(fMastPan);

      fill(48, 48, 48, 255);
      noStroke();
      rectMode(RADIUS);
      rect(0, 0, fWidth / 2, fWidth / 8, fWidth / 16);
      rect(0, fWidth / 8, fWidth / 4, fWidth / 16, fWidth / 16);

      popMatrix();
    }
  }
}
