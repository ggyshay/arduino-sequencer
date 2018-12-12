#include <Arduino.h>
#define instrumentsLedsPort 2
#define instrumentsButtonsPort 7
#define stepsLedsPort 11
#define stepsButtonsPort 4
#define controlButtonsPort 6
#define potsPort A0
#define pat0 0
#define pat1 1
#define pat2 2
#define pat3 3
#define SEQUENCE_LENGTH 8

extern bool shiftPressed;
extern bool copyPressed;
extern bool beatRepeatPressed;
extern bool startPressed;
extern bool pat3Pressed;
extern bool pat2Pressed;
extern bool pat1Pressed;
extern bool  pat0Pressed;


class Sequence {
  public:
    bool getNextStep(void);
    void setStep(byte i, bool value);
    void setLength(byte _length);
    void resetSequence(void);
    bool values[SEQUENCE_LENGTH] = {false};
    byte s_length = SEQUENCE_LENGTH;
    byte getPosition(void);
  private:
    byte currentPosition = 0;
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
void sendBits(byte i);
void noteOn(byte cmd, byte pitch, byte velocity);
