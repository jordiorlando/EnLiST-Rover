#include <Wire.h>
#define MOTOR_ADDRESS 0x42

void setup()
{
	Wire.begin(MOTOR_ADDRESS);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);

	pinMode(13, OUTPUT);
}

void loop()
{
	delay(100);
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
