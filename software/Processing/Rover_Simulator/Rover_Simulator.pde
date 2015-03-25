/*import org.gamecontrolplus.gui.*;
import org.gamecontrolplus.*;
import net.java.games.input.*;

ControlIO control;
Configuration config;
ControlDevice gpad;*/

PFont f;  // Declare a font variable for on-screen text

// String to store the keyboard input
String sInputString = "";

// Stick inputs, all of the range [-1, 1]
float X1Stick, Y1Stick, X2Stick, Y2Stick;

// Constant declarations
int nMaxSpeed = 255;
float fRadiusFactor = 500, fRadiusFactorTemp = 500;
// Global variables for defining the rover's motion
float fRoverRadius = 0, fRoverVelocity, fRoverRotation;
// Drive mode 0 is akin to a car, while 1 is only for rotating in place
boolean bDriveMode = false;
// Mast mode 0 is position control, while 1 is rate control
boolean bMastMode = false;
// Angle of the mast
float fMastPan = 0;

// Toggle for drawing the wheel indicators
RadioButton drawWheelIndicators = new RadioButton(15, 30, 5);
// Toggles for drawing the rover's turn radius and velocity
RadioButton drawRoverRadius = new RadioButton(15, 50, 5);
RadioButton drawRoverVelocity = new RadioButton(15, 70, 5);
RadioButton drawRoverRotation = new RadioButton(15, 50, 5);

// Text field for the radius multiplier
TextField radiusFactor;

Rover rover = new Rover(200, 400);

public void setup() {
	size(displayWidth, displayHeight);

	// Define the font and use it
	f = createFont("Source Code Pro", 16, true);
	textFont(f, 16);
	textAlign(LEFT, CENTER);

	/*// Initialise the ControlIO
	control = ControlIO.getInstance(this);
	// Find a device that matches the configuration file
	gpad = control.getMatchedDevice("ChillStream");
	if (gpad == null) {
		println("No suitable device configured");
		System.exit(-1); // End the program NOW!
	}

	// Register a listener so that changeDriveMode() is called every time the
	// left stick is pressed.
	gpad.getButton("B1").plug(this, "changeDriveMode", ControlIO.ON_PRESS);

	// Register a listener so that changeMastMode() is called every time the
	// right stick is pressed.
	gpad.getButton("B2").plug(this, "changeMastMode", ControlIO.ON_PRESS);*/

	// Initialize radio buttons
	drawWheelIndicators.set(true);
	drawRoverRadius.set(true);
	drawRoverVelocity.set(true);
	drawRoverRotation.set(true);

	// BLAH
	radiusFactor = new TextField(textWidth("Radius Factor: ") + 10, height - 15);
	radiusFactor.set(false);
}

public void draw() {
	// Global style
	background(200, 200, 200);
	strokeWeight(2);

	// Get gamepad values for the requisite sliders
	/*X1Stick = gpad.getSlider("X1").getValue();
	Y1Stick = gpad.getSlider("Y1").getValue();
	X2Stick = gpad.getSlider("X2").getValue();
	Y2Stick = gpad.getSlider("Y2").getValue();*/

	// Use mouse position instead of a gamepad
	X1Stick = (float(mouseX) * 2 / width) - 1;
	Y1Stick = 1 - (float(mouseY) * 2 / height);

	// Different rules for each mode
	if (bDriveMode) {
		fRoverRadius = 0;
		fRoverVelocity = 0;
		fRoverRotation = -X1Stick;
	} else {
		// Handle the endpoints of the functions. If the stick is all the way to
		// the left or the right, we have to set the rover radius equal to the
		// minimum positive or minimum negative float value, respectively.
		if (X1Stick == -1) {
			fRoverRadius = Float.MIN_NORMAL;
		} else if (X1Stick == 1) {
			fRoverRadius = -Float.MIN_NORMAL;
		} else {
			fRoverRadius = fRadiusFactor * tan(HALF_PI*(X1Stick + 1));
		}
		fRoverVelocity = nMaxSpeed * Y1Stick * abs(fRoverRadius) / maxRadius(fRoverRadius);
	}

	rover.draw();

	// Display important information (HUD)
	fill(48, 48, 48);
	drawWheelIndicators.draw("Wheel Indicators");
	if (bDriveMode) {
		text("Drive Mode: 1", 10, 10);
		drawRoverRotation.draw("Rotation: " + fRoverRotation);
	} else {
		text("Drive Mode: 0", 10, 10);
		drawRoverRadius.draw("Radius: " + fRoverRadius);
		drawRoverVelocity.draw("Velocity: " + fRoverVelocity);
		text("Radius Factor: ", 10, height - 15);
		radiusFactor.draw(Float.toString(fRadiusFactorTemp));
	}

	fill(48, 48, 48);
	strokeWeight(2);
	float fRightTextWidth = textWidth("Mast Pan: -180" + (char)0x00B0) + 10;
	if (bMastMode) {
		text("Mast Mode: 1", width - fRightTextWidth, 10);
	} else {
		text("Mast Mode: 0", width - fRightTextWidth, 10);
	}
	text("Mast Pan: " + String.format("%.0f", degrees(fMastPan)) + (char)0x00B0, width - fRightTextWidth, 30);
}

