//************************************************************
// File: CS328_lab5_wheels
// Purpose: Control speed of wheel rotation via PWM input
// Receive data to the bluetooth module
// Group 5: Elena Van, Raner Chow, Audrey Saidel
//************************************************************
// MOTOR WHEELS
#define MotorPWM_A 4 // Left motor
#define MotorPWM_B 5 // Right motor
// Left Motor Rotation pins
#define INA1A 32
#define INA2A 34
// Right Motor Rotation pins
#define INA1B 30
#define INA2B 36

// ENCODER
#define ENCODER_LEFT 2  // Encoder for Left Motor 
#define ENCODER_RIGHT 3  // Encoder for Right Motor 

// Encoder counters for counting pulses
static volatile int16_t count_left=0;
static volatile int16_t count_right = 0;

// 3.215 = (60sec/0.1sec)/(48gear ratio * 4pulses/rev)
float rotation=3.125;  
float RPM_left = 0;
float RPM_right = 0;

bool testStarted = false; // Keeps track if the test is running

//************************************************************
// Method: ISRMotorLeft() & ISRMotorRight()
// Input: NONE
// Description: Function that is called during interrupt to 
//    make sure the pulses are counted
//************************************************************
void ISRMotorLeft()
{
  count_left++; // increment pulse counter for left motor
}
void ISRMotorRight() 
{
  count_right++; // increment pulse counter for right motor
}

//************************************************************
// Method: setup
// Input: ENCODER_LEFT, ENCODER_RIGHT
// Description: Initialize bluetooth on Serial2
//    set pins for motors and encoders
// 
//************************************************************
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
//************************************************************
// Method: loop()
// Input: NONE
// Description: Initialize bluetooth on Serial2
//    set pins for motors and encoders
// 
//************************************************************
void loop(){
 if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim(); // remove newline or spaces
    // Start command starts the testing
    if (command.equalsIgnoreCase("start") && !testStarted) {
      testStarted = true;
      bluetoothStart();
    }
  }
}

//************************************************************
// Method: Forward()
// Input: int speed - value(0-255)
// Description:  Rotate the motor in a clockwise fashion
//    in order to go forward
//    speed determines how much power is sent
//************************************************************
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

//************************************************************
// Method: increaseSpeed()
// Input: NONE
// Description: Gradually increases speed (PWM) in from 0-255
//   incrementing by 5. Also calculate RPM
//   
//************************************************************
void increaseSpeed() {
  for (int speed = 0; speed <= 255; speed += 5) { // Increments from 0-255 by five
    // Everytime speed is changed pulse counters sent back to zero
    count_left = 0;
    count_right = 0;
    // Calls forward function with desired speed
    Forward(speed);
    delay(100); // Window 
    // Calculate RPM for speed 
    RPM_left = count_left * rotation;
    RPM_right = count_right * rotation;
    // Send results to bluetooth module
   sendBluetooth(speed, RPM_left, RPM_right);
  }
    // Shut off motors
    analogWrite(MotorPWM_A, 0);
    analogWrite(MotorPWM_B, 0);
    Serial2.println("Test complete.");
}

//************************************************************
// Method: sendBluetooth()
// Input: float PWM, float leftRPM, float rightRPM
// Description: Send PWM and RPM to the bluetooth module
//   outputs: (PWM, LEFTRPM, RIGHTRPM)
//   for graphing points
//************************************************************
void sendBluetooth(float PWM,float leftRPM, float rightRPM ){
  Serial2.print(PWM);
  Serial2.print(", ");
  Serial2.print(leftRPM);
  Serial2.print(", ");
  Serial2.println(rightRPM);
}

//************************************************************
// Method: bluetoothStart()
// Input: NONE
// Description: Triggers when start is sent to the bluetooth module
//  Sets the boolea condition to false at the end so we can restart test
//   Shows when test is started and test is completed
//************************************************************
void bluetoothStart() {
  Serial2.println("Starting motor test...");
  Serial.println("Starting motor test...");
  increaseSpeed();
  Serial2.println("Test complete.");
  Serial.println("Test complete.");
  testStarted = false; // Allow restarting if needed
}



