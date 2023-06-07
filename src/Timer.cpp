#include <Arduino.h>
// #define DEBUG_MODE 1 // DEBUG_MODE should be defined here!!!
// #include "Globals.h"
#include "Timer.h"
#include "Sensor.h"
#include "Competition.h"

Timer::Timer(unsigned long timerResolutionIntervalMillis, Sensor *pSensorObj) : sensor_(pSensorObj)
{
    timerResolutionIntervalMillis_ = timerResolutionIntervalMillis;
    numLaps_ = 0;
    startTime_ = 0;
    currentLap_ = 0;
    lapStartTime_ = 0;
    timerStarted_ = false;
    sensor_ = pSensorObj;
}
Timer::~Timer()
{
}
void Timer::setSensorObjectPointer(Sensor *pSensor)
{
    sensor_ = pSensor;
}
void Timer::setCompetitionObjectPointer(Competition *pCompetition)
{
    competition_ = pCompetition;
}

void Timer::setup(int numLaps)
{
    numLaps_ = numLaps;
    currentLap_ = 1;
    elapsedTotalTime_ = 0;
    bestLapTime = 0;
    totalTime = 0;
    for (size_t i = 0; i < arraySize; i++)
    {
        currentLapTimes_[i] = 0;
    }
    timerStarted_ = false;
    sensor_->disableDetection();
}

void Timer::start(int numLaps)
{
    numLaps_ = numLaps;
    currentLap_ = 1;
    elapsedTotalTime_ = 0;
    bestLapTime = 0;
    totalTime = 0;
    for (size_t i = 0; i < arraySize; i++)
    {
        currentLapTimes_[i] = 0;
    }

    timerStarted_ = false;
    sensor_->disablePassDelay();
    sensor_->enableDetection();
}

void Timer::reset(){
    currentLap_ = 1;
    elapsedTotalTime_ = 0;
    bestLapTime = 0;
    totalTime = 0;
    for (size_t i = 0; i < arraySize; i++)
    {
        currentLapTimes_[i] = 0;
    }
    timerStarted_ = false;
    sensor_->disablePassDelay();
    sensor_->enableDetection();
}

void Timer::setPassDetected(unsigned long passTimeStampMillis)
{
    // debugPrintf1(".......Timer::setPassDetected.......received timestamp: \n", passTimeStampMillis);

    if (!timerStarted_)
    {
        //  debugPrint("Timer::setPassDetected    if (!timerStarted_) \n");
        startTime_ = passTimeStampMillis;
        lapStartTime_ = startTime_;
        timerStarted_ = true;
        debugPrintf1("Timer::setPassDetected--- NEW LAP STARTED!  ->  lapStartTime_= %lims\n", lapStartTime_);
        return;
    }

    if (currentLap_ > 0 && currentLap_ <= numLaps_)
    {
        // unsigned long currentTime = millis();
        unsigned long currentTime = passTimeStampMillis;
        unsigned long lapTime = currentTime - lapStartTime_;
        lapTimes_[currentLap_ - 1] = lapTime;
        // Print lap time and increment current lap
        debugPrintf4("Timer::setPassDetected  LAP FINISHED! ->   LAP № %i, currentTime = %li ms,  lapStartTime_ = %li ms, HumanReadableLapTime= %s\n", currentLap_, currentTime, lapStartTime_, this->getHumanReadableTime(lapTime));

        if (currentLap_ == numLaps_)
        {
            // Last lap finished, end competition
            totalTime = 0;
            bestLapTime = lapTimes_[1];
            for (int i = 0; i < numLaps_; i++)
            {
                totalTime += lapTimes_[i];
                if (lapTimes_[i] < bestLapTime)
                {
                    bestLapTime = lapTimes_[i];
                }
            }
            debugPrintf1("\nTimer::setPassDetected....COMPETITION ROUND FINISHED! --------Human Readable Total time: --------  %s\n", this->getHumanReadableTime(totalTime));
            // Reset timer
            numLaps_ = 0;
            startTime_ = 0;
            currentLap_ = 0;
            lapStartTime_ = 0;
            timerStarted_ = false;
            competition_->setRoundFinished(totalTime, bestLapTime);
        }
        else
        {
            currentLap_++;
            lapStartTime_ = passTimeStampMillis;
        }
    }
}

void Timer::loop()
{
    if (numLaps_ == 0)
    {
        return;
    }

    if (timerStarted_ == true)
    {

        {
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis_ >= timerResolutionIntervalMillis_)
            {
                currentLapTimes_[currentLap_ - 1] = currentMillis - lapStartTime_;
                elapsedTotalTime_ = currentMillis - startTime_;
                competition_->processTimerTick(currentLap_, currentLapTimes_, arraySize);
                previousMillis_ = currentMillis;
            }
        }
    }
}
void Timer::printTimeToSerial(unsigned long timeInMillis)
{
    debugPrintf1("Timer::printTimeToSerial - Human readable time is: %s", this->getHumanReadableTime(timeInMillis));
}

String Timer::getHumanReadableTime(unsigned long timeInMillis)
{
    // debugPrintf("Timer::getHumanReadableTime\n");
    String readableTime;
    unsigned long milli;
    unsigned long seconds;
    unsigned long minutes;
    unsigned long hours;
    unsigned long days;

    milli = timeInMillis;
    // Debug value chosen to be 1 hour, 1 minute, 1 second, and 1 millisecond
    //  inputMilliseconds = 3661001;
    //
    // milliseconds = currentMillis /100;
    // milliseconds = inputMilliseconds / 100;
    // millis1 = currentMillis/100;
    // seconds = inputMilliseconds / 1000;
    // minutes = seconds / 60;
    // hours = minutes / 60;
    // days = hours / 24;
    // seconds %= 60;
    // minutes %= 60;
    // hours %= 24;

    // 3600000 milliseconds in an hour
    hours = milli / 3600000;
    milli = milli - 3600000 * hours;
    // 60000 milliseconds in a minute
    minutes = milli / 60000;
    milli = milli - 60000 * minutes;
    // 1000 milliseconds in a second
    seconds = milli / 1000;
    milli = milli - 1000 * seconds;

    // if (days > 0)
    //     readableTime = String(days) + " ";

    if (hours > 0)
        readableTime += String(hours) + ":";
    if (minutes < 10)
        readableTime += "0";
    readableTime += String(minutes) + ":";
    if (seconds < 10)
        readableTime += "0";
    readableTime += String(seconds) + ".";
    if (milli < 100)
        readableTime += "0";
    if (milli < 10)
        readableTime += "0";
    readableTime += String(milli);
    return readableTime;
}
