#include <Arduino.h>
#include "Payload.h"

// -- General Arduino Variables -- //

const int VRx = A0;
const int VRy = A1;
const int SW = 2;

const int BestArduinoPinLED = 13;
const int ClickLED = BestArduinoPinLED;
const int NoMovementLED = 12;

const int movingThreshold = 10; // This variables is to tell if the joystick is moving or not

void setup() {
  pinMode(SW, INPUT_PULLUP); 
  pinMode(ClickLED, OUTPUT);
  pinMode(NoMovementLED, OUTPUT);

  Serial.begin(57600);
}

void loop() {
  // Read joystick values
  int rawX = analogRead(VRx);
  int rawY = analogRead(VRy);
  int sw = digitalRead(SW);

  // Determine joystick values with LED indicators
  bool noMovement = abs(rawX - 512) < movingThreshold && abs(rawY - 512) < movingThreshold;
  digitalWrite(NoMovementLED, noMovement && millis() % 1000 < 100 ? HIGH : LOW);
  digitalWrite(ClickLED, sw == 0 ? HIGH : LOW);

  // Send Payload Information
  Payload p;
  p.x  = analogRead(VRx);
  p.y  = analogRead(VRy);
  p.sw = digitalRead(SW);
  p.crc = payload_checksum(p);
  Serial.write(reinterpret_cast<uint8_t*>(&p), sizeof(Payload));

  /*char buffer[50];
  snprintf(buffer, sizeof(buffer), "X: %d, Y: %d, SW: %d\n", rawX, rawY, sw);
  Serial.write(buffer);*/

}
