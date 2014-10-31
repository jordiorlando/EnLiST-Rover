#include <Wire.h>
#include <PID_v1.h>

#define IN1		0
#define IN2		1
#define STALL	2
#define STANDBY	3
#define LAMP	5
#define PWM		7
#define SERVO	8
#define QEA		9
#define QEB		10

double fQE, fPWM, fSpeed, P, I, D;
PID speedPID(&fQE, &fPWM, &fSpeed, P, I, D, DIRECT);

void setup()
{
	Wire.begin(4);                // join i2c bus with address #4
	Wire.onReceive(receiveEvent); // register event

	// initialize PID variables
	fQE = 0;
	fSpeed = 0;

	// turn the PID controller on
	speedPID.SetMode(AUTOMATIC);
	speedPID.SetSampleTime(10);
}

void loop()
{
	fQE = 5;
	speedPID.Compute();
	analogWrite(PWM, fPWM);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
	uint8_t nAvailableBytes = Wire.available();
	char chCommand[nAvailableBytes];

	for (uint8_t i = 0; i < nAvailableBytes; i++) {
		chCommand[i] = Wire.read();
	}
}
