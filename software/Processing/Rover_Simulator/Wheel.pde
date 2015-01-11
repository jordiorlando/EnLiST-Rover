// Wheel class. Stores permanent data about each wheel, and is also responsible
// for calculating all the required numbers. It also takes care of drawing
// itself.
public class Wheel {
	// Determines whether this wheel is capable of rotating or not
	boolean bSteerable;
	// The horizontal and vertical offsets from the center of the wheel
	float fXPos, fYPos;
	// The three variables that define the motion of the wheel
	float fAngle, fRadius, fVelocity;

	boolean bDrawRadius = false;

	// Constructor. Takes an input to determine whether or not it is steerable
	// as well as its horizontal and vertical offsets from the center of the
	// rover.
	Wheel(boolean bSteerableTemp, float fXTemp, float fYTemp) {
		bSteerable = bSteerableTemp;
		fXPos = fXTemp;
		fYPos = fYTemp;
	}

	// Calculates the angle for the wheel in radians using the global variables.
	float angle() {
		if (bDriveMode) {
			// Angle is always the same when in mode 1
			if (!bSteerable || (fYPos == 0)) {
				fAngle = 0;  // Non-steerable wheels always face forward
			} else if (fXPos * fYPos > 0) {
				fAngle = -atan(fXPos/fYPos) + HALF_PI;
			} else {
				fAngle = -atan(fXPos/fYPos) - HALF_PI;
			}
		} else {
			// In mode 0, angle depends on the global rover radius
			if (!bSteerable || (fYPos == 0)) {
				fAngle = 0;  // Non-steerable wheels always face forward
			} else if (fRoverRadius * fYPos > 0) {
				fAngle = -atan((fRoverRadius + fXPos)/fYPos) + HALF_PI;
			} else {
				fAngle = -atan((fRoverRadius + fXPos)/fYPos) - HALF_PI;
			}
		}

		return fAngle;
	}

	// Calculates the wheel radius using an arbitrary/hypothetical input value.
	float radius(float fRadiusIn) {
		return sqrt(sq(fRadiusIn + fXPos) + sq(fYPos));
	}

	// Calculates the wheel radius using the global variables.
	float radius() {
		fRadius = sqrt(sq(fRoverRadius + fXPos) + sq(fYPos)) * fXPos / abs(fXPos);

		return fRadius;
	}

	// Calculates the wheel velocity using the global variables.
	float velocity() {
		radius();  // Make sure the radius calculation is up-to-date

		if (bDriveMode) {
			fVelocity = nMaxSpeed * fRoverRotation * fRadius / maxRadius(fRoverRadius);
		} else {
			fVelocity = abs(fRadius) * fRoverVelocity / abs(fRoverRadius);

			// Check for situations where the wheel should turn in the opposite
			// direction than expected. This occurs when the wheel is
			// non-steerable and its wheel radius is greater than the global
			// rover radius.
			//
			// TODO: make this work with all wheels, not just the non-steerable
			// ones.
			if ((!bSteerable || (fYPos == 0)) && (((fRoverRadius < 0) && (fRoverRadius + fXPos) > 0) || ((fRoverRadius > 0) && (fRoverRadius + fXPos) < 0))) {
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
	void drawVelocity() {
		// Don't draw anything if the wheel isn't turning
		if (drawRoverVelocity.pressed() && (fVelocity != 0)) {
			stroke(255, 255, 255);
			line(0, 0, 0, -fVelocity / 8);
			line(-2, -fVelocity / 8, 2, -fVelocity / 8);
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
		rect(0, 0, fWheelWidth, fWheelHeight, fWheelWidth / 2);

		// Uncomment to show an indicator on the front of the wheel
		/*stroke(0, 0, 0);
		line(-15, -45, 15, -45);*/
	}

	// Updates all the wheel variables.
	void update() {
		angle();
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
		float fTheta = fAngle + atan(fYDistance / fXDistance);

		if ((abs(fHypotenuse * cos(fTheta)) < fWheelWidth) && (abs(fHypotenuse * sin(fTheta)) < fWheelHeight)) {
			return true;
		} else {
			return false;
		}
	}
}
