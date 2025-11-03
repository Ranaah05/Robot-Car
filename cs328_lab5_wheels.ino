// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// MOTOR WHEELS
#define MotorPWM_A 4 //left motor
#define MotorPWM_B 5 //right motor

#define INA1A 32
#define INA2A 34
#define INA1B 30
#define INA2B 36

// ENCODER
#define ENCODER_LEFT 2  // Motor 1
#define ENCODER_RIGHT 3  //Motor 2


static volatile int16_t count_left=0;
static volatile int16_t count_right = 0;

// 3.215 = (60sec/0.1sec)/(48gear ratio * 4pulses/rev)
float rotation=3.125;  
float RPM_left = 0;
float RPM_right = 0;

bool testStarted = false; // Keeps track if the test is running
/***************************************/
// This is the Interrupt Service Routine
// for the left motor.  
/***************************************/
void ISRMotorLeft()
{
  count_left++;
}
void ISRMotorRight() 
{
  count_right++;
}

void setup() {
    Serial.begin(9600);  // Start serial output for monitoring
    Serial2.begin(38400);
    pinMode(MotorPWM_A, OUTPUT);
    pinMode(MotorPWM_B, OUTPUT);
    pinMode(INA1A, OUTPUT);
    pinMode(INA2A, OUTPUT);
    pinMode(INA1B, OUTPUT);
    pinMode(INA2B, OUTPUT);

    pinMode(ENCODER_LEFT, INPUT_PULLUP);
    pinMode(ENCODER_RIGHT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT), ISRMotorLeft, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT), ISRMotorRight, FALLING);

    Serial.println("Starting test: \n");
}

void loop(){
 /* delay(5000);
  increaseSpeed();
  while(1);
*/
 if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim(); // remove newline or spaces

    if (command.equalsIgnoreCase("start") && !testStarted) {
      testStarted = true;
      bluetoothStart();
    }
  }
}
// Method: Forward
// Input: speed – value [0-255]
// Rotate the motor in a clockwise fashion

void Forward(int speed)
{
  analogWrite(MotorPWM_A, speed);
  analogWrite(MotorPWM_B, speed);

  // Left Motor
  digitalWrite(INA1A, HIGH);
  digitalWrite(INA2A, LOW);

  // Right Motor
  digitalWrite(INA1B, HIGH);
  digitalWrite(INA2B, LOW);
}

// Gradually increase speed from 0 to 255
void increaseSpeed() {
  for (int speed = 0; speed <= 255; speed += 5) {
    count_left=0;
    count_right = 0;

    Forward(speed);
    delay(500); 
    RPM_left = count_left * rotation;
    RPM_right = count_right * rotation;

   sendBluetooth(speed, RPM_left, RPM_right);
  }
    analogWrite(MotorPWM_A, 0);
    analogWrite(MotorPWM_B, 0);
    Serial2.println("Test complete.");
}

void sendBluetooth(float PWM,float leftRPM, float rightRPM ){
  Serial2.print(PWM);
  Serial2.print(",");
  Serial2.print(leftRPM);
  Serial2.print(",");
  Serial2.println(rightRPM);
}

void bluetoothStart() {
  Serial2.println("Starting motor test...");
  Serial.println("Starting motor test...");
  increaseSpeed();
  Serial2.println("Test complete.");
  Serial.println("Test complete.");
  testStarted = false; // Allow restarting if needed
}



