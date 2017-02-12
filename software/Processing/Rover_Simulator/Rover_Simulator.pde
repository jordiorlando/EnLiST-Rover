import processing.serial.*; // Only necessary for Arduino serial

import org.gamecontrolplus.gui.*;
import org.gamecontrolplus.*;
import net.java.games.input.*;

ControlIO control;
Configuration config;
ControlDevice gpad;

PFont f;  // Declare a font variable for on-screen text

// String to store the keyboard input
String inputString = "";

// Stick inputs, all of the range [-1, 1]
float X1Stick, Y1Stick, X2Stick, Y2Stick;

// Constant declarations
int maxSpeedValue = 255;
float radiusMultiplier = 500, radiusMultiplierTemp = 500;
// Global variables for defining the rover's motion
float roverRadius = 0, roverVelocity, roverRotation;
// Drive mode 0 is akin to a car, while 1 is only for rotating in place
boolean driveMode = false;
// Mast mode 0 is position control, while 1 is rate control
boolean mastMode = false;
// Angle of the mast
float mastPanAngle = 0;

// Toggle for drawing the wheel indicators
RadioButton drawWheelIndicators = new RadioButton(15, 30, 5);
// Toggles for drawing the rover's turn radius and velocity
RadioButton drawRoverRadius = new RadioButton(15, 50, 5);
RadioButton drawRoverVelocity = new RadioButton(15, 70, 5);
RadioButton drawRoverRotation = new RadioButton(15, 50, 5);

// Text field for the radius multiplier
TextField radiusMultiplierField = new TextField();

// The actual rover object. Includes body, wheels, and mast.
Rover rover = new Rover(250, 500);

boolean arduinoConnected = false;
// Declare a serial port for the Arduino
Serial arduinoPort;

public void setup() {
  // Set display size to the size of the computer's screen, but makes it
  // resizable as well.
  size(1280, 720);
  surface.setResizable(true);
  surface.setSize(displayWidth, displayHeight);

  // Define the font and use it
  f = createFont("Source Code Pro", 16, true);
  textFont(f, 16);
  textAlign(LEFT, CENTER);

  // Initialise the ControlIO
  control = ControlIO.getInstance(this);
  // Find a device that matches the configuration file
  gpad = control.getMatchedDevice("ChillStream");
  if (gpad == null) {
    println("No suitable device configured");
    System.exit(-1); // End the program NOW!
  }

  // Register a listener so that toggleDriveMode() is called every time the
  // left stick is pressed.
  gpad.getButton("B1").plug(this, "toggleDriveMode", ControlIO.ON_PRESS);

  // Register a listener so that toggleMastMode() is called every time the
  // right stick is pressed.
  gpad.getButton("B2").plug(this, "toggleMastMode", ControlIO.ON_PRESS);

  // Initialize radio buttons
  drawWheelIndicators.set(true);
  drawRoverRadius.set(true);
  drawRoverVelocity.set(true);
  drawRoverRotation.set(true);

  // Initialize the radius multiplier text field
  radiusMultiplierField.set(false);

  // Initialize serial port
  if (arduinoConnected) {
    //println(Serial.list()[9]);
    arduinoPort = new Serial(this, Serial.list()[9], 115200);
  }
}

public void draw() {
  // Global style
  background(200, 200, 200);
  strokeWeight(2);

  // Get gamepad values for the requisite sliders
  X1Stick = gpad.getSlider("X1").getValue();
  Y1Stick = gpad.getSlider("Y1").getValue();
  X2Stick = gpad.getSlider("X2").getValue();
  Y2Stick = gpad.getSlider("Y2").getValue();

  // Use mouse position instead of a gamepad
  /*X1Stick = (float(mouseX) * 2 / width) - 1;
  Y1Stick = 1 - (float(mouseY) * 2 / height);*/

  // Different rules for each mode
  if (driveMode) {
    roverRadius = 0;
    roverVelocity = 0;
    roverRotation = -X1Stick;
  } else {
    // Handle the endpoints of the functions. If the stick is all the way to
    // the left or the right, we have to set the rover radius equal to the
    // minimum positive or minimum negative float value, respectively.
    if (X1Stick == -1) {
      roverRadius = Float.MIN_NORMAL;
    } else if (X1Stick == 1) {
      roverRadius = -Float.MIN_NORMAL;
    } else {
      roverRadius = radiusMultiplier * tan(HALF_PI*(X1Stick + 1));
    }
    roverVelocity = Y1Stick * abs(roverRadius) / maxRadius(roverRadius);
  }

  // Draw the rover
  rover.draw();

  // Draw the HUD
  drawHUD();

  // Write wheel0's servo position to the Arduino
  if (arduinoConnected) {
    arduinoPort.write('v' + String.format("%.0f", rover.wheels[0].wheelVelocity()));
    arduinoPort.write('s' + String.format("%.0f", degrees(rover.wheels[0].servoAngle()) + 10));
  }
}

