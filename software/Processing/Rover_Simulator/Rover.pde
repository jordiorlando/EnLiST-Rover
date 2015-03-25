public class Rover {
	// Wheel array
	Wheel[] wheels = new Wheel[6];
	private float fWidth, fHeight;

	Rover(float fWidthTemp, float fHeightTemp) {
		fWidth = fWidthTemp / 2;
		fHeight = fHeightTemp / 2;

		// Wheel declarations
		wheels[0] = new Wheel(STEERABLE, REVERSE, -(fWidth + 50), fHeight);
		wheels[1] = new Wheel(STEERABLE, NORMAL, fWidth + 50, fHeight);
		wheels[2] = new Wheel(STATIONARY, REVERSE, -(fWidth + 50), 0);
		wheels[3] = new Wheel(STATIONARY, NORMAL, fWidth + 50, 0);
		wheels[4] = new Wheel(STEERABLE, REVERSE, -(fWidth + 50), -fHeight);
		wheels[5] = new Wheel(STEERABLE, NORMAL, fWidth + 50, -fHeight);
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
			line(0, 0, 0, -fRoverVelocity / 2);
			line(-10, -fRoverVelocity / 2, 10, -fRoverVelocity / 2);
		}
	}

	// Draws a nice curvy vector-thing that represents the rover's rotation about
	// its center.
	private void drawRotation() {
		noFill();
		stroke(0, 0, 0, 255);
		ellipseMode(RADIUS);

		// Only necessary because Processing does not allow negative angles in arcs
		if (fRoverRotation < 0) {
			arc(0, 0, 50, 50, 0, -fRoverRotation * TWO_PI);
		} else {
			arc(0, 0, 50, 50, TWO_PI - fRoverRotation * TWO_PI, TWO_PI);
		}

		// Only draw the tip of the arrow if the rover is actually turning
		if (fRoverRotation != 0) {
			point(0, 0);
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

	// Draws the camera mast on the front of the rover
	private void drawMast() {
		pushMatrix();
		translate(0, -175);
		if (bMastMode) {
			float fNew = X2Stick * PI / 100;
			if (abs(fMastPan + fNew) <= PI) {
				fMastPan += fNew;
			}
		} else {
			fMastPan = X2Stick * PI;
		}
		rotate(fMastPan);

		fill(48, 48, 48, 255);
		noStroke();
		rectMode(RADIUS);
		rect(0, 0, 50, 15, 10);
		rect(0, 10, 25, 10, 10);

		popMatrix();
	}

	void draw() {
		// Center the matrix (from now on, the origin is at (640, 360))
		pushMatrix();
		translate(width / 2, height / 2);

		for (Wheel wheel : wheels) {
			// Update all wheels
			wheel.update();

			if (wheel.bPressed) {
				wheel.drawRadius();
			}
		}

		if (bDriveMode) {
			drawBody();
			drawMast();
			if (drawRoverRotation.pressed()) {
				drawRotation();
			}
		} else {
			if (drawRoverRadius.pressed()) {
				drawRadius();
			}
			drawBody();
			drawMast();
			if (drawRoverVelocity.pressed()) {
				drawVelocity();
			}
		}

		// Draw all wheels
		for (Wheel wheel : wheels) {
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
}
