#include "Competition.h"
#include "Sensor.h"
#include "Timer.h"
#include "Lights.h"

Sensor::Sensor(int minPassDetectionDistance, int maxPassDetectionDistance, int sensorReadingIntervalMillis, int disableSensorReadingIntervalMillis) : sensorVL53L0X_()

{
    minPassDetectionDistance_ = minPassDetectionDistance;
    maxPassDetectionDistance_ = maxPassDetectionDistance;
    sensorReadingIntervalMillis_ = sensorReadingIntervalMillis;
    disableSensorReadingIntervalMillis_ = disableSensorReadingIntervalMillis;
}

void Sensor::setCompetitionObjectPointer(Competition *pCompetition)
{
    competition_ = pCompetition;
}

void Sensor::setLightsObjectPointer(Lights *pLights)
{
    lights_ = pLights;
}

void Sensor::setup()
{
    Wire.begin();
    // Set I2C sub-device address
    sensorVL53L0X_.begin(0x50);
    // Set to Back-to-back mode and high precision mode
    sensorVL53L0X_.setMode(sensorVL53L0X_.eContinuous, sensorVL53L0X_.eHigh);
    // Laser rangefinder begins to work
    sensorVL53L0X_.start();
    passDetectionDelayEnabled_ = false;
}

void Sensor::loop()
{

    unsigned long currentMillis = millis();
    if (!passDetectionDelayEnabled_)
    {
        if (currentMillis - detectionPreviousMillis_ >= sensorReadingIntervalMillis_ && detectionEnabled_)
        {
            sensorDistanceReading_ = sensorVL53L0X_.getDistance();
            lastDistanceReading_ = sensorDistanceReading_;
            if (sensorDistanceReading_ > minPassDetectionDistance_ && sensorDistanceReading_ < maxPassDetectionDistance_)
            {
                // Remove sensor glithces (sporadic, single false positive reading in range, without real object presented in that range)
                // Skip numberOfReadingsToSkip readings - default skip 1 reading.
                // This skipps 1 reading, regardless of if it is real reading or a false positive reading...
                // Moving average or other methods for removing the glitches will make the sensor respond slower,
                // thats why in our case (of single false positive readig glitches), skipping a reading approach is taken.
                // As skipping is done every time a reading is taken, the reading of the sensor is every 25ms and
                // the comparatively slow speed of the robot, this approach should have no effect on the Total competition time recorded
                // This way the acuracy of the gate becomes about 50ms. 
                //! Our observation is, that a wall/reflective surface, placed a bit after the desired measurment
                //! range helps reduce the number of false positive readings, so put
                //! an extra wall on the opposite side of the line a bit outside the reading range chosen!
                //! (in the case of a maze, there is a wall on the opposite side anyways),
                //! 
                if (skipFalsePositiveCounter_ < numberOfReadingsToSkip)
                {
                    skipFalsePositiveCounter_++;
                }
                else
                {
                    skipFalsePositiveCounter_ = 0;
                    disabledDetecttionPreviousMillis_ = currentMillis;
                    passDetectionDelayEnabled_ = true;
                    detectionPreviousMillis_ = currentMillis;
                    // debugPrintf2("\nReading №: %i  Distance in cm: %3.2f\n",reaingsCounter_, (sensorDistanceReading_ * 0.1));
                    lastDistanceReading_ = sensorDistanceReading_;
                    reaingsCounter_++;
                    lights_->clearGreen();
                    lights_->clearYellow();
                    lights_->showRed();
                    competition_->setRobotPassDetected(currentMillis);
                }
            }
        }
    }
    else if (currentMillis - disabledDetecttionPreviousMillis_ >= disableSensorReadingIntervalMillis_ && passDetectionDelayEnabled_)
    // Pass detection is disabled in order to give the robot time to pass trough the gate
    // Detection is disabled for a preset time - disableSensorReadingIntervalMillis_
    {
        if (!competition_->getRoundFinished())
        {
            debugPrintln("Sensor::loop() disable Readings Interval passed, enabling detection!\n");
            skipFalsePositiveCounter_ = 0;
            passDetectionDelayEnabled_ = false;
            lights_->clearRed();
            lights_->showGreen();
            detectionPreviousMillis_ = currentMillis;
            // competition_->setRobotPassDelayFinished();
        }
    }
}

  void Sensor::reset(){
            skipFalsePositiveCounter_ = 0;
            lights_->clearRed();
            // lights_->showGreen();
            debugPrintln("Sensor::reset()\n");
            passDetectionDelayEnabled_ = false;
            detectionPreviousMillis_ = millis();
            // competition_->setRobotPassDelayFinished();  
}


void Sensor::enableDetection()
{
    detectionEnabled_ = true;
}

void Sensor::disableDetection()
{
    detectionEnabled_ = false;
}

void Sensor::disablePassDelay()
{
    passDetectionDelayEnabled_ = false;
}

