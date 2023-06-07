#pragma once
#ifndef Lights_h
#define Lights_h


class Lights
{
private:
    const int ledPinRed_ = 14;
    const int ledPinYellow_ = 12;
    const int ledPinGreen_ = 13;
public:
    Lights(/* args */);
    ~Lights();
    void setup();
    void clearAll();
    void showRed();
    void showYellow();
    void showGreen();
    void clearRed();
    void clearYellow();
    void clearGreen();
    void toggleRed();
    void toggleYellow();
    void toggleGreen();

};


#endif //Lights_h