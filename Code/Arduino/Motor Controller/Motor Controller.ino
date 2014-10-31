#include <Wire.h>
#include <PID_v1.h>

#define MOTOR_ADDRESS	0x42

#define IN1		0
#define IN2		1
#define STALL	2
#define STANDBY	3
#define LAMP	5
#define PWM		7
#define SERVO	8
#define QEA		9
#define QEB		10

int16_t nServo;
double fQE, fPWM, fSpeed, P, I, D;
PID speedPID(&fQE, &fPWM, &fSpeed, P, I, D, DIRECT);

void setup()
{
	// connect to I2C with specified address
	Wire.begin(MOTOR_ADDRESS);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);

	// initialize PID variables
	fQE = 0;
	fSpeed = 0;

	// turn the PID controller on
	speedPID.SetMode(AUTOMATIC);
	speedPID.SetSampleTime(10);

	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(STALL, OUTPUT);
	pinMode(STANDBY, OUTPUT);
	pinMode(LAMP, OUTPUT);
	pinMode(PWM, OUTPUT);
	pinMode(SERVO, OUTPUT);
	pinMode(QEA, INPUT);
	pinMode(QEB, INPUT);

	digitalWrite(STANDBY, HIGH);
}

void loop()
{
	fQE = 5;
	speedPID.Compute();
	analogWrite(PWM, (uint8_t)abs(fPWM));
}

void receiveEvent(int nNumBytes)
{
	uint8_t nBytes[nNumBytes];

	for (uint8_t i = 0; i < nNumBytes; i++) {
		nBytes[i] = Wire.read();
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
			digitalWrite(LAMP, HIGH);
			break;
	}
}

void requestEvent()
{
	uint8_t nSpeed = ((int32_t)fSpeed >> 24) & 0x000000FF;
	Wire.write(nSpeed);
	nSpeed = ((int32_t)fSpeed >> 16) & 0x000000FF;
	Wire.write(nSpeed);
	nSpeed = ((int32_t)fSpeed >> 8) & 0x000000FF;
	Wire.write(nSpeed);
	nSpeed = (int32_t)fSpeed & 0x000000FF;
	Wire.write(nSpeed);

	Wire.write("hello ");
}
