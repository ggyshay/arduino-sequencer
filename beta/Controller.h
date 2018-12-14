#include <Arduino.h>
#define instrumentsLedsPort 2
#define instrumentsButtonsPort 7
#define stepsLedsPort 11
#define stepsButtonsPort 4
#define controlButtonsPort 6
#define potsPort A0
#define POT_RESOLUTION 2
#define POT_TIMEOUT 300
#define pat0 4
#define pat1 5
#define pat2 6
#define pat3 7
#define SEQUENCE_LENGTH 8

extern bool shiftPressed;
extern bool copyPressed;
extern bool beatRepeatPressed;
extern bool startPressed;
extern bool pat3Pressed;
extern bool pat2Pressed;
extern bool pat1Pressed;
extern bool pat0Pressed;


class Sequence {
  public:
    bool getStep(byte i);
    void setStep(byte i, bool value);
    void setLength(byte _length);
//    void resetSequence(void);
    bool values[SEQUENCE_LENGTH] = {false};
    byte s_length = SEQUENCE_LENGTH;
//    byte getPosition(void);
//    byte currentPosition = 0;
};


class Instrument {
  public:
    bool getStep (byte pat, byte idx);
    void setStep (byte pat, byte idx, bool value);
    void resetSequence(void);
    Instrument(byte _note);
    Sequence *patterns [4];
    bool nextStep(byte selectedPattern);
    byte note;
    byte getPosition(byte selectedPattern);
    byte currentPosition = 0;
    byte repeating = false;
};

class Button {
    // be possible to be release sensitive or not
  public:
    bool *value;
    bool isReleaseSensitive;
    void setReading(bool);
    Button(bool *_value, bool _isReleaseSensitive);
    void setPointer(bool *ptr);
  private:
    bool buttonState = false;
    bool lastButtonState = false;
    unsigned long lastDebounceTime = 0; // TODO: change to byte, this is too costly

};

class Potentiometer {
  public:
    Potentiometer(byte control);
    Potentiometer();
    void setReading(byte value);
    long potTime;
    bool moving;

  private:
    byte _control;
    byte _value;
};
void sendBits(byte i);
void writeMIDI(byte cmd, byte pitch, byte velocity);
