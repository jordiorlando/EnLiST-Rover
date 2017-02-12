#define IN1	0
#define IN2	1
#define STALL	2
#define STANDBY	3
#define LAMP	5
#define PWM	7
#define SERVO	8
#define QEA	9
#define QEB	10

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(STALL, OUTPUT);
  pinMode(STANDBY, OUTPUT);
  pinMode(LAMP, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(QEA, INPUT);
  pinMode(QEB, INPUT);
  
  digitalWrite(STANDBY, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  analogWrite(PWM, 63);
}

void loop() {
  /*
  digitalWrite(STALL, digitalRead(QEA));
  digitalWrite(LAMP, digitalRead(QEB));
  */
  
  
  for (uint8_t i = 0; i < 255; i++) {
    analogWrite(LAMP, i);
    //analogWrite(PWM, i / 4);
    delay(10);
  }
  
  for (uint8_t i = 255; i > 0; i--) {
    analogWrite(LAMP, i);
    //analogWrite(PWM, i / 4);
    delay(10);
  }
  
}
