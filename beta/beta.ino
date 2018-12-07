#include "Controller.h"

Instrument *instruments[8];
Button *controlButtons[8];

Instrument *repeating;
byte copyingPattern = -1;
byte pressedPattern = -1;
byte selectedInstrument = -1;
byte selectedPattern = -1;

void setup() {
  for (byte i = 0; i < 8; i++) {
    instruments[i] = new Instrument(0x24 + i);
  }
  controlButtons[0] = new Button(&shiftPressed);
  controlButtons[1] = new Button(&copyPressed);
  controlButtons[2] = new Button(&beatRepeatPressed);
  controlButtons[3] = new Button(&startPressed);
  controlButtons[4] = new Button(&pat0Pressed);
  controlButtons[5] = new Button(&pat1Pressed);
  controlButtons[6] = new Button(&pat2Pressed);
  controlButtons[7] = new Button(&pat3Pressed);
}

void loop() {
  //read control buttons
  repeating = nullptr;
  selectedInstrument = -1;
  for (byte i = 0; i < 8; ++i) {
    sendBits(i);
    readControlButton(digitalRead(controlButtonsPort), i);
    readInstrumentButton(digitalRead(instrumentsButtonsPort), i);
  }

  //asm tree
  if (shiftPressed) {
    if (copyPressed) {
      if (pressedPattern != -1 && copyingPattern != -1) {
        copyPattern(copyingPattern, pressedPattern, selectedInstrument);
      }
    }

    // read steps as length setting
    read16(true);
  } else {
    if (copyPressed) {
      copyingPattern = pressedPattern;
    }

    //le steps normal
    read16(false);
  }

  handleMIDIMessage();
}

void readControlButton(bool value, byte i) {
  controlButtons[i]->setReading(value, i);
  switch (i) {
    case pat0:
      pressedPattern = 0;
      selectedPattern = 0
      break;
    case pat1:
      pressedPattern = 1;
      selectedPattern = 1;
      break;
    case pat2:
      pressedPattern = 2;
      selectedPattern = 2;
      break;
    case pat3:
      pressedPattern = 3;
      selectedPattern = 3;
      break;
    default:
      pressedPattern = -1;
      break;
  }
}

void readInstrumentButton(bool value, byte i) {
  if (!shiftPressed && beatRepeatPressed && value) {
    repeating = instruments[i];
  }
  if (value) selectedInstrument = i;
}


void read16(bool shift) {
  if (shift) {
    byte newLength = 0;
    for (byte i = 0; i < 16; ++i) {
      sendBits(i);
      if (digitalRead(stepButtonsPort)) newLength = i;
      //read pots
    }
  } else {
    for (byte i = 0; i < 16; ++i) {
      sendBits(i);
      instruments[selectedInstrument]->setStep(selectedPattern, i)
    }
  }
}


void handleMIDIMessage() {
  // TODO: copy midi code
}
