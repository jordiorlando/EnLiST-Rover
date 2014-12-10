#include <TinyWireS.h>
//#include <Servo8Bit.h>
#include <PID_v1.h>

// the default buffer size
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#define MOTOR_ADDRESS	42 // change this to a unique address

#define REGSIZE 16 // size of the I2C register

// pin definitions
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
	nI2CReg[1]: requested wheel speed (0~255)
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
			0: turn forwards
			1: turn backwards
	nI2CReg[3]: current wheel speed (0~255)
	nI2CReg[4]: requested servo position (0~180)
	nI2CReg[5]: requested LAMP brightness
	nI2CReg[6]: lamp mode control
		7:
		6:
		5:
		4:
		3:
		2:
		1:
		0:
	nI2CReg[7]:
	nI2CReg[8]: requested P
	nI2CReg[9]:
	nI2CReg[10]:
	nI2CReg[11]:
	nI2CReg[12]: requested I
	nI2CReg[13]:
	nI2CReg[14]:
	nI2CReg[15]:
	nI2CReg[16]: requested D
	nI2CReg[17]:
	nI2CReg[18]:
	nI2CReg[19]:

*/
volatile uint8_t nI2CReg[REGSIZE]; // I2C register
volatile uint8_t nRegPos; // current I2C register pointer position
volatile bool bNewStuff; // flag for new I2C data

bool bStandby = true;
bool bBrakeMode = 0;
bool bPIDMode = 0;

//Servo8Bit servo; // declare servo object

double fQE, fPWM, fSpeed, Kp = 2, Ki = 0.1, Kd = 1; // all required PID variables
PID speedPID(&fQE, &fPWM, &fSpeed, Kp, Ki, Kd, DIRECT); // declare PID loop

void setup() {
	TinyWireS.begin(MOTOR_ADDRESS); // connect to I2C with specified address
	TinyWireS.onReceive(receiveEvent); // register receive event
	TinyWireS.onRequest(requestEvent); // register request event

	//servo.attach(PB2);

	// initialize PID variables
	fQE = 0;
	fSpeed = 0;

	// turn the PID controller on
	speedPID.SetMode(AUTOMATIC);
	speedPID.SetSampleTime(10);
	speedPID.SetOutputLimits(-255, 255);

	// set IO directions
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(STALL, OUTPUT);
	pinMode(STANDBY, OUTPUT);
	pinMode(LAMP, OUTPUT);
	pinMode(PWM, OUTPUT);
	pinMode(SERVO, OUTPUT);
	pinMode(QEA, INPUT);
	pinMode(QEB, INPUT);

	bNewStuff = true; // initial read
}

void loop() {
	TinyWireS_stop_check(); // has to run often to detect I2C stop state

	if (bNewStuff) {
		// update standby
		bStandby = nI2CReg[0] & (1 << 3);
		digitalWrite(STANDBY, !bStandby);

		// update brake mode
		bBrakeMode = nI2CReg[0] & (1 << 2);

		// update PID mode
		bPIDMode = nI2CReg[0] & (1 << 1);
		if (bPIDMode) {
			speedPID.SetMode(AUTOMATIC);
		} else {
			speedPID.SetMode(MANUAL);
		}

		// get requested speed
		if (nI2CReg[0] & 1) {
			fSpeed = nI2CReg[1] * -1;
		} else {
			fSpeed =  nI2CReg[1];
		}

		// get servo position
		// servo function with speed = nI2CReg[4];

		analogWrite(LAMP, nI2CReg[5]); // update lamp brightness

		bNewStuff = false; // reset I2C flag
	}

	if (bStandby) {
	} else {
		if (bPIDMode) {
			fQE = 5; // test speed reading of 5 (replace with actual QE function call)
			speedPID.Compute(); // compute PID
		} else {
			fPWM = fSpeed;
		}

		// update motor speed
		if (fPWM == 0) {
			// freewheel
			if (bBrakeMode) {
				digitalWrite(IN1, LOW);
				digitalWrite(IN2, LOW);
			} else {
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

		// save current speed to register
		if (fQE >= 0) {
			nI2CReg[2] = 0;
			nI2CReg[3] = (uint8_t)fQE;
		} else {
			nI2CReg[2] = 1;
			nI2CReg[3] = (uint8_t)abs(fQE);
		}
	}
}

void receiveEvent(uint8_t nNumBytes) {
	if (!nNumBytes || nNumBytes > TWI_RX_BUFFER_SIZE) return; // sanity check

	nRegPos = TinyWireS.receive(); // set I2C register pointer position

	// leave if this write was only to set the pointer for next read
	if (!--nNumBytes) return;

	while (nNumBytes--) {
		nI2CReg[nRegPos] = TinyWireS.receive();
		nRegPos++; // increment I2C register pointer

		// if register position is invalid, fail silently
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
	TinyWireS.send(nI2CReg[nRegPos]); // send byte

	// increment the reg position on each read, and loop back to zero
	nRegPos++;
	if (nRegPos >= REGSIZE) {
		nRegPos = 0;
	}
}
