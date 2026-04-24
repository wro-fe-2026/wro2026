// WRO 2026 Future Engineers - CONALEP Plantel 133 Chilapa de Alvarez
// Open Challenge - Preliminary code v1.0
// Team: Josue Cervantes, Uriel Silva, Eduardo Martinez
// Controller: Arduino Nano V3.0
// Note: This is a preliminary version. Modules may be refactored in future commits.

#include "Adafruit_VL53L0X.h"
#include <Servo.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Servo myServo;

// --- PIN DEFINITIONS ---
const int startSensorPin = 7;
const int motorIN1 = 3;
const int motorIN2 = 4;
const int servoPin = 9;

// --- VARIABLES ---
bool programRunning = false;
const int motorSpeed = 120;
bool previousButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(startSensorPin, INPUT_PULLUP);
  pinMode(motorIN1, OUTPUT);
  pinMode(motorIN2, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(90); // Center position at start
  previousButtonState = digitalRead(startSensorPin);
  if (!lox.begin()) {
    Serial.println("VL53L0X Error");
    while (1);
  }
  Serial.println("Waiting for activation on Pin 7...");
}

void loop() {
  // --- TOGGLE BUTTON (ON/OFF) ---
  bool currentButtonState = digitalRead(startSensorPin);

  if (currentButtonState == LOW && previousButtonState == HIGH) {
    programRunning = !programRunning;
    if (programRunning) {
      Serial.println("Robot ACTIVATED!");
    } else {
      Serial.println("Robot OFF");
      // Turn off motor and center servo when deactivated
      digitalWrite(motorIN1, LOW);
      digitalWrite(motorIN2, LOW);
      myServo.write(90);
    }
    delay(300); // Debounce
  }
  previousButtonState = currentButtonState; // Always update

  // --- MAIN PROGRAM ---
  if (programRunning) {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    // Motor forward
    analogWrite(motorIN1, motorSpeed);
    digitalWrite(motorIN2, LOW);

    if (measure.RangeStatus != 4) {
      int distance = measure.RangeMilliMeter / 10; // mm to cm
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      // Map distance to servo degrees
      // Range: 5cm (very close) to 50cm (far)
      // Close = 0 degrees, Far = 180 degrees
      int servoDegrees = map(distance, 5, 50, 0, 180);
      servoDegrees = constrain(servoDegrees, 0, 180);
      myServo.write(servoDegrees);

      Serial.print("Servo: ");
      Serial.print(servoDegrees);
      Serial.println(" degrees");
    }
  } else {
    // Robot off: motors stopped
    digitalWrite(motorIN1, LOW);
    digitalWrite(motorIN2, LOW);
  }

  delay(50);
}
