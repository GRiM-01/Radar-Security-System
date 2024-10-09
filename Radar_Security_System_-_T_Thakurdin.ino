#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pins for the first security system
#define RCWLPin1 2
#define PIRPin1 3
#define LEDPin1 4

// Define pins for the second security system
#define RCWLPin2 5
#define PIRPin2 6
#define LEDPin2 7

// Define pins for the third security system
#define RCWLPin3 8
#define PIRPin3 9
#define LEDPin3 10

// Define the pin for the alarm buzzer
#define BuzzerPin 11

// Flags to track system and threat states
bool alarmActive = false;
bool systemArmed1 = false;
bool systemArmed2 = false;
bool systemArmed3 = false;
bool threatDetected1 = false;
bool threatDetected2 = false;
bool threatDetected3 = false;

// Record the start time of the alarm
unsigned long alarmStartTime = 0;

// Variable to store the current message displayed on the LCD
String currentDisplay = "";

void setup() {
  // Set input/output modes for pins of the first security system
  pinMode(RCWLPin1, INPUT);
  pinMode(PIRPin1, INPUT);
  pinMode(LEDPin1, OUTPUT);

  // Set input/output modes for pins of the second security system
  pinMode(RCWLPin2, INPUT);
  pinMode(PIRPin2, INPUT);
  pinMode(LEDPin2, OUTPUT);

  // Set input/output modes for pins of the third security system
  pinMode(RCWLPin3, INPUT);
  pinMode(PIRPin3, INPUT);
  pinMode(LEDPin3, OUTPUT);

  // Set pin mode for the alarm buzzer
  pinMode(BuzzerPin, OUTPUT);

  // Initialize and configure the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please Arm");
  lcd.setCursor(0, 1);
  lcd.print("System");

  // Start serial communication
  Serial.begin(9600);
}

// Function to produce an alternating sound for the alarm
void alarmSound() {
  static bool isHighTone = false;
  static unsigned long lastChangeMillis = 0;

  if (millis() - lastChangeMillis >= 500) {
    int frequency = 1000;  //frequencies for a lower pitch

    if (isHighTone) {
      frequency = 800;
      delay(50);  // delay after high tone
    }

    tone(BuzzerPin, frequency, 500);  // alarm duration

    isHighTone = !isHighTone;
    lastChangeMillis = millis();
  }
}

// Function to stop the alarm sound
void stopAlarmSound() {
  noTone(BuzzerPin);
}

void loop() {
  // Check for incoming commands through serial communication
  if (Serial.available() > 0) {
    char command = Serial.read();

    // Process the received command and update system states accordingly
    if (command == '1' && !systemArmed1) {
      systemArmed1 = true;
      threatDetected1 = false;  // Clear threat detection when arming
      updateDisplay("System 1 Armed");
    } else if (command == '2' && !systemArmed2) {
      systemArmed2 = true;
      threatDetected2 = false;  // Clear threat detection when arming
      updateDisplay("System 2 Armed");
    } else if (command == '3' && !systemArmed3) {
      systemArmed3 = true;
      threatDetected3 = false;  // Clear threat detection when arming
      updateDisplay("System 3 Armed");
    } else if (command == '4' && systemArmed1) {
      systemArmed1 = false;
      threatDetected1 = false;  // Clear threat detection when disarming
      digitalWrite(LEDPin1, LOW);
      lcd.clear();
      lcd.print("System 1 Unarmed");
      stopAlarmSound();
    } else if (command == '5' && systemArmed2) {
      systemArmed2 = false;
      threatDetected2 = false;  // Clear threat detection when disarming
      digitalWrite(LEDPin2, LOW);
      lcd.clear();
      lcd.print("System 2 Unarmed");
      stopAlarmSound();
    } else if (command == '6' && systemArmed3) {
      systemArmed3 = false;
      threatDetected3 = false;  // Clear threat detection when disarming
      digitalWrite(LEDPin3, LOW);
      lcd.clear();
      lcd.print("System 3 Unarmed");
      stopAlarmSound();
    }
  }

  // Monitor each security system's area for potential threats
  monitorArea(RCWLPin1, PIRPin1, LEDPin1, systemArmed1, threatDetected1);
  monitorArea(RCWLPin2, PIRPin2, LEDPin2, systemArmed2, threatDetected2);
  monitorArea(RCWLPin3, PIRPin3, LEDPin3, systemArmed3, threatDetected3);

  // short delay to balance system speed and control
  delay(10); 
}

// Function to update the LCD display with a new message
void updateDisplay(String message) {
  // Only update the display if the message has changed
  if (currentDisplay != message) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    currentDisplay = message;
  }
}

// Function to monitor the area for potential threats
void monitorArea(int rcwlPin, int pirPin, int ledPin, bool systemArmed, bool &threatDetected) {
  if (systemArmed) {
    // Read the status of the RCWL and PIR sensors
    bool RCWL_detection = digitalRead(rcwlPin);
    bool PIR_detection = digitalRead(pirPin);

    // Trigger the alarm when both RCWL and PIR detect motion
    if (RCWL_detection == HIGH && PIR_detection == HIGH) {
      threatDetected = true;

      // Update the display with information about breached areas
      updateDisplay("Breached Area(s):");

      lcd.setCursor(0, 1);

      // Indicate which security systems detected a threat
      if (threatDetected1) {
        lcd.print(" 1 ");
      }

      if (threatDetected2) {
        lcd.print(" 2 ");
      }

      if (threatDetected3) {
        lcd.print(" 3 ");
      }

      // Activate the LED and sound the alarm
      digitalWrite(ledPin, HIGH);
      alarmSound();
    } else if (threatDetected) {
      // Reset the alarm state if no motion detected after an intruder was detected
      threatDetected = false;
      digitalWrite(ledPin, LOW);
      stopAlarmSound();

      // Clear the display if no threat detected
      updateDisplay("");
    }
  }
}
