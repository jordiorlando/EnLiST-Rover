#include <Wire.h>

#define MOTOR_ADDRESS 0x42

String inString = ""; // string to hold input

bool bBrakeMode = 0;
bool bDirection;
uint8_t nSpeed = 0;
uint8_t nServo = 90;
uint8_t nLamp = 0;

void setup()
{
	Wire.begin(); // Join i2c bus (address optional for master)

	// Disable I2C pullups so that we can use 3V3 I2C
	#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
	#endif
	cbi(PORTC, 4);
	cbi(PORTC, 5);

	// Open serial connection to processing sketch
	Serial.begin(115200);

	// TESTING
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
}

void loop()
{
	if (Serial.available()) {
		delay(1);
		int inChar = Serial.read();

		if ((char)inChar == 'v') {
			inChar = Serial.read();
			while (isDigit(inChar) || (char)inChar == '-') {
				inString += (char)inChar;
				inChar = Serial.read();
			}

			int nSpeedTemp = atoi(inString.c_str());

			if (nSpeedTemp < 0) {
				bDirection = 1;
			} else {
				bDirection = 0;
			}
			nSpeed = abs(nSpeedTemp);

			//Serial.print("SPEED: ");
			//Serial.println(nSpeedTemp);

			// Clear the string for new input:
			inString = "";
		}
		if ((char)inChar == 's') {
			inChar = Serial.read();
			while (isDigit(inChar)) {
				inString += (char)inChar;
				inChar = Serial.read();
			}

			nServo = atoi(inString.c_str());

			if (nServo > 120) {
				digitalWrite(13, HIGH);
			}

			//Serial.print("SERVO: ");
			//Serial.println(nServo);

			// Clear the string for new input:
			inString = "";
		}
		if ((char)inChar == 'b') {
			inChar = Serial.read();
			if ((char)inChar == '0') {
				bBrakeMode = 0;
				//Serial.println("Brake Mode: NORMAL");
			} else if ((char)inChar == '1') {
				bBrakeMode = 1;
				//Serial.println("Brake Mode: FREEWHEEL");
			}
			inChar = Serial.read();

			// Clear the string for new input:
			inString = "";
		}
		if ((char)inChar == 'l') {
			inChar = Serial.read();
			while (isDigit(inChar)) {
				inString += (char)inChar;
				inChar = Serial.read();
			}

			nLamp = atoi(inString.c_str());

			//Serial.print("LAMP: ");
			//Serial.println(nLamp);

			// Clear the string for new input:
			inString = "";
		}

		Wire.beginTransmission(MOTOR_ADDRESS); // transmit to device #42
		Wire.write(0); // register pointer: 0x00

		uint8_t nControl = 0;
		if (bDirection) {
			nControl = 1;
		}
		if (bBrakeMode) {
			nControl |= (1 << 2);
		}

		Wire.write(nControl); // 0x00
		Wire.write(nSpeed); // 0x01
		Wire.write(0); // 0x02
		Wire.write(0); // 0x03
		Wire.write(nServo); // 0x04
		Wire.write(nLamp); // 0x05

		Wire.endTransmission(); // stop transmitting

		// Clear the string for new input:
		inString = "";
	}

	delay(1);

	/*
	for (int i = 0; i < 256; i++) {
		Wire.beginTransmission(MOTOR_ADDRESS); // transmit to device #42
		Wire.write(5);
		Wire.write(i);
		Wire.endTransmission(); // stop transmitting

		delay(5);
	}
	for (int i = 255; i >= 0; i--) {
		Wire.beginTransmission(MOTOR_ADDRESS); // transmit to device #42
		Wire.write(5);
		Wire.write(i);
		Wire.endTransmission(); // stop transmitting

		delay(5);
	}
	*/
}
