#include "Structures.h"


Instrument *repeating;
byte copyingPattern = -1;
byte pressedPattern = -1;
byte playingPattern = 0;
byte selectedInstrument = 0;
byte selectedPattern = 0;
byte clockCounter = 0;
byte stepIndicator = 0;

Instrument *instruments[8];
Button *controlButtons[8];
Button *steps[8];
Button *instrumentsButtons[8];
Potentiometer pots[16];
bool instrumentsButtonsAux[8] = {false};

void setupStructures () {
  for (byte i = 0; i < 8; i++) {
    instruments[i] = new Instrument(36 + i);
    steps[i] = new Button(nullptr, false);
    instrumentsButtons[i] = new Button(instrumentsButtonsAux + i, true);
  }
  for (byte i = 0; i < 16; i++) {
    pots[i] = Potentiometer(48 + i);
  }
  controlButtons[0] = new Button(&shiftPressed, true);
  controlButtons[1] = new Button(&copyPressed, true);
  controlButtons[2] = new Button(&beatRepeatPressed, true);
  controlButtons[3] = new Button(&startPressed, true);
  controlButtons[4] = new Button(&pat0Pressed, true);
  controlButtons[5] = new Button(&pat1Pressed, true);
  controlButtons[6] = new Button(&pat2Pressed, true);
  controlButtons[7] = new Button(&pat3Pressed, true);

  pinMode(instrumentsLedsPort, OUTPUT);
  pinMode(stepsLedsPort, OUTPUT);

  pinMode(instrumentsButtonsPort, INPUT);
  pinMode(stepsButtonsPort, INPUT);
  pinMode(controlButtonsPort, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(A0, INPUT);
}
