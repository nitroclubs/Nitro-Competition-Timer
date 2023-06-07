#include "Lights.h"
#include <Arduino.h>

Lights::Lights(/* args */)
{
}

Lights::~Lights()
{
}

void Lights::setup()
{
    pinMode(ledPinRed_, OUTPUT);
    pinMode(ledPinYellow_, OUTPUT);
    pinMode(ledPinGreen_, OUTPUT);
}
void Lights::clearAll()
{
    digitalWrite(ledPinRed_, LOW);
    digitalWrite(ledPinYellow_, LOW);
    digitalWrite(ledPinGreen_, LOW);
}
void Lights::showRed()
{
    digitalWrite(ledPinRed_, HIGH);
}
void Lights::showYellow()
{
    digitalWrite(ledPinYellow_, HIGH);
}
void Lights::showGreen()
{
    digitalWrite(ledPinGreen_, HIGH);
}
void Lights::clearRed()
{
    digitalWrite(ledPinRed_, LOW);
}
void Lights::clearYellow()
{
    digitalWrite(ledPinYellow_, LOW);
}
void Lights::clearGreen()
{
    digitalWrite(ledPinGreen_, LOW);
}
void Lights::toggleRed()
{
    digitalWrite(ledPinRed_, !digitalRead(ledPinRed_));
}
void Lights::toggleYellow()
{
    digitalWrite(ledPinYellow_, !digitalRead(ledPinYellow_));
}
void Lights::toggleGreen()
{
    digitalWrite(ledPinGreen_, !digitalRead(ledPinGreen_));
}
