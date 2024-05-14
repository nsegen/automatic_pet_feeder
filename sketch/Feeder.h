#ifndef FEEDER_H
#define FEEDER_H

#include <Arduino.h>

struct FeederConfig {
  short feedSpeed; // задержка между шагами мотора (мкс)
  short feedAmount;
  short stepsFrw; // шаги вперёд
  short stepsBkw;
};

class Feeder {
  private:
    const byte *drvPins;
    const byte *steps;
    bool shouldFeed;
    FeederConfig feederConfig;
    
    // выключаем ток на мотор
    void disableMotor();
    void oneRev();
    void runMotor(short dir);

  public:
    Feeder(const byte motorPins[4], const byte motorSteps[4]);
    
    short getFeedSpeed();
    void setFeedSpeed(short speed);

    short getFeedAmount();
    void setFeedAmount(short amount);
    
    short getStepsFrw();
    void setStepsFrw(short steps);
    
    short getStepsBkw();
    void setStepsBkw(short steps);

    void feed();
    void handleFeeder();

    void setConfig(FeederConfig config);
    FeederConfig getConfig();
};

#endif