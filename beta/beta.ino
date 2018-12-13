#include "Controller.h"

Instrument *instruments[8];
Button *controlButtons[8];
Potentiometer pots[16];

Instrument *repeating;
byte copyingPattern = -1;
byte pressedPattern = -1;
byte selectedInstrument = -1;
byte selectedPattern = 0;
byte clockCounter = 0;

void setup() {
  pinMode(A0, INPUT);
  for (byte i = 0; i < 8; i++) {
    instruments[i] = new Instrument(0x24 + i);
  }
  for (byte i = 0; i < 16; i++) {
    pots[i] = new Potentiometer(48 + i);
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
      selectedPattern = 0;
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
    byte oldLength = instruments[selectedInstrument]->patterns[selectedPattern]->s_length;
    for (byte i = 0; i < 16; ++i) {
      sendBits(i);
      if (digitalRead(stepsButtonsPort)) newLength = i;
      if(i < oldLength) digitalWrite(stepsLedsPort, HIGH);
      else digitalWrite(stepsLedsPort, LOW);
      //read pots
      readPotentiometers(i);
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  } else {
    for (byte i = 0; i < 16; ++i) {
      sendBits(i);
      instruments[selectedInstrument]->setStep(selectedPattern, i, digitalRead(stepsButtonsPort));
      //pots
      readPotentiometers(i);
    }
  }
}


void handleMIDIMessage() {
  if (Serial.available() > 0) {
    byte c = Serial.read();

    if (c == 0xFA) {
      clockCounter = 5;
      for (byte i = 0; i < 8; i++) {
        instruments[i]->resetSequence();
      }
    } else if (c == 0xFC) {
      clockCounter = 0;
      for (byte i = 0; i < 8; i++) {
        instruments[i]->resetSequence();
      }
    } else if (c == 0xF8) {
      clockCounter++;
      if (clockCounter == 6)
      {
        clockCounter = 0;
        nextStep();
      }
    }
  }
}

void nextStep() {
  for (byte i = 0; i < 8; i++) {
    if (instruments[i]->nextStep(selectedPattern)){
      writeMIDI(0x90, instruments[i]->note, 0x4F);
    }else {
      writeMIDI(0x90, instruments[i]->note, 0);
    }
  }
}

void copyPattern(byte a, byte b, byte selectedInst) {
  for (byte i = 0; i < 16; i++) {
    instruments[selectedInst]->patterns[b]->values[i] =
      instruments[selectedInst]->patterns[a]->values[i];
  }
}

void readPotentiometer(byte i) {
  pots[i].setReading(analogRead(potsPort) >> 3);
}