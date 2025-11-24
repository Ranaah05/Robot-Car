//************************************************************
// Project One: Drive in a Square
// File: Project_1.ino
// Purpose: Program robot car to drive in a square, hit every corner
// Group 5: Elena Raner Audrey
//************************************************************
// Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PWM for Left and Right Motors
#define MotorPWM_A 4 // left - A
#define MotorPWM_B 5 // right - B
// Left Motor Rotation Pins
#define INA1A 32
#define INA2A 34
// Right Motor Rotation pins
#define INA1B 30
#define INA2B 36

// OLED Definitiosn
#define SCREEN_WIDTH 128 // Width in px
#define SCREEN_HEIGHT 64 // Height in px
#define LOGO_WIDTH  32
#define LOGO_HEIGHT 32
#define OLED_RESET -1    
#define SCREEN_ADDRESS 0x3C 

// Car Light Definitions
#define LED_FLT_pin	49	// Front Left turning LED blinking -- yellow exterior - yellow wire
#define LED_FLH_pin	47	// Front Left Highbeam LED -- White interior-- white wire
#define LED_FLL_pin	45	// Front Left Lowbeam LED  -- White center -- red wire
#define LED_FRT_pin	43	// Front Right turning LED blinking -- yellow exterior - yellow wire
#define LED_FRH_pin	41	// Front Right Highbeam LED -- White interior -- white wire
#define LED_FRL_pin	39	//Front Right Lowbeam LED  -- White center -- red wire
#define LED_RRT_pin	37	// Rear Right turning LED blinking -- yellow exterior -- yellow wire
#define LED_RRR_pin	35	// Rear Right Reverse LED -- White interior -- white wire
#define LED_RRB_pin	33	// Rear Right Brake LED  -- Red center -- red wire
#define LED_RLT_pin	31	// Rear Left turning LED blinking -- yellow exterior -- yellow wire
#define LED_RLR_pin	29	// Rear Left Reverse LED -- White interior -- white wire
#define LED_RLB_pin	27	// Rear Left Brake LED  -- Red center -- red wire

// OLED Instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global speed and turn variables
int speed = 80; // speed 
int duration_ms = 570; // duration of turn
int j = 0;

// Global distance we want car to travel
float feet = 2.25;

// OLED Timer
unsigned long timerStart = 0;
bool timerRunning = false;
int timerValue = 0;   // seconds


// ARRAY OF LIGHT PINS
int LED_pins[] = {LED_FLT_pin, LED_FLH_pin, LED_FLL_pin,
                LED_FRT_pin, LED_FRH_pin, LED_FRL_pin,
                LED_RRT_pin, LED_RRR_pin, LED_RRB_pin,
                LED_RLT_pin, LED_RLR_pin, LED_RLB_pin};

//************************************************************
// Method: setup
// Input: NONE
// Description: Initialize bluetooth on Serial2
//    set pins for motors and encoders
// 
//************************************************************
void setup() {
  // Initialize OLED SCREEN
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    for(;;);
  }

  // Initialize BLUETOOTH
  Serial.begin(9600);
  Serial2.begin(38400);
  Serial.println("Serial2 initialized at 38400 baud.");

  // Turn all lights off
  for (int i = 0; i < 12; i++) {
    pinMode(LED_pins[i], OUTPUT);
    digitalWrite(LED_pins[i], LOW); // start OFF
  }

  // Finalize OLED
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10,10, SSD1306_WHITE);
  display.display();

  // Initialize Motors
    pinMode(MotorPWM_A, OUTPUT);
    pinMode(MotorPWM_B, OUTPUT);
    pinMode(INA1A, OUTPUT);
    pinMode(INA2A, OUTPUT);
    pinMode(INA1B, OUTPUT);
    pinMode(INA2B, OUTPUT);

    // Normal on lights (just lowbeams)
    digitalWrite(LED_FLL_pin, HIGH);
    digitalWrite(LED_FRL_pin, HIGH);
   
}

//************************************************************
// Method: loop
// Input: NONE
// Description: run the bluetooth_commands() function
//   to take bluetooth input whenever it is sent
// 
//************************************************************
void loop() {
  bluetooth_commands();
}