// Automatically called whenever a mouse button is pressed.
void mousePressed() {
	if (mouseButton == LEFT) {
		if (bDriveMode) {
			if (drawRoverRotation.over()) {
				drawRoverRotation.toggle();
			}
		} else {
			float fRadiusWidth = textWidth("Radius Factor: ") + 10;

			if (drawRoverRadius.over()) {
				drawRoverRadius.toggle();
			} else if (drawRoverVelocity.over()) {
				drawRoverVelocity.toggle();
			}

			if (radiusFactor.over()) {
				radiusFactor.set(true);
			} else {
				fRadiusFactorTemp = fRadiusFactor;
				radiusFactor.set(false);
			}
		}

		if (drawWheelIndicators.over()) {
			drawWheelIndicators.toggle();
		} else if (rover.over()) {
			changeDriveMode();
		} else {
			// Check if the mouse is over any of the wheels
			for (Wheel wheel : rover.wheels) {
				if (wheel.over()) {
					wheel.bPressed = !wheel.bPressed;
				}
			}
		}
	}
}

// Automatically called whenever a key is pressed on the keyboard.
void keyPressed() {
	if (radiusFactor.pressed()) {
		if (key == '\n' ) {
			fRadiusFactor = fRadiusFactorTemp;
			sInputString = "";
		} else if (key == 8) {
			// If the backspace key is pressed, delete the last character in the
			// input string.
			if (sInputString.length() > 1) {
				sInputString = sInputString.substring(0, sInputString.length() - 1);
				fRadiusFactorTemp = Float.parseFloat(sInputString);
			} else if (sInputString.length() == 1) {
				sInputString = "";
				fRadiusFactorTemp = fRadiusFactor;
			}
		} else {
			// Otherwise, concatenate the String. Each character typed by the
			// user is added to the end of the input String.
			if (key > 47 && key < 58) {
				sInputString = sInputString + key;
				fRadiusFactorTemp = Float.parseFloat(sInputString);
			}
		}
	}
}

// Called whenever the left joystick button on the gamepad transitions from low
// to high. Toggles the control mode each time.
void changeDriveMode() {
	bDriveMode = !bDriveMode;
}

// Called whenever the left joystick button on the gamepad transitions from low
// to high. Toggles the control mode each time.
void changeMastMode() {
	bMastMode = !bMastMode;
}

// Returns the value of the maximum distance of any wheel from its center to the
// center of rotation of the rover.
float maxRadius(float fRadius) {
	float fMaxRadius = 0;
	for (Wheel wheel : rover.wheels) {
		if (wheel.radius(fRadius) > fMaxRadius) {
			fMaxRadius = wheel.radius(fRadius);
		}
	}

	return fMaxRadius;
}
