
#pragma once

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
    debugPrintln("debugPrintln: Serial initialized!");
#endif
  }

} // namespace serial
