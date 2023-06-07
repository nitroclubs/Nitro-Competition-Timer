#ifndef Timer_h
#define Timer_h
// #define DEBUG_MODE 1 // DEBUG_MODE should be defined here!!!
#include "Globals.h"

//Forward declarations:
class Sensor;
class Competition; 

class Timer {
  private:
    static const int arraySize = 2; // = MAX NUMBER OF LAPS
    
    int numLaps_=0;
    unsigned long startTime_;
    unsigned long lapTimes_[arraySize];
     unsigned long  currentLapTimes_[arraySize];
     unsigned long  elapsedTotalTime_;
    int currentLap_;
    unsigned long lapStartTime_;
    bool timerStarted_;
    unsigned long timerResolutionIntervalMillis_ = 0;
    unsigned long previousMillis_ = 0;
    unsigned long bestLapTime = 0;
    unsigned long totalTime = 0;

    Sensor *sensor_;
    Competition* competition_;


private:
    // void printTime(unsigned long time);

  public:
    // Timer(Sensor *s);
    Timer(unsigned long timerResolutionIntervalMillis, Sensor* pSensorObj);

    ~Timer();
    void setSensorObjectPointer(Sensor* pSensor);
    void setCompetitionObjectPointer(Competition* pCompetition);
    void setup(int numLaps);
    void loop();
    void start(int numLaps);
    void reset();
    void setPassDetected(unsigned long passTimeStampMillis);
    String getHumanReadableTime(unsigned long timeInMillis);
    void printTimeToSerial(unsigned long timeInMillis);

};
#endif