#pragma once
#ifndef Competition_h
#define Competition_h
// #define DEBUG_MODE 1 // DEBUG_MODE should be defined here!!!
#include "Globals.h"
#include <ArduinoJson.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>

//Forward declarations:
class Sensor;
class Timer;
class Lights;
class WebSocketsServer;
class Competition
{
private:
  Sensor* sensor_;
  Timer* timer_;
  Lights* lights_;
  WebSocketsServer* ws_; 

unsigned long totalElapsedTime_=0;
bool roundFinished_ = false;
int numberOfLaps_=2;
int TEMPCOUNTER = 0;

public:
  Competition(Sensor* sensorObj, Timer* timerObj, Lights* lightsObj);
  Competition();
  ~Competition();
  void setup();
  void startNewRound();
  void cancelRound();
  void setNumberOfLaps(int numLaps);
  void setRoundFinished(unsigned long totalTime, unsigned long bestLapTime);
  bool getRoundFinished();
  void processTimerTick(int currentLapNumber, unsigned long currentLapTimes[], int lapTimesArraySize);
  void wsEventCallback(byte num, WStype_t type, uint8_t *payload, size_t length);
  void setWebSocketsServersObjectPointer(WebSocketsServer* pWebSocketsSrv);
  void setRobotPassDetected(unsigned long timeStampMillis);
};

#endif //Competition_h