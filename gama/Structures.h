#include <Arduino.h>
#include "Controller.h"
#define SEQUENCE_LENGTH 16

class Sequence {
  public:
    bool getStep(byte i);
    void setStep(byte i, bool value);
    void setLength(byte _length);
    bool values[SEQUENCE_LENGTH] = {false};
    byte s_length = 16;
    void reset();
};

class Instrument {
  public:
    Instrument(byte _note);
    bool getStep (byte pat, byte idx);
    void setStep (byte pat, byte idx, bool value);
    void resetSequence(void);
    void erasePattern(void);
    void setVelocityReading(byte value, unsigned long now);
    Sequence *patterns [4];
    Potentiometer *pot;
    bool nextStep(byte selectedPattern);
    byte note;
    byte getPosition(byte selectedPattern);
    byte getVelocity();
    byte currentPosition = 0;
    byte repeating = false;
    
};



extern Instrument *instruments[8];
extern Button *controlButtons[8];
extern Button *steps[16];
extern Button *instrumentsButtons[8];
extern bool instrumentsButtonsAux[8];
extern Potentiometer pots[16];

extern Instrument *repeating;
extern byte copyingPattern;
extern byte playingPattern;
extern byte selectedInstrument;
extern byte selectedPattern;
extern byte clockCounter;
extern byte stepIndicator;
extern byte page;

//setup
void setupStructures(void);
