#define FEED_SPEED 3000   // задержка между шагами мотора (мкс)
#define STEPS_FRW 19        // шаги вперёд
#define STEPS_BKW 12        // шаги назад#include <ESP8266WiFi.h>
#define FEED_AMOUNT 1000;
#include "Feeder.h"

void Feeder::disableMotor() {
  Serial.println("[Feeder.disableMotor]");
  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], 0);
}

void Feeder::oneRev() {
  Serial.println("[Feeder.oneRev]");
  for (int i = 0; i < stepsBkw; i++) runMotor(-1);
  for (int i = 0; i < stepsFrw; i++) runMotor(1);
}

void Feeder::runMotor(short dir) {
  static byte step = 0;
  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], bitRead(steps[step & 0b11], i));
  delayMicroseconds(feedSpeed);
  step += dir;
}

Feeder::Feeder(const byte motorPins[4], const byte motorSteps[4]) : drvPins(motorPins), steps(motorSteps) {
  feedSpeed = FEED_SPEED;   // задержка между шагами мотора (мкс)
  feedAmount = FEED_AMOUNT;
  stepsFrw = STEPS_FRW;       // шаги вперёд
  stepsBkw = STEPS_BKW;
  shouldFeed = false;
}

short Feeder::getFeedSpeed() { return feedSpeed; }
void Feeder::setFeedSpeed(short speed) { 
  Serial.print("[Feeder.setFeedSpeed] ");
  Serial.println(speed);
  feedSpeed = speed;
}

short Feeder::getFeedAmount() { return feedAmount; }
void Feeder::setFeedAmount(short amount) { 
  Serial.print("[Feeder.setFeedAmount] ");
  Serial.println(amount);
  feedAmount = amount; 
}

short Feeder::getStepsFrw() { return stepsFrw; }
void Feeder::setStepsFrw(short steps) { 
  Serial.print("[Feeder.setStepsFrw] ");
  Serial.println(steps);
  stepsFrw = steps; 
}

short Feeder::getStepsBkw() { return stepsBkw; }
void Feeder::setStepsBkw(short steps) { 
  Serial.print("[Feeder.setStepsBkw] ");
  Serial.println(steps);
  stepsBkw = steps; 
}

void Feeder::feed() { 
  Serial.println("[Feeder.feed] Set feed flag.");
  shouldFeed = true; 
}
void Feeder::handleFeeder() {
  if (shouldFeed) {
    Serial.println("[Feeder.handleFeeder] Start feeding.");
    
    Serial.print("[Feeder.handleFeeder] feedSpeed = ");
    Serial.println(feedSpeed);
    
    Serial.print("[Feeder.handleFeeder] feedAmount = ");
    Serial.println(feedAmount);
    
    Serial.print("[Feeder.handleFeeder] stepsFrw = ");
    Serial.println(stepsFrw);
    
    Serial.print("[Feeder.handleFeeder] stepsBkw = ");
    Serial.println(stepsBkw);

    for (int i = 0; i < feedAmount; i++) oneRev();
    disableMotor();
    shouldFeed = false;
  }
}