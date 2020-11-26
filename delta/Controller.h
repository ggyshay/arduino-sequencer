#include <Arduino.h>
#include <Wire.h>
#define instrumentsLedsPort 2
#define instrumentsButtonsPort 7
#define stepsLedsPort 3
#define stepsButtonsPort 4
#define controlButtonsPort 6
#define potsPort A0
#define pat3Led A1
#define pat2Led A2
#define pat1Led A3
#define pat0Led 12
#define POT_RESOLUTION 2
#define POT_TIMEOUT 300
#define pat0 4
#define pat1 5
#define pat2 6
#define pat3 7

extern bool shiftPressed;
extern bool copyPressed;
extern bool beatRepeatPressed;
extern bool startPressed;
extern bool patPlaying;
extern bool pat3Pressed;
extern bool pat2Pressed;
extern bool pat1Pressed;
extern bool pat0Pressed;

class Potentiometer
{
public:
  Potentiometer(byte control);
  Potentiometer();
  void setReading(byte value, unsigned long now);
  unsigned long potTime;
  bool moving;
  byte _value;
  byte _control;
  bool isMidiEnabled = false;

private:
};

class Button
{
public:
  bool *value;
  bool isReleaseSensitive;
  void setReading(bool);
  Button(bool *_value, bool _isReleaseSensitive);
  void setPointer(bool *ptr);

private:
  bool buttonState = false;
  bool lastButtonState = false;
  unsigned long lastDebounceTime = 0;
};

class StepBank
{
public:
  void updateI(byte, unsigned int, bool);
  unsigned int *values = 0;
  unsigned int lastState = 0;
  byte stepIndicator = 0;
  void updateStepIndicator();
  unsigned int lastDebounceTimes[16];
};

void sendBits(byte i);
void writeMIDI(byte cmd, byte pitch, byte velocity);
void setupController();