//************************************************************
// Method: bluetooth_commands()
// Input: Serial2 Bluetooth Input from Phone
// Description: Read string command sent from DSD on iPhone
//   Command list: 
//    Break - Breaks and turns on break lights
//    Forward - Turns forward and to the left four times
//************************************************************
void bluetooth_commands(){
  // If bluetooth available
  if (Serial2.available()){
    // Read string command from input 
    String cmd = Serial2.readString(); // Reads until newline character
    cmd.trim(); // Removes any trailing whitespace
    // COMMANDS
    if (cmd == "Break"){   // Breaks
      breaks();
    }
    if (cmd == "Forward"){  // Forward command for completing our specific square task
      // Start timer in the background
      startTimer();
      // For loop calls the go_forward function for each line of the square
      for(j = 0; j < 4; j++){
        go_forward(); // turn_left is embedded into the function
      }
      stopTimer();
      displayTimer();   // Shows final time
      display.display();
      delay(10000); // display final timer for at least ten seconds
      timerValue = 0;
    }
  } 
}

//************************************************************
// Method: breaks()
// Input: Serial2 Bluetooth Input from Phone
// Description: Break the car
//************************************************************
void breaks(){
  digitalWrite(LED_RRB_pin, HIGH);
  digitalWrite(LED_RLB_pin, HIGH);
  analogWrite(MotorPWM_A, 0);
  analogWrite(MotorPWM_B, 0);
}

//************************************************************
// Method: go_forward()
// Input: NONE
// Description: Travel forward however many feet are specified
// at the specified speed, then call breaks() and turn_Left() 
// functions
//************************************************************
void go_forward(){
  // On second iteration begin traveling less
  if (j == 1){
    feet = 1.8;
  }
  // Set the speed for both motors using global speed
  analogWrite(MotorPWM_A, speed);
  analogWrite(MotorPWM_B, speed);

  // Both motors move forward
  digitalWrite(INA1A, HIGH);
  digitalWrite(INA2A, LOW);
  digitalWrite(INA1B, HIGH);
  digitalWrite(INA2B, LOW);

  // Calculate time to run 
  unsigned long duration = feet * 1000;  // milliseconds per foot
  delay(duration);
  // Stop motors
  breaks();
  // Wait before turn
  delay(1000);
  // Turn break lights off
  digitalWrite(LED_RRB_pin, LOW);
  digitalWrite(LED_RLB_pin, LOW);
  // call turn_left() function 
  turn_left();

} 

//************************************************************
// Method: turn_left()
// Input: NONE
// Description: Function that turns the car to the left and also
// puts on the cars blinker 
//************************************************************
void turn_left(){
  // Set speed of the right motor a little higher than the left
  int rightspeed = 90;

  // Blinker
  digitalWrite(LED_FLT_pin, HIGH); 
  digitalWrite(LED_RLT_pin, HIGH); 
  delay(100);
  digitalWrite(LED_FLT_pin, LOW);
  digitalWrite(LED_RLT_pin, LOW); 
  delay(100);
  digitalWrite(LED_FLT_pin, HIGH);
  digitalWrite(LED_RLT_pin, HIGH); 
  delay(100);
  digitalWrite(LED_FLT_pin, LOW);
  digitalWrite(LED_RLT_pin, LOW); 

  // Left motor backward
  analogWrite(MotorPWM_A, speed);
  digitalWrite(INA1A, LOW);
  digitalWrite(INA2A, HIGH);

  // Right motor forward
  analogWrite(MotorPWM_B, rightspeed);
  digitalWrite(INA1B, HIGH);
  digitalWrite(INA2B, LOW);

  delay(duration_ms); // turn duration adjustment

  // Stop both motors
  breaks();
  delay(1000);

  // Turn OFF blinkers
  digitalWrite(LED_FLT_pin, LOW);
  digitalWrite(LED_RLT_pin, LOW);
}

//************************************************************
// Method: displayTimer()
// Input: NONE 
// Description: Display timer after car finishes its run
//************************************************************
void displayTimer() {
    // Draw big timer under first line
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 16);    // Under first line 
    display.print(timerValue);
}

//************************************************************
// Method: stopTimer()
// Input: NONE 
// Description: Stop the timer for how long it takes the car
// to finish its run
//************************************************************
void stopTimer() {
    timerRunning = false;  
    timerValue = (millis() - timerStart) / 1000;  // store elapsed seconds
}

//************************************************************
// Method: startTimer()
// Input: NONE 
// Description:  Start the timer for how long it takes the car
// to finish its run
//************************************************************
void startTimer() {
    timerStart = millis();
    timerValue = 0;
    timerRunning = true;
}

