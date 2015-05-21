/*
================
TinyServo Header
================
*/

#ifndef TinyServo_h
#define TinyServo_h

#include <Arduino.h>
//#include <inttypes.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>

extern const byte SERVOS;
extern byte servoPos[];
extern const byte servoPin[];

#if defined( __AVR_ATtiny2313__ ) || defined( __AVR_ATtiny4313__ )
	#define __AVR_ATtinyX313__

	#define TIMSKn TIMSK
	#define OCRxA OCR1A

	#if F_CPU == 8000000
		// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
		// timer tick = 8uS with 64 prescaler (@ 8MHz)
		// this translates to a servo HIGH of 75 ticks to 300 ticks, with LOW of 2500 ticks
		// 75 to 300 ticks gives us a servo resolution of 225 steps
		const byte SERVOMIN = 75;
		const byte SERVOMAX = 225; // 300 - servomin
		#define SERVOTIMESLICE 2500/SERVOS
	#else
		#error TinyServo does not support this CPU clock speed
	#endif
#elif defined( __AVR_ATtiny24__ ) || defined( __AVR_ATtiny44__ ) || defined( __AVR_ATtiny84__ )
	#define __AVR_ATtinyX4__

	#define TIMSKn TIMSK1
	#define OCRxA OCR1A

	#if F_CPU == 8000000
		// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
		// timer tick = 8uS with 64 prescaler (@ 8MHz)
		// this translates to a servo HIGH of 75 ticks to 300 ticks, with LOW of 2500 ticks
		// 75 to 300 ticks gives us a servo resolution of 225 steps
		const byte SERVOMIN = 75;
		const byte SERVOMAX = 225; // 300 - servomin
		#define SERVOTIMESLICE 2500/SERVOS
	#else
		#error TinyServo does not support this CPU clock speed
	#endif
#elif defined( __AVR_ATtiny25__ ) || defined( __AVR_ATtiny45__ ) || defined( __AVR_ATtiny85__ )
	#define __AVR_ATtinyX5__

	#define TIMSKn TIMSK

	#if TIMER_TO_USE_FOR_MILLIS == 1
		#define OCRxA OCR0A
		#if F_CPU == 8000000
			#warning TinyServo recommends moving the Core to timer0 in /hardware/arduino/cores/tiny/core_build_options.h
			// the worst of all possible combinations
			// expect lots of servo twitching with this configuration!
			// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
			// timer tick with 256 prescaler = 32uS @ 8MHz
			// this translates to a servo HIGH of 18 ticks to 75 ticks, with LOW of 625 ticks
			// 18 to 75 ticks gives us a servo resolution of 57 steps
			const byte SERVOMIN = 18;
			const byte SERVOMAX = 57; // 75 - servomin
			#define SERVOTIMESLICE 625/SERVOS
		#elif F_CPU == 16000000
			// minor twitching was observed with this configuration. likely due to the millis() interrupt taking priority over ours
			// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
			// timer tick with 256 prescaler = 16uS @ 16MHz
			// this translates to a servo HIGH of 37 ticks to 150 ticks, with LOW of 1250 ticks
			// 37 to 150 ticks gives us a servo resolution of 113 steps
			const byte SERVOMIN = 37;
			const byte SERVOMAX = 113; // 150 - servomin
			#define SERVOTIMESLICE 1250/SERVOS
		#else
			#error TinyServo does not support this CPU clock speed
		#endif
	#else
		// we get to use the better timer1, better resolution (and a higher interrupt priority to reduce jitter)
		#define OCRxA OCR1A
		#if F_CPU == 8000000
			// no twitching observed with this configuration
			// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
			// timer tick = 16uS with 128 prescaler (@ 8MHz)
			// this translates to a servo HIGH of 37 ticks to 150 ticks, with LOW of 1250 ticks
			// 37 to 150 ticks gives us a servo resolution of 113 steps
		#elif F_CPU == 16000000
			// tiny bit of twitching observed with this configuration
			// each 50Hz servo needs a HIGH of 600uS to 2400uS, followed by a LOW of ~20,000uS
			// timer tick = 16uS with 256 prescaler (@ 16MHz)
			// this translates to a servo HIGH of 37 ticks to 150 ticks, with LOW of 1250 ticks
			// 37 to 150 ticks gives us a servo resolution of 113 steps
		#else
			#error TinyServo does not support this CPU clock speed
		#endif
		const byte SERVOMIN = 37;
		const byte SERVOMAX = 113; // 150 - servomin
		#define SERVOTIMESLICE 1250/SERVOS
	#endif
#else
	#error TinyServo library does not support this processor.
#endif

void moveServo(const byte servo, const byte pos);
void setupServos();

#endif
