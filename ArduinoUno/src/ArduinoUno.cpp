#include <Arduino.h>
#include "Payload.h"
#include <Wire.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// -- General Arduino Variables -- //

const int VRx = A0;
const int VRy = A1;
const int SW = 2;

const int ClickLED = 8;
const int UP = 11; 
const int DOWN = 10;
const int RIGHT = 6;
const int LEFT = 9;

const int maxValue = 255; // The max brightness value for the LEDs, which is 255 for 8-bit PWM
const int movingThreshold = 30; // This variables is to tell if the joystick is moving or not

//static constexpr uint8_t SOF1_VALUE = 0xFF;
//static constexpr uint8_t SOF2_VALUE = 0x55;

void setup() {
  pinMode(SW, INPUT_PULLUP); 
  pinMode(ClickLED, OUTPUT);
  pinMode(UP, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(LEFT, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
}

void loop() {

  /*// -- TESTING LEDS & ARDUINO FUNCITONALITY -- //
  digitalWrite(ClickLED, HIGH); 
  delay(1000); 
  digitalWrite(ClickLED, LOW); 
  delay(1000); */

  // Read joystick values
  int rawX = analogRead(VRx);
  int rawY = analogRead(VRy);
  int sw = digitalRead(SW);

  // Determine joystick values with LED indicators
  //bool noMovement = abs(rawX - 512) < movingThreshold && abs(rawY - 512) < movingThreshold;

  

  int mapXValue = map(abs(rawX-512), 0, 512, 0, maxValue);
  int mapYValue = map(abs(rawY-512), 0, 512, 0, maxValue);

  if (rawX > 512 + movingThreshold) {
    analogWrite(RIGHT, mapXValue);
    digitalWrite(LEFT, LOW);
  } else if (rawX < 512 - movingThreshold) {
    digitalWrite(RIGHT, LOW);
    analogWrite(LEFT, mapXValue);
  } else {
    digitalWrite(RIGHT, LOW);
    digitalWrite(LEFT, LOW);
  }
  
  if (rawY > 512 + movingThreshold) {
    analogWrite(UP, mapYValue);
    digitalWrite(DOWN, LOW);
  } else if (rawY < 512 - movingThreshold) {
    digitalWrite(UP, LOW);
    analogWrite(DOWN, mapYValue);
  } else {
    digitalWrite(UP, LOW);
    digitalWrite(DOWN, LOW);
  }

  digitalWrite(ClickLED, sw == 0 ? HIGH : LOW);

  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(rawX - 512);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("Y:");
  lcd.print(rawY - 512);
  lcd.print(" SW:");
  lcd.print(sw == 0 ? "ON    " : "OFF   ");

  // Send Payload Information
  Payload p{};

  p.x  = analogRead(VRx);
  p.y  = analogRead(VRy);
  p.sw = digitalRead(SW);
  p.crc = payload_checksum(p);
  Serial.write(reinterpret_cast<uint8_t*>(&p), sizeof(Payload));

  /**/
}
