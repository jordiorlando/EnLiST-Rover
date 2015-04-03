//
// tylernt@gmail.com's ATTiny Hardware Timer Assisted Servo Library v1.0 20-Nov-13
//
// http://code.google.com/p/arduino-tiny/ pinouts
//
//                  ATtiny25/45/85
//                      +-\/-+
//              RESET  1|    |8  VCC
//               3/A3  2|    |7  2/A1
//               4/A2  3|    |6  1
//                GND  4|    |5  0
//                      +----+
//
//                  ATtiny24/44/84
//                      +-\/-+
//                VCC  1|    |14  GND
//                 10  2|    |13  0/A0
//                  9  3|    |12  1/A1
//              RESET  4|    |11  2/A2
//                  8  5|    |10  3/A3
//               7/A7  6|    |9   4/A4
//               6/A6  7|    |8   5/A5
//                      +----+
//
//                    ATtiny2313
//                      +-\/-+
//              RESET  1|    |20  VCC
//                  0  2|    |19  16
//                  1  3|    |18  15
//                  2  4|    |17  14
//                  3  5|    |16  13
//                  4  6|    |15  12
//                  5  7|    |14  11
//                  6  8|    |13  10
//                  7  9|    |12  9
//                GND 10|    |11  8
//                      +----+

#include <TinyServo.h>
const byte SERVOS = 3; // how many servos do you have? up to 5 on ATTiny85 and 8 on ATtiny84/2313
const byte servoPin[SERVOS] = { 2, 4, 0 }; // what pins are your servos on?
// you have the option to give your servos nice names. 0 refers to the first servo pin above, 1 to the second, etc
#define HEADSERVO 0
#define LEFTLEGSERVO 1
#define RIGHTLEGSERVO 2

// ========================================================================================================================
void setup()
{

setupServos();

}

// ========================================================================================================================
void loop()
{

delay(1000);

// servos can be moved individually, by name, to 0° to 180°
moveServo(LEFTLEGSERVO, 180);
moveServo(RIGHTLEGSERVO, 180);
delay(1000);
moveServo(LEFTLEGSERVO, 0);
moveServo(RIGHTLEGSERVO, 0);
delay(1000);

// servos can also be moved en-masse with a loop
for(byte i=0; i <= 180; i++)
  {
  for(byte servo=0; servo < SERVOS; servo++)
    {
    moveServo(servo, i);
    delay(50);
    }
  }

moveServo(LEFTLEGSERVO, 0);
moveServo(RIGHTLEGSERVO, 0);
delay(1000);

// you also have the option of precisely positioning servos by writing directly to the servoPos variables
// to move to 0°, set a value of 0. the value required to move to 180° depends on what chip, timer, and clock speed TinyServo is running on/at:
//       ATtinyx4 / x313 = 225
// ATTinyx5 16MHz Timer1 = 113
// ATTinyx5 16MHz Timer0 = 113
// ATTinyx5 8MHz  Timer1 = 113
// ATTinyx5 8MHz  Timer0 = 57
// The Core defaults to Timer1 on the ATtinyx5, it's recommended you move it to Timer0 by editing /hardware/arduino/cores/tiny/core_build_options.h
servoPos[HEADSERVO] = 50;
delay(1000);

// The maximum value is also available via the SERVOMAX constant
servoPos[HEADSERVO] = 0;
delay(1000);
servoPos[HEADSERVO] = SERVOMAX;

}
