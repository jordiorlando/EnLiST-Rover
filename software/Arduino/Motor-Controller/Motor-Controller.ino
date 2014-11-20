#include <TinyWireS.h>
#include <Servo8Bit.h>
#include <PID_v1.h>

// the default buffer size
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

#define MOTOR_ADDRESS	0x42 // change this to a unique address

#define IN1		0
#define IN2		1
#define STALL	2
#define STANDBY	3
#define LAMP	5
#define PWM		7
#define SERVO	8
#define QEA		9
#define QEB		10

volatile uint8_t nI2CReg[16]; // I2C register
volatile uint8_t nRegPos; // current I2C register pointer position
const uint8_t nRegSize = sizeof(nI2CReg); // size of the I2C register

int16_t nServo; // servo position (-180~180)
double fQE, fPWM, fSpeed, P, I, D; // all required PID variables
PID speedPID(&fQE, &fPWM, &fSpeed, P, I, D, DIRECT); // declare PID loop

void setup()
{
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

	//digitalWrite(STANDBY, HIGH); // start in standby mode
}

void loop()
{
	TinyWireS_stop_check(); // has to run often to detect I2C stop state

	fQE = 5; // test speed of 5 (replace with actual QE reading)
	speedPID.Compute(); // compute PID
	analogWrite(PWM, (uint8_t)abs(fPWM)); // update motor speed

	// break speed apart into 4 bytes for I2C register
	nI2CReg[0] = ((int32_t)fSpeed >> 24) & 0x000000FF;
	nI2CReg[1] = ((int32_t)fSpeed >> 16) & 0x000000FF;
	nI2CReg[2] = ((int32_t)fSpeed >> 8) & 0x000000FF;
	nI2CReg[3] = (int32_t)fSpeed & 0x000000FF;
}

void receiveEvent(uint8_t nNumBytes)
{
	// sanity check
	if (nNumBytes < 1 || nNumBytes > TWI_RX_BUFFER_SIZE) {
		return;
	}

	nRegPos = TinyWireS.receive(); // set I2C register pointer position

	// check if this write was only to set the pointer for next read
	nNumBytes--;
	if (!nNumBytes) {
		return;
	}

	volatile uint8_t nBytes[nNumBytes];

	for (uint8_t i = 0; i < nNumBytes; i++) {
		nBytes[i] = TinyWireS.receive();
	}

	switch (nBytes[0])
	{
		case 0x00: // standby
			digitalWrite(STANDBY, LOW);
			fSpeed = 0;
			break;
		case 0x01: // freewheel
			digitalWrite(IN1, LOW);
			digitalWrite(IN2, LOW);
			digitalWrite(STANDBY, HIGH);
			fSpeed = 0;
			break;
		case 0x02: // brake
			digitalWrite(IN1, HIGH);
			digitalWrite(IN2, HIGH);
			digitalWrite(STANDBY, HIGH);
			fSpeed = 0;
			break;
		case 0x03: // turn forwards
			digitalWrite(IN1, LOW);
			digitalWrite(IN2, HIGH);
			digitalWrite(STANDBY, HIGH);
			fSpeed = (double)nBytes[1];
			break;
		case 0x04: // turn backwards
			digitalWrite(IN1, HIGH);
			digitalWrite(IN2, LOW);
			digitalWrite(STANDBY, HIGH);
			fSpeed = (double)nBytes[1] * -1;
			break;
		case 0x05: // set servo
			nServo = nBytes[2];
			nServo |= (nBytes[1] << 8);
			break;
		case 0x06: // turn off lamp
			digitalWrite(LAMP, LOW);
			break;
		case 0x07: // turn on lamp
			analogWrite(LAMP, 127);
			break;
	}
}

void requestEvent()
{
	TinyWireS.send(nI2CReg[nRegPos]); // send byte

	// increment the reg position on each read, and loop back to zero
	nRegPos++;
	if (nRegPos >= nRegSize) {
		nRegPos = 0;
	}
}
