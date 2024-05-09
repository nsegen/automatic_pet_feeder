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
  for (int i = 0; i < feederConfig.stepsBkw; i++) runMotor(-1);
  for (int i = 0; i < feederConfig.stepsFrw; i++) runMotor(1);
}

void Feeder::runMotor(short dir) {
  static byte step = 0;
  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], bitRead(steps[step & 0b11], i));
  delayMicroseconds(feederConfig.feedSpeed);
  step += dir;
}

Feeder::Feeder(const byte motorPins[4], const byte motorSteps[4]) : drvPins(motorPins), steps(motorSteps) {
  feederConfig.feedSpeed = FEED_SPEED;   // задержка между шагами мотора (мкс)
  feederConfig.feedAmount = FEED_AMOUNT;
  feederConfig.stepsFrw = STEPS_FRW;       // шаги вперёд
  feederConfig.stepsBkw = STEPS_BKW;
  shouldFeed = false;
}

short Feeder::getFeedSpeed() { return feederConfig.feedSpeed; }
void Feeder::setFeedSpeed(short speed) { 
  Serial.print("[Feeder.setFeedSpeed] ");
  Serial.println(speed);
  feederConfig.feedSpeed = speed;
}

short Feeder::getFeedAmount() { return feederConfig.feedAmount; }
void Feeder::setFeedAmount(short amount) { 
  Serial.print("[Feeder.setFeedAmount] ");
  Serial.println(amount);
  feederConfig.feedAmount = amount; 
}

short Feeder::getStepsFrw() { return feederConfig.stepsFrw; }
void Feeder::setStepsFrw(short steps) { 
  Serial.print("[Feeder.setStepsFrw] ");
  Serial.println(steps);
  feederConfig.stepsFrw = steps; 
}

short Feeder::getStepsBkw() { return feederConfig.stepsBkw; }
void Feeder::setStepsBkw(short steps) { 
  Serial.print("[Feeder.setStepsBkw] ");
  Serial.println(steps);
  feederConfig.stepsBkw = steps; 
}

void Feeder::feed() { 
  Serial.println("[Feeder.feed] Set feed flag.");
  shouldFeed = true; 
}
void Feeder::handleFeeder() {
  if (shouldFeed) {
    Serial.println("[Feeder.handleFeeder] Start feeding.");
    
    Serial.print("[Feeder.handleFeeder] feedSpeed = ");
    Serial.println(feederConfig.feedSpeed);
    
    Serial.print("[Feeder.handleFeeder] feedAmount = ");
    Serial.println(feederConfig.feedAmount);
    
    Serial.print("[Feeder.handleFeeder] stepsFrw = ");
    Serial.println(feederConfig.stepsFrw);
    
    Serial.print("[Feeder.handleFeeder] stepsBkw = ");
    Serial.println(feederConfig.stepsBkw);

    for (int i = 0; i < feederConfig.feedAmount; i++) oneRev();
    disableMotor();
    shouldFeed = false;
  }
}

void Feeder::setConfig(FeederConfig config) {
  feederConfig = config;
}

FeederConfig Feeder::getConfig() {
  return feederConfig;
}