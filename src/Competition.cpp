#include <Arduino.h>
#include "Competition.h"
#include "Sensor.h"
#include "Timer.h"
#include "Lights.h"
#include <ArduinoJson.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>

Competition::Competition(Sensor *sensorObj, Timer *timerObj, Lights *lightsObj) : sensor_(sensorObj), timer_(timerObj), lights_(lightsObj)
{
}

Competition::~Competition()
{
}

void Competition::setup()
{
  debugPrintf("...............Competition::setup()...............\n");
  lights_->showGreen();
  lights_->showYellow();
  lights_->showRed();
}
void Competition::startNewRound()
{
  debugPrintf("\nCompetition::start().......NEW COMPETITION ROUND STARTED........\n");
  roundFinished_ = false;
  debugPrintf1("Competition::start().......NUMBER OF LAPS: %i\n", numberOfLaps_);
  lights_->clearAll();
  lights_->showGreen();
  timer_->start(numberOfLaps_);
   sensor_->enableDetection();
}
void Competition::cancelRound()
{
  sensor_->reset();
  timer_->reset();
  String jsonString = ""; // create a JSON string for sending data to the client
  StaticJsonDocument<256> doc;
  doc["command"] = "ShowFinalResult";
  doc["numberOfLaps"] = numberOfLaps_;
  doc["bestLapTime"] = "99:99:999";
  doc["totalFinalTime"] = "99:99:999";
  serializeJson(doc, jsonString);
  // debugPrintln(jsonString);      // print JSON string to console for debugprt purposes (you can comment this out)
  ws_->broadcastTXT(jsonString); // send JSON string to clients

  // debugPrint("...............Competition::cancelRound...............\n");
  lights_->showGreen();
  lights_->showYellow();
  lights_->showRed();

}
void Competition::setNumberOfLaps(int numLaps)
{
  debugPrint("...............Competition::setNumberOfLaps(int numLaps)...............\n");
  numberOfLaps_ = numLaps;
}

void Competition::setRoundFinished(unsigned long totalTime, unsigned long bestLapTime)
{
  roundFinished_ = true;
  String jsonString = ""; // create a JSON string for sending data to the client
  StaticJsonDocument<256> doc;
  doc["command"] = "ShowFinalResult";
  doc["numberOfLaps"] = numberOfLaps_;
  doc["currentLapNumber"] = 0;//1;
  // doc["lap1currentTime"] = "";//"01:01:01:001";
  // doc["lap2currentTime"] = "";//"02:02:02:002";
  // doc["totalElapsedTime"] = "";//"11:11:11:110";
  doc["bestLapTime"] =  timer_->getHumanReadableTime(bestLapTime);//"11:11:11:110";
  doc["totalFinalTime"] = timer_->getHumanReadableTime(totalTime);//"99:99:99:999";
  serializeJson(doc, jsonString);
  debugPrintln(jsonString);      // print JSON string to console for debugprt purposes (you can comment this out)
  ws_->broadcastTXT(jsonString); // send JSON string to clients
  lights_->showGreen();
  lights_->showYellow();
  lights_->showRed();
  // debugPrint("...............Competition::setRoundFinished...............\n");
}

bool Competition::getRoundFinished()
{
  return roundFinished_;
}

