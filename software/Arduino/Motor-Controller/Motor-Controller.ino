#include <TinyWireS.h>
#include <Servo8Bit.h>
#include <PID_v1.h>

// the default buffer size
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#define MOTOR_ADDRESS	0x42 // change this to a unique address

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

	nI2CReg[0]: 0 for forwards, 1 for backwards
	nI2CReg[1]: requested wheel speed (0~255)
	nI2CReg[2]: 0 for forwards, 1 for backwards
	nI2CReg[3]: current wheel speed (0~255)
	nI2CReg[4]: requested servo position (0~180)
	nI2CReg[5]: requested LAMP brightness
	nI2CReg[6]:
		7:
		6:
		5:
		4:
		3:
		2:
		1: brake mode (0 = brake, 1 = freewheel)
		0: standby
	nI2CReg[7]: stall status

*/
volatile uint8_t nI2CReg[REGSIZE]; // I2C register
volatile uint8_t nRegPos; // current I2C register pointer position
volatile bool bNewStuff; // flag for new I2C data

bool bStandby = true;

double fQE, fPWM, fSpeed, P, I, D; // all required PID variables
PID speedPID(&fQE, &fPWM, &fSpeed, P, I, D, DIRECT); // declare PID loop

void setup() {
	TinyWireS.begin(MOTOR_ADDRESS); // connect to I2C with specified address
	TinyWireS.onReceive(receiveEvent); // register receive event
	TinyWireS.onRequest(requestEvent); // register request event

	// initialize PID variables
	fQE = 0;
	fSpeed = 0;

	// turn the PID controller on
	speedPID.SetMode(AUTOMATIC);
	speedPID.SetSampleTime(10);

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

	//digitalWrite(STANDBY, bStandby); // start in standby mode
}

void loop() {
	TinyWireS_stop_check(); // has to run often to detect I2C stop state

	if (bNewStuff) {
		// get requested speed
		if (!nI2CReg[0]) {
			fSpeed =  nI2CReg[1];
		} else {
			fSpeed = nI2CReg[1] * -1;
		}

		// get servo position
		// servo function with speed = nI2CReg[4];

		analogWrite(LAMP, nI2CReg[5]); // update lamp brightness

		// update standby
		bStandby = nI2CReg[6] % 2;
		digitalWrite(STANDBY, bStandby);

		bNewStuff = 0; // reset I2C flag
	}

	if (bStandby) {
		//speedPID.SetMode(MANUAL);
	} else {
		fQE = 5; // test speed reading of 5 (replace with actual QE function call)
		speedPID.Compute(); // compute PID

		// update motor speed
		if (fPWM == 0) {
			// freewheel
			if (nI2CReg[6] & 0x02) {
				digitalWrite(IN1, LOW);
				digitalWrite(IN2, LOW);
			}
		} else {
			if (fPWM > 0) {
				digitalWrite(IN1, LOW);
				digitalWrite(IN2, HIGH);
				analogWrite(PWM, (uint8_t)fPWM);
			} else {
				digitalWrite(IN1, HIGH);
				digitalWrite(IN2, LOW);
				analogWrite(PWM, (uint8_t)abs(fPWM));
			}
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
	// sanity check
	if (nNumBytes < 1 || nNumBytes > TWI_RX_BUFFER_SIZE) {
		return;
	}

	nRegPos = TinyWireS.receive(); // set I2C register pointer position

	// leave if this write was only to set the pointer for next read
	nNumBytes--;
	if (!nNumBytes) {
		return;
	}

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
