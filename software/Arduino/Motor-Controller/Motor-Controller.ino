#include <TinyWireS.h>
#include <TinyServo.h>
#include <PID_v1.h>

// The default buffer size
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

// Change this to a unique address for each controller
#define MOTOR_ADDRESS	0x42

// Size of the I2C register
#define REGSIZE 16

// Pin definitions
#define IN1		0
#define IN2		1
#define STALL	2
#define STANDBY	3
#define LAMP	5
#define PWM		7
#define SERVO	8
#define QEA		9
#define QEB		10

/*	REGISTER LAYOUT

	nI2CReg[0]: speed control
		7:
		6:
		5:
		4:
		3: standby
			0: normal operation
			1: standby
		2: brake mode
			0: brake
			1: freewheel
		1: PID mode
			0: PID off
			1: PID on
		0: requested direction
			0: turn forwards
			1: turn backwards
	nI2CReg[1]: requested wheel speed [0, 255]
	nI2CReg[2]:
		7:
		6:
		5:
		4:
		3:
		2:
		1: stall status
			0: no stall
			1: stall
		0: current direction
			0: forwards
			1: backwards
	nI2CReg[3]: current wheel speed [0, 255]
	nI2CReg[4]: requested servo position [0, 180]
	nI2CReg[5]: requested LAMP brightness
	nI2CReg[6]: lamp mode control
		7:
		6:
		5:
		4:
		3:
		2:
		1:
		0: solid
	nI2CReg[7]:
	nI2CReg[8]:
	nI2CReg[9]:
	nI2CReg[10]: PID P value
	nI2CReg[11]:
	nI2CReg[12]: PID I value
	nI2CReg[13]:
	nI2CReg[14]: PID D value
	nI2CReg[15]:

*/
volatile uint8_t nI2CReg[REGSIZE]; // I2C register
volatile uint8_t nRegPos; // Current I2C register pointer position
volatile bool bNewStuff; // Flag for new I2C data

bool bStandby = true;
bool bBrakeMode = 0;
bool bPIDMode = 0;

// Number of servos
const byte SERVOS = 1;
// Array of servos for library
const byte servoPin[SERVOS] = {SERVO};
// Nice alias for use with the servo library
#define STEERING_SERVO	0

// All required PID variables
double fQE, fPWM, fSpeed, Kp = 2, Ki = 0.1, Kd = 1;
// Declare PID loop
PID speedPID(&fQE, &fPWM, &fSpeed, Kp, Ki, Kd, DIRECT);

void setup() {
	TinyWireS.begin(MOTOR_ADDRESS); // Connect to I2C with specified address
	TinyWireS.onReceive(receiveEvent); // Register receive event
	TinyWireS.onRequest(requestEvent); // Register request event

	setupServos();

	// Initialize PID variables
	fQE = 0;
	fSpeed = 0;

	// Turn the PID controller on
	speedPID.SetMode(AUTOMATIC);
	speedPID.SetSampleTime(10);
	speedPID.SetOutputLimits(-255, 255);

	// Set IO directions
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(STALL, OUTPUT);
	pinMode(STANDBY, OUTPUT);
	pinMode(LAMP, OUTPUT);
	pinMode(PWM, OUTPUT);
	//pinMode(SERVO, OUTPUT); // DO NOT uncomment this line.
	pinMode(QEA, INPUT);
	pinMode(QEB, INPUT);

	bNewStuff = true; // Initial read
}

void loop() {
	TinyWireS_stop_check(); // Has to run often to detect I2C stop state

	if (bNewStuff) {
		// Update standby
		bStandby = nI2CReg[0] & (1 << 3);
		digitalWrite(STANDBY, !bStandby);

		// Update brake mode
		bBrakeMode = nI2CReg[0] & (1 << 2);

		// Update PID mode
		bPIDMode = nI2CReg[0] & (1 << 1);
		if (bPIDMode) {
			speedPID.SetMode(AUTOMATIC);
		} else {
			speedPID.SetMode(MANUAL);
		}

		// Get requested speed
		if (nI2CReg[0] & 1) {
			fSpeed = nI2CReg[1] * -1;
		} else {
			fSpeed =  nI2CReg[1];
		}

		analogWrite(LAMP, nI2CReg[5]); // Update lamp brightness

		bNewStuff = false; // Reset I2C flag
	}

	if (bStandby) {
	} else {
		if (bPIDMode) {
			fQE = 5; // Test speed reading of 5 (replace with actual QE function call)
			speedPID.Compute(); // Compute PID
		} else {
			fPWM = fSpeed;
		}

		// Update motor speed
		if (fPWM == 0) {
			// Freewheel
			if (bBrakeMode) {
				digitalWrite(IN1, LOW);
				digitalWrite(IN2, LOW);
			} else { // Normal
				digitalWrite(IN1, HIGH);
				digitalWrite(IN2, HIGH);
				digitalWrite(PWM, LOW);
			}
		} else if (fPWM > 0) {
			digitalWrite(IN1, LOW);
			digitalWrite(IN2, HIGH);
			analogWrite(PWM, (uint8_t)fPWM);
		} else {
			digitalWrite(IN1, HIGH);
			digitalWrite(IN2, LOW);
			analogWrite(PWM, (uint8_t)abs(fPWM));
		}

		// Save current speed to register
		if (fQE >= 0) {
			nI2CReg[2] = 0;
			nI2CReg[3] = (uint8_t)fQE;
		} else {
			nI2CReg[2] = 1;
			nI2CReg[3] = (uint8_t)abs(fQE);
		}

		// Set servo position
		moveServo(STEERING_SERVO, nI2CReg[4]);
	}
}

void receiveEvent(uint8_t nNumBytes) {
	if (!nNumBytes || nNumBytes > TWI_RX_BUFFER_SIZE) return; // Sanity check

	// Set I2C register pointer position
	nRegPos = TinyWireS.receive();

	// Leave if this write was only to set the pointer for next read
	if (!--nNumBytes) return;

	while (nNumBytes--) {
		nI2CReg[nRegPos] = TinyWireS.receive(); // Receive byte
		nRegPos++; // Increment I2C register pointer

		// If register position is invalid, fail silently
		if (nRegPos == REGSIZE) {
			while (nNumBytes--) {
				TinyWireS.receive();
			}
			return;
		}
	}

	bNewStuff = true;
}

void requestEvent() {
	TinyWireS.send(nI2CReg[nRegPos]); // Send byte

	// Increment the reg position on each read, and loop back to zero
	nRegPos++;
	if (nRegPos >= REGSIZE) {
		nRegPos = 0;
	}
}
