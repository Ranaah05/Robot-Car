//************************************************************
// Lights
// File: Project_1.ino
// Purpose: To learn how to program OLED display using Adafruit library
// Also learn how to use bluetooth
// Group 5: Elena Raner Audrey
//************************************************************

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

int LED_pins[] = {LED_FLT_pin, LED_FLH_pin, LED_FLL_pin,
                LED_FRT_pin, LED_FRH_pin, LED_FRL_pin,
                LED_RRT_pin, LED_RRR_pin, LED_RRB_pin,
                LED_RLT_pin, LED_RLR_pin, LED_RLB_pin};

void setup() {
  Serial.begin(9600);

  Serial2.begin(38400);
  Serial.println("Serial2 initialized at 38400 baud.");

  for (int i = 0; i < 12; i++) {
    pinMode(LED_pins[i], OUTPUT);
    digitalWrite(LED_pins[i], LOW); // start OFF
  }

}

void loop() {
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
      break_lights();
    }
    // Reverse lights
    if (cmd == "REVERSE"){
      reverse_lights();
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
// Break function
void break_lights(){
  digitalWrite(LED_RRB_pin, HIGH);
  digitalWrite(LED_RLB_pin, HIGH);
}
// Reverse function
void reverse_lights(){
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