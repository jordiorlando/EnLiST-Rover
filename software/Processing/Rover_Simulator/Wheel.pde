// Defines
public static final boolean STATIONARY = false;
public static final boolean STEERABLE = true;
public static final boolean NORMAL = false;
public static final boolean REVERSE = true;

// Wheel class. Stores permanent data about each wheel, and is also responsible
// for calculating all the required numbers. It also takes care of drawing
// itself.
public class Wheel {
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
	// Flag to set whether or not a wheel radius should be drawn
	private boolean bDrawRadius = false;

	// Constructor. Takes an input to determine whether or not it is steerable
	// as well as its horizontal and vertical offsets from the center of the
	// rover.
	Wheel(boolean bSteerableTemp, boolean bReverseTemp, float fXTemp, float fYTemp) {
		bSteerable = bSteerableTemp;
		bReverse = bReverseTemp;
		fXPos = fXTemp;
		fYPos = fYTemp;

		// Calculate the angle of the wheel's home position. This corresponds to
		// 0 degrees on the servo.
		fCenterAngle = atan2(fYPos, fXPos);
		if (bReverse) {
			fCenterAngle += PI;  // If the wheel is reversed, reverse the angle
		}
	}

	// Calculates the horizontal and vertical distances from the center of
	// rotation of the wheel to the center of the wheel.
	private void center() {
		fCenterX = fRoverRadius + fXPos;
		fCenterY = fYPos;
	}

	// Calculates the angle for the wheel in radians using the global variables.
	float angle() {
		if (bDriveMode) {
			if (!bSteerable && bReverse) {
				fAngle = -HALF_PI;
			} else if (!bSteerable && !bReverse) {
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

			// If the wheel is reversed, reverse the angle
			if (bReverse) {
				fAngle += PI;
			}
		}

		// Bound the angle to +/- 2PI
		fAngle %= TWO_PI;

		return fAngle;
	}

	// Converts the wheel angle to a servo position between -90 and 90 degrees.
	private float servoPos() {
		float fServoPos = degrees(fCenterAngle - fAngle);
		if (fServoPos > 180) {
			fServoPos -= 360;
		} else if (fServoPos < -180) {
			fServoPos += 360;
		}
		// Bound anything that is too large or too small

		return fServoPos;
	}

	// Calculates the wheel radius using an arbitrary/hypothetical input value.
	float radius(float fRadiusIn) {
		return sqrt(sq(fRadiusIn + fXPos) + sq(fYPos));
	}

	// Calculates the wheel radius using the global variables.
	float radius() {
		fRadius = sqrt(sq(fRoverRadius + fXPos) + sq(fYPos));

		return fRadius;
	}

	// Calculates the wheel velocity using the global variables.
	float velocity() {
		if (bDriveMode) {
			fVelocity = nMaxSpeed * fRoverRotation * fRadius / maxRadius(fRoverRadius);
		} else {
			if (bReverse) {
				fVelocity = -fRadius * fRoverVelocity / abs(fRoverRadius);
			} else {
				fVelocity = fRadius * fRoverVelocity / abs(fRoverRadius);
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

		println(servoPos());
	}

	// Draws a vector that represents the velocity of the wheel.
	void drawVelocity() {
		// Don't draw anything if the wheel isn't turning
		if (drawRoverVelocity.pressed() && (fVelocity != 0)) {
			stroke(255, 255, 255);
			line(0, 0, fVelocity / 8, 0);
			line(fVelocity / 8, -2, fVelocity / 8, 2);
		}
	}

	// Draws the actual wheel itself.
	void drawWheel() {
		fill(48, 48, 48);
		if (over()) {
			stroke(0, 0, 0);
		} else {
			noStroke();
		}
		rectMode(RADIUS);
		rect(0, 0, fWheelHeight, fWheelWidth, fWheelWidth / 2);

		// Uncomment to show an indicator on the front of the wheel
		/*stroke(0, 0, 0);
		line(fWheelHeight + 5, -fWheelWidth + 5, fWheelHeight + 5, fWheelWidth - 5);*/
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
		if (bDrawRadius) {
			point(0, 0);
		}
		drawVelocity();

		popMatrix();
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
}