void Competition::processTimerTick(int currentLapNumber, unsigned long currentLapTimes[], int lapTimesArraySize)
{
  // debugPrintf3("Competition::processTimerTick.... Current LAP: %i Elapsed Lap Time: %s Elapsed Total time: %s)\n", currentLapNumber, timer_->getHumanReadableTime(currentLapTimes[currentLapNumber - 1]), timer_->getHumanReadableTime(elapsedTime));
  // debugPrintf1("Timer::printTimeToSerial - Human readable time is: %s\n", timer_->getHumanReadableTime(timeInMillis));
  String jsonString = ""; // create a JSON string for sending data to the client
  StaticJsonDocument<256> doc;
  doc["command"] = "UpdateLapCounters";
  doc["numberOfLaps"] = numberOfLaps_;
  doc["currentLapNumber"] =currentLapNumber;
  doc["lap1currentTime"] = timer_->getHumanReadableTime(currentLapTimes[0]);//"01:01:01:001";
  doc["lap2currentTime"] = timer_->getHumanReadableTime(currentLapTimes[1]);//"02:02:02:002";
              totalElapsedTime_ = 0;
            for (int i = 0; i < numberOfLaps_; i++)
            {
                totalElapsedTime_ += currentLapTimes[i];
            }
  // debugPrintf1("Competition::processTimerTick - Human readable  totalElapsedTime_ is: %s\n", timer_->getHumanReadableTime(totalElapsedTime_));
  doc["totalElapsedTime"] = timer_->getHumanReadableTime(totalElapsedTime_); //"11:11:11:110"; //elapsedTime
  // doc["bestLapTime"] = "11:11:11:110";
  // doc["totalFinalTime"] = "99:99:99:999";

  serializeJson(doc, jsonString);
  // debugPrintln(jsonString);      // print JSON string to console for debugprt purposes (you can comment this out)
  ws_->broadcastTXT(jsonString); // send JSON string to clients
}

void Competition::wsEventCallback(byte num, WStype_t type, uint8_t *payload, size_t length)
{ // WebSocket lib: the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload

  debugPrintf("Competition::wsEventCallback !!!!\n");

  switch (type)
  {                         // switch on the type of information sent
  case WStype_DISCONNECTED: // if a client is disconnected, then type == WStype_DISCONNECTED
    debugPrintln("Client " + String(num) + " disconnected");
    break;
  case WStype_CONNECTED: // if a client is connected, then type == WStype_CONNECTED
    debugPrintln("Client " + String(num) + " connected");
    // optionally you can add code here what to do when connected
    break;
  case WStype_TEXT: // if a client has sent data, then type == WStype_TEXT
    // try to decode the JSON string received
    StaticJsonDocument<200> doc; // create a JSON container
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      debugPrintf1("deserializeJson() failed: %s\n", error.f_str());
      return;
    }
    else
    {
      //! NITRO COMPETITION TIMER SIMPLE JSON COMMUNICATION PROTOCOL:
      //        Client side:
      // actionRequest: 1 //'StartNewRound'
      // actionRequest: 2 //'CancelRound'
      // actionRequest: 3 //'SetCompetitionLine'
      // actionRequest: 4 //'SetCompetitionObstacleLine'
      // actionRequest: 6 //'SetCompetitionTeleoperation'
      // actionRequest: 5 //'SetCompetitionMaze'
      //
      // END JSON COMMUNICATION PROTOCOL ------------------

      int command = doc["actionRequest"];

      // debugPrintln("Received command from user: " + String(num));
      // debugPrintln("Command: " + String(jsonCommand));

      debugPrintf2("Received command from user: %s  Command: %s\n", String(num), String(command));
      switch (command)
      {
      case 1: //'StartNewRound'
        debugPrintf("switch (command) //'StartNewRound'\n");
        this->startNewRound();
        break;
      case 2: //'CancelRound'
        debugPrintf("switch (command) //'CancelRound'\n");
        this->cancelRound();
        break;
      case 3: //'SetCompetitionLine'
        debugPrintf("switch (command) //'SetCompetitionLine'\n");
        this->setNumberOfLaps(2);

        break;
      case 4: //'SetCompetitionObstacleLine'
        debugPrintf("switch (command) //'SetCompetitionObstacleLine'\n");
        this->setNumberOfLaps(2);
        break;
      case 5: //'SetCompetitionMaze'
        debugPrintf("switch (command) //'SetCompetitionMaze'\n");
        this->setNumberOfLaps(1);
        break;
      case 6: //'SetCompetitionTeleoperation'
        debugPrintf("switch (command) ///'SetCompetitionTeleoperation'\n");
        this->setNumberOfLaps(1);
        break;

      default:
        // if nothing else matches, do the default
        // default is optional
        break;
      }
    }
  }
}
void Competition::setWebSocketsServersObjectPointer(WebSocketsServer *pWebSocketsSrv)
{
  ws_ = pWebSocketsSrv;
}

void Competition::setRobotPassDetected(unsigned long timeStampMillis)
{
  // debugPrintf("...............Competition::setRobotPassDetected().......\n");
  timer_->setPassDetected(timeStampMillis);
}

