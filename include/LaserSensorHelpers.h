// #pragma once

// #include "globals.h"
// #include "serialHelpers.h"
// #include "Adafruit_VL53L0X.h"

// namespace laserSensor {
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// void setupVL53L0X() {
//   // debugPrint("Adafruit VL53L0X test.");
//   if (!lox.begin()) {
//     debugPrint(F("Failed to boot VL53L0X"));
//     while (1)
//       ;
//   }
//   // start continuous ranging
//   lox.startRangeContinuous();
// }
// void checkDistance() {
//   if (lox.isRangeComplete()) {
//     // debugPrint("Distance in mm: ");
//     sensorDistanceReading = lox.readRange();
//     if (sensorDistanceReading < outOfRangeMinDistance || sensorDistanceReading > outOfRangeMaxDistance) {
//       newSensorReadingAvailable = false;
//     } else {
//       // debugPrint(" reading mm: ");
//       // debugPrint(sensorDistanceReading);
//       newSensorReadingAvailable = true;
//     }
//   } else {
//     newSensorReadingAvailable = false;
//   }

//   if (newSensorReadingAvailable) {

//     RunningAverageBuffer[NextRunningAverage++] = sensorDistanceReading;
//     if (NextRunningAverage >= RunningAverageCount) {
//       NextRunningAverage = 0;
//     }
//     float RunningAverageDistance = 0;
//     for (int i = 0; i < RunningAverageCount; ++i) {
//       RunningAverageDistance += RunningAverageBuffer[i];
//     }
//     RunningAverageDistance /= RunningAverageCount;
//     debugPrint("Distance in mm: ");
//     debugPrint(RunningAverageDistance);
//   }
// }

// } //namespace sensor
