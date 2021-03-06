
#include "Controller.h"
#define debounceDelay 50

bool shiftPressed = false;
bool copyPressed = false;
bool beatRepeatPressed = false;
bool startPressed = false;
bool patPlaying = false;
bool pat0Pressed = false;
bool pat1Pressed = false;
bool pat2Pressed = false;
bool pat3Pressed = false;

void Button::setReading(bool reading)
{
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      if (isReleaseSensitive)
      {
        *value = buttonState;
      }
      else
      {
        if (buttonState == HIGH)
        {
          *value = !*value;
        }
      }
    }
  }
  lastButtonState = reading;
}

Button::Button(bool *_value, bool _isReleaseSensitive)
{
  value = _value;
  isReleaseSensitive = _isReleaseSensitive;
}

void Button::setPointer(bool *ptr)
{
  value = ptr;
}

Potentiometer::Potentiometer(byte control)
{
  _control = control;
  _value = 0;
  isMidiEnabled = true;
}

Potentiometer::Potentiometer()
{
  isMidiEnabled = false;
  _control = 100;
  _value = 0x7F;
}

void Potentiometer::setReading(byte value, unsigned long now)
{
  if (_value - value > POT_RESOLUTION || _value - value < -POT_RESOLUTION)
  {
    potTime = now;
  }

  if (potTime + POT_TIMEOUT > now)
  {
    moving = true;
  }
  else
  {
    moving = false;
  }

  if (moving)
  {
    _value = value;
    if (isMidiEnabled) {
      writeMIDI(0xB0, _control, _value);
    }
  }
}

void sendBits(byte n)
{
  PORTB = PORTB & B110000;
  PORTB = PORTB | n; // numbers starting at port 8 (to 11)
}

void writeMIDI(byte cmd, byte pitch, byte velocity)
{
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);

//  Wire.beginTransmission(8); // transmit to device #9
//  Wire.write(cmd);
//  Wire.write(pitch);
//  Wire.write(velocity);
//  Wire.endTransmission(); // stop transmitting
}


void setupController() {
  pinMode(instrumentsLedsPort, OUTPUT);
  pinMode(stepsLedsPort, OUTPUT);
  pinMode(instrumentsButtonsPort, INPUT);
  pinMode(stepsButtonsPort, INPUT);
  pinMode(controlButtonsPort, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(pat0Led, OUTPUT);
  pinMode(pat1Led, OUTPUT);
  pinMode(pat2Led, OUTPUT);
  pinMode(pat3Led, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}
