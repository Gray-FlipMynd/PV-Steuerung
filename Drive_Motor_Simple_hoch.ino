// Pin definitions
const int R_PWM = 23;
const int L_PWM = 19;
const int R_EN = 22;
const int L_EN = 21;


void setup() {
  // put your setup code here, to run once:
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i = 0; i < 180; i++) {  // 3 minutes = 180 seconds
      analogWrite(R_PWM, 0);  // Change the value (0-255) for different speeds
      analogWrite(L_PWM, 255);
      delay(1000);
    }

    // Turn off the motor
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, 0);

}
