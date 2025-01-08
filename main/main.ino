// Pin definitions
const int directionPin = 13;
const int pwmPin = 11;
const int brakePin = 8;
const int commandPin = 2;
const int positionPin1 = 9;  // Output pin for position 1
const int positionPin2 = 10; // Output pin for position 2

// Position definitions
#define POS_UNKNOWN 0
#define POS_TOKAMAK 2
#define POS_DUMP 1

// Timing constants
const unsigned long debounceDelayCommand = 10;
const unsigned long debounceDelaySwitch = 50;
const unsigned long moveTimeout = 200;
const unsigned long MOVE_TIMEOUT = 2000; // 2 seconds timeout
const bool TEST_MODE = true;  // Set to false for normal operation
const unsigned long TEST_INTERVAL = 100000;  // 100 seconds

// State variables
volatile bool positionChanged = false;
volatile bool commandReceived = false;
volatile int targetPosition = POS_UNKNOWN;
volatile unsigned long lastInterruptTime = 0;
unsigned long lastTestTime = 0;

void setMotorDirection(bool direction) {
  digitalWrite(directionPin, direction);
}

void updatePositionIndicators(int position) {
  switch(position) {
    case POS_TOKAMAK:
      digitalWrite(positionPin1, HIGH);
      digitalWrite(positionPin2, LOW);
      break;
    case POS_DUMP:
      digitalWrite(positionPin1, LOW);
      digitalWrite(positionPin2, HIGH);
      break;
    default:
      digitalWrite(positionPin1, LOW);
      digitalWrite(positionPin2, LOW);
      break;
  }
}

void setMotorBrake(bool brake) {
  digitalWrite(brakePin, brake);
}

void setMotorPower(bool on) {
  digitalWrite(pwmPin, on);
  if (!on) {
    setMotorBrake(true);  // Enable brake when stopping
  }
}

void moveToPosition(int target) {
  int current = getPosition();
  updatePositionIndicators(current);
  if (current == target) return;
  
  unsigned long startTime = millis();
  bool success = false;
  
  setMotorBrake(false);
  setMotorDirection(target == POS_DUMP);
  
  while ((millis() - startTime) < MOVE_TIMEOUT) {
    setMotorPower(true);
    delay(moveTimeout);  // Short movement attempt
    current = getPosition();
    updatePositionIndicators(current); // Add after movement

    if (current == target) {
      success = true;
      break;
    }
    
    setMotorPower(false);
    delay(100);  // Brief pause between attempts
  }
  
  setMotorPower(false);  // Ensure motor is off
  
  if (!success) {
    Serial.println("Movement timeout - failed to reach target position");
  }
  updatePositionIndicators(getPosition());
}

void commandISR() {
  static unsigned long lastCmdTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastCmdTime > debounceDelayCommand) {
    commandReceived = true;
    // Set position based on pin state
    targetPosition = digitalRead(commandPin) ? POS_TOKAMAK : POS_DUMP;
    lastCmdTime = currentTime;
  }
}

void pinChangeISR() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelaySwitch) {
    positionChanged = true;
    lastInterruptTime = interruptTime;
  }
}

int getPosition() {
  int sensor1 = digitalRead(A0);
  int sensor2 = digitalRead(A1);
  
  if (sensor1 == 1 && sensor2 == 0) {
    return POS_TOKAMAK;
  } else if (sensor1 == 0 && sensor2 == 1) {
    return POS_DUMP;
  }
  return POS_UNKNOWN; // Unknown position
}

void printPosition() {
  int pos = getPosition();
  Serial.print("Current position: ");
  
  switch(pos) {
    case POS_TOKAMAK:
      Serial.println("TOKAMAK");
      break;
    case POS_DUMP:
      Serial.println("DUMP");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }
}

void setup() {
  //define pins
  pinMode(directionPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(commandPin, INPUT_PULLUP);  // Set command pin as input with pull-up resistor
  pinMode(positionPin1, OUTPUT);
  pinMode(positionPin2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(commandPin), commandISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(A0), pinChangeISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(A1), pinChangeISR, CHANGE);
  Serial.begin(115200);
  updatePositionIndicators(getPosition()); // Initialize position indicators
}

void loop() {
  if (TEST_MODE) {
    unsigned long currentTime = millis();
    if (currentTime - lastTestTime >= TEST_INTERVAL) {
      targetPosition = (targetPosition == POS_TOKAMAK) ? POS_DUMP : POS_TOKAMAK;
      commandReceived = true;
      Serial.println("\n--- TEST MODE ---");
      lastTestTime = currentTime;
    }
  }

  if (positionChanged) {
    printPosition();
    positionChanged = false;
  }
  
  if (commandReceived) {
    Serial.print("Moving to target position: ");
    Serial.println(targetPosition == POS_TOKAMAK ? "TOKAMAK" : "DUMP");
    moveToPosition(targetPosition);
    commandReceived = false;
  }
  
  delay(100); // Short delay for loop stability
}