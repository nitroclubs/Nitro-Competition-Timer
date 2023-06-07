#pragma once
#ifndef SerialHelper_h
#define SerialHelper_h
namespace serial
{

  // Initialise serial
  void setupSerial()
  {
#if DEBUG_MODE == 1
    Serial.begin(921600);
    // wait until serial port opens for native USB devices
    while (!Serial)
    {
          delay(1);
    }
    Serial.println("debugPrintln: Serial initialized!");
#endif
  }

} // namespace serial

#endif //SerialHelper_h