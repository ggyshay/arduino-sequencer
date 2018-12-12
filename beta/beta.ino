#include "Controller.h"

Instrument *instruments[8];
Button *controlButtons[8];
Button *steps[8];
Button *instrumentsButtons[8];
bool instrumentsButtonsAux[8] = {false};

Instrument *repeating;
byte copyingPattern = -1;
byte pressedPattern = -1;
byte selectedInstrument = 0;
byte selectedPattern = 0;
byte clockCounter = 0;
byte stepIndicator = 0;

void setup() {
  Serial.begin(115200);
  for (byte i = 0; i < 8; i++) {
    instruments[i] = new Instrument(0x24 + i);
    steps[i] = new Button(nullptr, false);
    instrumentsButtons[i] = new Button(instrumentsButtonsAux + i, true);
  }
  controlButtons[0] = new Button(&shiftPressed, true);
  controlButtons[1] = new Button(&copyPressed, true);
  controlButtons[2] = new Button(&beatRepeatPressed, true);
  controlButtons[3] = new Button(&startPressed, false);
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

  setupStepsPointers();
}

void loop() {
  //read control buttons
  //  repeating = nullptr;
  for (byte i = 0; i < 8; ++i) {
    sendBits(i);
    readControlButton(i);
    
    readInstrumentButton(i);
    if (*instrumentsButtons[i]->value && i != selectedInstrument) {
      selectedInstrument = i;
      setupStepsPointers();
    }

  }

  if (shiftPressed) {
    //    if (copyPressed) {
    //      if (pressedPattern != -1 && copyingPattern != -1) {
    //        copyPattern(copyingPattern, pressedPattern, selectedInstrument);
    //      }
    //    }

    // read steps as length setting
    read16(true);
  } else {
    //    if (copyPressed) {
    //      copyingPattern = pressedPattern;
    //    }
    //le steps normal
    read16(false);
  }

  //  handleMIDIMessage();
}

void readControlButton(byte i) {
  controlButtons[i]->setReading(digitalRead(controlButtonsPort));
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

void readInstrumentButton(byte i) {
   
  //    if (!shiftPressed && beatRepeatPressed && value) {
  //      repeating = instruments[i];
  //    }
    instrumentsButtons[i]->setReading(digitalRead(instrumentsButtonsPort));
}


void read16(bool shift) {
  if (shift) {
    byte oldLength = instruments[selectedInstrument]->patterns[selectedPattern]->s_length;
    byte newLength = oldLength;
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i) {
      sendBits(i);
      if (digitalRead(stepsButtonsPort)) newLength = i + 1;
      if (i < oldLength) digitalWrite(stepsLedsPort, HIGH);
      else digitalWrite(stepsLedsPort, LOW);
      //read pots
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  } else {
    //    Serial.println("read16");
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i) {
      sendBits(i);
      if (*(steps[i]->value)) {
        digitalWrite(stepsLedsPort, stepIndicator < 4);
        // TODO: reintroduce index highlight
        //      } else if (instruments[selectedInstrument]->getPosition(selectedPattern) == i) {
        //        digitalWrite(stepsLedsPort, stepIndicator < 1);
      } else {
        digitalWrite(stepsLedsPort, false);
      }
      steps[i]->setReading(digitalRead(stepsButtonsPort));
    }
    stepIndicator = (stepIndicator + 1) & 0b00000111;
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
    if (instruments[i]->nextStep(selectedPattern)) {
      noteOn(0x90, instruments[i]->note, 0x4F);
    } else {
      noteOn(0x90, instruments[i]->note, 0);
    }
  }
}

void copyPattern(byte a, byte b, byte selectedInst) {
  for (byte i = 0; i < SEQUENCE_LENGTH; i++) {
    instruments[selectedInst]->patterns[b]->values[i] =
      instruments[selectedInst]->patterns[a]->values[i];
  }
}

void setupStepsPointers() {
  for (byte i = 0; i < 8; i++) {
    steps[i]->setPointer(instruments[selectedInstrument]->patterns[selectedPattern]->values + i);
  }
}