// Automatically called whenever a mouse button is pressed.
void mousePressed() {
  if (mouseButton == LEFT) {
    if (driveMode) {
      if (drawRoverRotation.over()) {
        drawRoverRotation.toggle();
      }
    } else {
      float radiusWidth = textWidth("Radius Multiplier: ") + 10;

      if (drawRoverRadius.over()) {
        drawRoverRadius.toggle();
      } else if (drawRoverVelocity.over()) {
        drawRoverVelocity.toggle();
      }

      if (radiusMultiplierField.over()) {
        radiusMultiplierField.set(true);
      } else {
        radiusMultiplierTemp = radiusMultiplier;
        radiusMultiplierField.set(false);
      }
    }

    if (drawWheelIndicators.over()) {
      drawWheelIndicators.toggle();
    } else if (rover.over()) {
      toggleDriveMode();
    } else {
      // Check if the mouse is over any of the wheels
      for (Rover.Wheel wheel : rover.wheels) {
        if (wheel.over()) {
          wheel.toggle();
        }
      }
    }
  }
}

// Automatically called whenever a key is pressed on the keyboard.
void keyPressed() {
  if (radiusMultiplierField.pressed()) {
    if (key == '\n' ) {
      radiusMultiplier = radiusMultiplierTemp;
      inputString = "";
    } else if (key == 8) {
      // If the backspace key is pressed, delete the last character in the
      // input string.
      if (inputString.length() > 1) {
        inputString = inputString.substring(0, inputString.length() - 1);
        radiusMultiplierTemp = Float.parseFloat(inputString);
      } else if (inputString.length() == 1) {
        inputString = "";
        radiusMultiplierTemp = radiusMultiplier;
      }
    } else {
      // Otherwise, concatenate the String. Each character typed by the
      // user is added to the end of the input String.
      if (key > 47 && key < 58) {
        inputString = inputString + key;
        radiusMultiplierTemp = Float.parseFloat(inputString);
      }
    }
  }
}

// Called whenever the left joystick button on the gamepad transitions from low
// to high. Toggles the control mode each time.
void toggleDriveMode() {
  driveMode = !driveMode;
}

// Called whenever the left joystick button on the gamepad transitions from low
// to high. Toggles the control mode each time.
void toggleMastMode() {
  mastMode = !mastMode;
}

// Returns the value of the maximum distance of any wheel from its center to the
// center of rotation of the rover.
float maxRadius(float r) {
  float max = 0;
  for (Rover.Wheel wheel : rover.wheels) {
    if (wheel.radius(r) > max) {
      max = wheel.radius(r);
    }
  }

  return max;
}

void drawHUD() {
  // Left side (drive)
  fill(48, 48, 48);
  drawWheelIndicators.draw("Wheel Indicators");
  // Different rules for each mode
  if (driveMode) {
    text("Drive Mode: 1", 10, 10);
    drawRoverRotation.draw("Rotation: " + roverRotation);
  } else {
    text("Drive Mode: 0", 10, 10);
    drawRoverRadius.draw("Radius: " + roverRadius);
    drawRoverVelocity.draw("Velocity: " + roverVelocity);
    text("Radius Multiplier: ", 10, height - 15);
    radiusMultiplierField.draw(String.format("%.0f", radiusMultiplierTemp), textWidth("Radius Multiplier: ") + 10, height - 15);
  }

  // Right side (mast)
  fill(48, 48, 48);
  strokeWeight(2);
  float rightTextWidth = textWidth("Mast Pan: -180" + (char)0x00B0) + 10;
  // Different rules for each mode
  if (mastMode) {
    text("Mast Mode: 1", width - rightTextWidth, 10);
  } else {
    text("Mast Mode: 0", width - rightTextWidth, 10);
  }
  text("Mast Pan: " + String.format("%.0f", degrees(mastPanAngle)) + (char)0x00B0, width - rightTextWidth, 30);
}
