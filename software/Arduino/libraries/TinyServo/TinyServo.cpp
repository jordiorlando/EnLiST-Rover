/*
=================
TinyServo Library
=================
*/

#include "TinyServo.h"

// servoPos stores the number of timer ticks for this servo's HIGH pulse
#if defined( __AVR_ATtinyX313__ ) || defined( __AVR_ATtinyX4__ )
	byte servoPos[8];
#elif defined( __AVR_ATtinyX5__ )
	byte servoPos[5];
#endif

// Used to track which servo gets it's HIGH pulse next
volatile byte currentServo = 0;
// When we only have a few servos, there's more than 255 timer ticks of dead
// time between events; this is used to count the number of 255 tick dead time
// intervals we need to add up. Also used (when negative) to track whether an
// interrupt is supposed to set a servo HIGH or LOW.
volatile char servoControlRegister = 0;

// =============================================================================

// Translates a desired servo angle of 0 to 180° into the number of timer ticks
// the HIGH pulse should be.
void moveServo(const byte servo, const byte pos) {
	servoPos[servo] = pos * (word)SERVOMAX / 180;
}

// =============================================================================

void setupServos() {
	// Required to set up the hardware timer
	noInterrupts();

	#if defined( __AVR_ATtinyX313__ ) || defined( __AVR_ATtinyX4__ )
		// Timer1 Normal operation, OC1A/OC1B (the hardware PWM output pins)
		// disconnected, clearing existing bits.
		TCCR1A = 0;

		// Timer1 waveform=clear on timer compare, clearing existing bits
		TCCR1B = (1 << WGM12);

		// Prescale of 64, preserving existing bits
		TCCR1B |= (1 << CS10) | (1 << CS11);

		// Timer1 Output Compare A Match Interrupt (TIM1_COMPA) Enable, clearing
		// existing bits.
		TIMSKn = (1 << OCIE1A);
	#elif defined( __AVR_ATtinyX5__ )
		#if TIMER_TO_USE_FOR_MILLIS == 1 // We must use crummy timer0, since the Core is using timer1
			// Set Clear Timer on Compare Match (CTC) mode, clearing other bits
			TCCR0A = (1 << WGM01);

			// Clock select, clkI/O/256 from prescaler, clearing other bits
			TCCR0B = (1 << CS02);

			// Timer/Counter0 Output Compare Match A Interrupt Enable,
			// preserving other bits so we don't break millis().
			TIMSKn |= (1 << OCIE0A);
		#else // We get to use the nicer timer1
			// Set Clear Timer on Compare Match (CTC) mode, clearing other bits
			TCCR1 = (1 << CTC1);
			#if F_CPU == 8000000
				// Clock select, clkI/O/128 from prescaler, preserving other
				// bits.
				TCCR1 |= (1 << CS13);
			#elif F_CPU == 16000000
				// Clock select, clkI/O/256 from prescaler, preserving other
				// bits.
				TCCR1 |= (1 << CS10) | (1 << CS13);
			#endif
			
			// Timer/Counter1 Output Compare Match A Interrupt Enable,
			// preserving other bits so we don't break millis().
			TIMSKn |= (1 << OCIE1A);
		#endif
	#endif

	OCRxA = 255; // Output Compare Register -- how many ticks before calling ISR

	for(byte i=0; i < SERVOS; i++) {
		pinMode(servoPin[i], OUTPUT);
		moveServo(i, 90);
	}

	interrupts();
}

// =============================================================================

#if defined( __AVR_ATtinyX313__ )
	ISR(TIMER1_COMPA_vect)
		{
#elif defined( __AVR_ATtinyX4__ )
	ISR(TIM1_COMPA_vect)
		{
#elif defined( __AVR_ATtinyX5__ )
	#if TIMER_TO_USE_FOR_MILLIS == 1
		ISR(TIM0_COMPA_vect)
	#else
		ISR(TIM1_COMPA_vect)
	#endif
	{
		// the sad reality of using an 8-bit timer is sometimes we need to time things for longer than 255 ticks;
		// servoControlRegister lets us do that. Each decrement of servoControlRegister = 255 ticks.
		// the extra interrupts add quite a bit of jitter to the dead time, but dead time jitter doesn't matter nearly as mich as HIGH pulse jitter
		if(servoControlRegister > 0) {
			servoControlRegister--;
			OCRxA = 255;
			return;
		}
#endif

	if(servoControlRegister == 0) { // finished with any dead time we needed, time to send the HIGH pulse
		servoControlRegister = -1;
		OCRxA = servoPos[currentServo] + SERVOMIN; // schedule end of the HIGH pulse
		digitalWrite(servoPin[currentServo], HIGH);
	} else { // servo has finished it's entire HIGH pulse and now needs a long LOW rest
		digitalWrite(servoPin[currentServo], LOW);
		#if defined( __AVR_ATtinyX313__ ) || defined( __AVR_ATtinyX4__ )
			OCRxA = SERVOTIMESLICE - servoPos[currentServo];
			servoControlRegister = 0;
		#elif defined( __AVR_ATtinyX5__ )
			OCRxA = (SERVOTIMESLICE - servoPos[currentServo]) & 255; // schedule the remainder for the next servo using fast bitwise math instead of slower modulus 255
			servoControlRegister = (SERVOTIMESLICE - servoPos[currentServo]) >> 8; // schedule the number of 255-tick-dead-time periods necessary using fast bitwise math instead of slower division by 255
		#endif

		// Next servo's turn
		currentServo++;

		// Finished servicing all servos, so...
		if(currentServo == SERVOS) {
			// ...start over with the first
			currentServo = 0;
		}
	}

	return;
}
