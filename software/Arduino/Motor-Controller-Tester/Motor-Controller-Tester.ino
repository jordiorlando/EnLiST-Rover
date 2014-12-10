#include <Wire.h>

#define MOTOR_ADDRESS 42

String inString = "";    // string to hold input

bool bBrakeMode = 0;
bool bDirection;
uint8_t nSpeed = 0;
uint8_t nServo = 90;
uint8_t nLamp = 0;

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  
  Serial.begin(115200);
}

void loop()
{
  if (Serial.available()) {
    int inChar = Serial.read();
    
    if (isDigit(inChar)) {
      bDirection = 0;
      inString += (char)inChar;
      
      while (Serial.available()) {
        inChar = Serial.read();
        if (isDigit(inChar)) {
          // convert the incoming byte to a char
          // and add it to the string:
          inString += (char)inChar;
        }
        
        // if you get a newline, print the string,
        // then the string's value:
        if (inChar == '\n') {
          nSpeed = inString.toInt();
          Serial.print("SPEED: ");
          Serial.println(nSpeed);
        }
      }
    } else if ((char)inChar == '-') {
      bDirection = 1;
      
      while (Serial.available()) {
        inChar = Serial.read();
        if (isDigit(inChar)) {
          // convert the incoming byte to a char
          // and add it to the string:
          inString += (char)inChar;
        }
        
        // if you get a newline, print the string,
        // then the string's value:
        if (inChar == '\n') {
          nSpeed = inString.toInt();
          Serial.print("SPEED: -");
          Serial.println(nSpeed);
        }
      }
    } else if ((char)inChar == 'b') {
      inChar = Serial.read();
      if ((char)inChar == '0') {
        bBrakeMode = 0;
        Serial.println("Brake Mode: NORMAL");
      } else if ((char)inChar == '1') {
        bBrakeMode = 1;
        Serial.println("Brake Mode: FREEWHEEL");
      }
    } else if ((char)inChar == 'l') {
      int inChar = Serial.read();
    
      if (isDigit(inChar)) {
        inString += (char)inChar;
        
        while (Serial.available()) {
          inChar = Serial.read();
          if (isDigit(inChar)) {
            // convert the incoming byte to a char
            // and add it to the string:
            inString += (char)inChar;
          }
          
          // if you get a newline, print the string,
          // then the string's value:
          if (inChar == '\n') {
            nLamp = inString.toInt();
            Serial.print("LAMP: ");
            Serial.println(nLamp);
          }
        }
      }
    } else if ((char)inChar == 's') {
      int inChar = Serial.read();
    
      if (isDigit(inChar)) {
        inString += (char)inChar;
        
        while (Serial.available()) {
          inChar = Serial.read();
          if (isDigit(inChar)) {
            // convert the incoming byte to a char
            // and add it to the string:
            inString += (char)inChar;
          }
          
          // if you get a newline, print the string,
          // then the string's value:
          if (inChar == '\n') {
            nServo = inString.toInt();
            Serial.print("SERVO: ");
            Serial.println(nServo);
          }
        }
      }
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
    
    // clear the string for new input:
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
