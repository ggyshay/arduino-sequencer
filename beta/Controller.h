#include <Arduino.h>
#define instrumentsLedsPort 2
#define instrumentsButtonsPort 3
#define stepsLedsPort 4
#define stepButtonsPort 5
#define controlButtonsPort 6
#define potsPort A0
#define pat0 0
#define pat1 1
#define pat2 2
#define pat3 3

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
    bool values[16] = {false};
  private:
    byte s_length;
    byte currentPosition = 0;
};


class Instrument {
  public:
    bool getStep (byte pat, byte idx);
    void setStep (byte pat, byte idx, bool value);
    void resetSequence(void);
    Instrument(byte _note);
    Sequence *patterns [4];
    bool nextStep();
    byte note;
};

class Button {
  public:
    bool *value;
    void setReading(bool, byte);
    Button(bool *_value);

  private:
    bool buttonState = false;
    bool lastButtonState = false;
    unsigned long lastDebounceTime = 0;
};


void copyPattern(byte a, byte b, byte selectedInstrument);
