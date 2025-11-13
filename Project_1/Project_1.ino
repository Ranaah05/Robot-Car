//************************************************************
// Lights
// File: Project_1.ino
// Purpose: To learn how to program OLED display using Adafruit library
// Also learn how to use bluetooth
// Group 5: Elena Raner Audrey
//************************************************************

// PRIORITY LIST:
// 1. STRAIGHT 3 FT IS IT A STRAIGHT LINE?
// 2. TURN 90 DEGREES WHEN ARE YOU GOING TO TURN HOW MANY DEGREES
//  BUTTONS FOR DEGREES AND SPEED
// oled DISPLAYING SPEED AND DEGREES AND TIMER 
// SWITCH CHANGING BUTTONS FROM SPEED INCREMEMNTS TO DEGREES

#define MotorPWM_A 4
#define MotorPWM_B 5


#define INA1A 32
#define INA2A 34
// Right Motor Rotation pins
#define INA1B 30
#define INA2B 36

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED DEFIINITIONS
#define SCREEN_WIDTH 128 // Width in px
#define SCREEN_HEIGHT 64 // Height in px
#define LOGO_WIDTH  32
#define LOGO_HEIGHT 32
#define OLED_RESET -1    
#define SCREEN_ADDRESS 0x3C // THIS COULD BE 0X3d

// CAR LIGHT DEFINITIONS
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

#define MODE_SWITCH A0 // Toggle turn degrees or speed for button incrementing

// OLED INSTANCE
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int speed = 130;
int duration_ms = 3000;

int feet = 3;

// ARRAY OF LIGHT PINS
int LED_pins[] = {LED_FLT_pin, LED_FLH_pin, LED_FLL_pin,
                LED_FRT_pin, LED_FRH_pin, LED_FRL_pin,
                LED_RRT_pin, LED_RRR_pin, LED_RRB_pin,
                LED_RLT_pin, LED_RLR_pin, LED_RLB_pin};

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
  pinMode(MODE_SWITCH, INPUT_PULLUP);

    pinMode(MotorPWM_A, OUTPUT);
    pinMode(MotorPWM_B, OUTPUT);
    pinMode(INA1A, OUTPUT);
    pinMode(INA2A, OUTPUT);
    pinMode(INA1B, OUTPUT);
    pinMode(INA2B, OUTPUT);



}

void loop() {
  // Display Speed and Degrees TO:DO TEST AND SEE IF THIS WORKS FOR THE OLED..
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.setCursor(0, 0);
  if (digitalRead(MODE_SWITCH) == LOW) { // Switch is ON 
    String text = "Speed: " + String(speed);
    display.println(text);
  } 
  else { // Switch is OFF
    String text = "Turn duration: " + String(duration_ms);
    display.println(text);
  }
  display.display();


// IF BLUETOOTH AVAILABLE
// TO:DO PUT THIS IN A FUNCTION CALLED BLUETOOTH... MAYBE THIS IS TRIGGERED BY START.
  if (Serial2.available()){
    // STRING COMMAND FROM INPUT
    String cmd = Serial2.readStringUntil('\n'); // READS UNTIL NEWLINE CHARACTER
    cmd.trim(); // Removes any trailing whitespace

    // Commands
    // Turn all lights on
    if (cmd == "STARTUP"){
      on_lights();
    }
    // Shut all lights off
    if (cmd == "SHUTOFF"){
      off_lights();
    }
    // Break lights
    if (cmd == "BREAK"){
      breaks();
    }
    // Reverse lights
    if (cmd == "REVERSE"){
      reverse();
    }
    if (cmd == "Foward"){
      go_forward();
    }
  }
}
// Shuts all lights off
void off_lights(){
   for (int i = 0; i < 12; i++) {
      pinMode(LED_pins[i], OUTPUT);
      digitalWrite(LED_pins[i], LOW); 
  }
}

// Breaks function
void breaks(){
  digitalWrite(LED_RRB_pin, HIGH);
  digitalWrite(LED_RLB_pin, HIGH);
  analogWrite(MotorPWM_A, 0);
  analogWrite(MotorPWM_B, 0);
}

// Reverse function
void reverse(){
  digitalWrite(LED_RRR_pin, HIGH);
  digitalWrite(LED_RLR_pin, HIGH);
}
// Normal on lights
void on_lights(){
  digitalWrite(LED_FLH_pin, HIGH);
  digitalWrite(LED_FRH_pin, HIGH);
  digitalWrite(LED_FRL_pin, HIGH);
  digitalWrite(LED_FLL_pin, HIGH);
}

void go_forward(){
  speed = 100;  // Adjust this value (0-255) for desired speed
  analogWrite(MotorPWM_A, speed);
  analogWrite(MotorPWM_B, speed);

  // Both motors forward
  digitalWrite(INA1A, HIGH);
  digitalWrite(INA2A, LOW);
  digitalWrite(INA1B, HIGH);
  digitalWrite(INA2B, LOW);

  // Calculate time to run (adjust 1000 ms/ft based on testing)
  unsigned long duration = feet * 1000;  // milliseconds per foot
  delay(duration);
  // Stop motors
  breaks();

} // SPEED AND LENGTH

// Turn left 90 degrees in place
void TurnLeft(){
  // Turn ON left blinkers
  digitalWrite(LED_FLT_pin, HIGH);
  digitalWrite(LED_RLT_pin, HIGH);

  // Left motor backward
  analogWrite(MotorPWM_A, speed);
  digitalWrite(INA1A, LOW);
  digitalWrite(INA2A, HIGH);

  // Right motor forward
  analogWrite(MotorPWM_B, speed);
  digitalWrite(INA1B, HIGH);
  digitalWrite(INA2B, LOW);

  delay(duration_ms); // adjust for ~90°

  // Stop both motors
  breaks();

  // Turn OFF blinkers
  digitalWrite(LED_FLT_pin, LOW);
  digitalWrite(LED_RLT_pin, LOW);
}