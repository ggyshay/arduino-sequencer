#include <Arduino.h>
#include "Controller.h"

// data structures
extern Instrument *instruments[8];
extern Button *controlButtons[8];
extern Button *steps[8];
extern Button *instrumentsButtons[8];
extern bool instrumentsButtonsAux[8];
extern Potentiometer pots[16];

// flags and aux vars
extern Instrument *repeating;
extern byte copyingPattern;
extern byte playingPattern;
extern byte selectedInstrument;
extern byte selectedPattern;
extern byte clockCounter;
extern byte stepIndicator;

//setup
void setupStructures(void);
