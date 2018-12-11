#include <Arduino.h>
#define instrumentsLedsPort 2
#define instrumentsButtonsPort 3
#define stepsLedsPort 4
#define stepsButtonsPort 5
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
extern bool pat0Pressed;


class Sequence {
  public:
    bool getNextStep(void);
    void setStep(byte i, bool value);
    void setLength(byte _length);
    void resetSequence(void);
    bool values[16] = {false};
    byte s_length;
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

class Potentiometer {
  public:
    Potentiometer(byte control);
    void setValue(byte value);
    byte getValue(void);

  private:
    byte _control;
    byte _value;
    byte _oldValue;
};

void sendBits(byte i);
void noteOn(byte cmd, byte pitch, byte velocity);

