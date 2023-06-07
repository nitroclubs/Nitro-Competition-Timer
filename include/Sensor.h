#pragma once
#ifndef Sensor_h
#define Sensor_h
// #define DEBUG_MODE 1 // DEBUG_MODE should be defined here!!!
#include "Globals.h"
#include "DFRobot_VL53L0X.h"

// Forward declarations:
class Timer;
class Competition;
class Lights;
class DFRobot_VL53L0X;

class Sensor
{
protected:
private:
  DFRobot_VL53L0X sensorVL53L0X_;

  bool detectionEnabled_ = false;
  bool passDetectionDelayEnabled_ = false;
  int minPassDetectionDistance_;
  int maxPassDetectionDistance_;
  int sensorDistanceReading_;
  int lastDistanceReading_=0;
  int reaingsCounter_ = 0;
  int numberOfReadingsToSkip = 1;
  int skipFalsePositiveCounter_ = 0;
  int sensorReadingIntervalMillis_;
  unsigned long detectionPreviousMillis_ = 0;
  int disableSensorReadingIntervalMillis_;
  unsigned long disabledDetecttionPreviousMillis_ = 0;

  Timer *timer_;
  Competition *competition_;
  Lights *lights_;

public:
  Sensor(int minPassDetectionDistance, int maxPassDetectionDistance, int sensorReadingIntervalMillis, int disableSensorReadingsIntervalMillis);
  void setLightsObjectPointer(Lights *pLights);
  void setCompetitionObjectPointer(Competition *pCompetition);
  void setup();
  void loop();
  void reset();
  void enableDetection();
  void disableDetection();
  void disablePassDelay();
};

#endif // Sensor_h