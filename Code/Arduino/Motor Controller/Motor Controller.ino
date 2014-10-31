#include <Wire.h>
#include <PID_v1.h>

#define MOTOR_ADDRESS 0x42
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
	Wire.begin(MOTOR_ADDRESS);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);

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

void receiveEvent(int howMany)
{
  while (0 < Wire.available())
  {
	uint8_t nByte = Wire.read();

	if (nByte == 2)
		digitalWrite(13, HIGH);
	else if (nByte == 1)
		digitalWrite(13, LOW);
  }
}

void requestEvent()
{
	Wire.write("hello ");
}
