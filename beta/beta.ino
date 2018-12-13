#include "Structures.h"

<<<<<<< HEAD
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
=======
void setup()
{
  Serial.begin(115200);
  setupStructures();
  setupStepsPointers();
>>>>>>> 686829424043131ca8ed8890ee3bf03bb001cba6
}

void loop()
{
  repeating = nullptr;
  read8();
  if (shiftPressed)
  {
    //    if (copyPressed) {
    //      if (pressedPattern != -1 && copyingPattern != -1) {
    //        copyPattern(copyingPattern, pressedPattern, selectedInstrument);
    //      }
    //    }
    read16(true); // read steps as length setting
  }
  else
  {
    //    if (copyPressed) {
    //      copyingPattern = pressedPattern;
    //    }
    read16(false); // read steps normally
  }

  handleMIDIMessage();
}

bool readControlButton(byte i)
{
  bool tmp = digitalRead(controlButtonsPort);
  controlButtons[i]->setReading(tmp);
  if (!tmp)
    return false;

  if (i == pat0 || i == pat1 || i == pat2 || i == pat3)
  {
    if (selectedPattern == i)
      return false;
    pressedPattern = i;
    selectedPattern = i;
    return true;
  }
  else
  {
    pressedPattern = -1;
    return false;
  }
}

void readInstrumentButton(byte i)
{
  bool value = digitalRead(instrumentsButtonsPort);
  if (!shiftPressed && beatRepeatPressed)
  {
    instruments[i]->repeating = value;
  }
  instrumentsButtons[i]->setReading(value);
}

void read8()
{
  for (byte i = 0; i < 8; ++i)
  {
    sendBits(i);
    bool patChanged = readControlButton(i);
    if (patChanged)
    {
      setupStepsPointers();
    }
    readInstrumentButton(i);
    if (*instrumentsButtons[i]->value && i != selectedInstrument)
    {
      selectedInstrument = i;
      setupStepsPointers();
    }
  }
}

void read16(bool shift)
{
  if (shift)
  {
    byte oldLength = instruments[selectedInstrument]->patterns[selectedPattern]->s_length;
    byte newLength = oldLength;
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      sendBits(i);
      if (digitalRead(stepsButtonsPort))
        newLength = i + 1;
      if (i < oldLength)
        digitalWrite(stepsLedsPort, HIGH);
      else
        digitalWrite(stepsLedsPort, LOW);
      //read pots
      readPotentiometers(i);
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  }
  else
  {
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      sendBits(i);
      //pots
      readPotentiometers(i);
      if (*(steps[i]->value))
      {
        digitalWrite(stepsLedsPort, stepIndicator < 4);
      }
      else if (instruments[selectedInstrument]->getPosition(selectedPattern) == i)
      {
        digitalWrite(stepsLedsPort, stepIndicator < 1);
      }
      else
      {
        digitalWrite(stepsLedsPort, false);
      }
      steps[i]->setReading(digitalRead(stepsButtonsPort));
    }
    stepIndicator = (stepIndicator + 1) & 0b00000111;
  }
}

void handleMIDIMessage()
{
  if (Serial.available() > 0)
  {
    byte c = Serial.read();
    if (c == 0xFA)
    {
      clockCounter = 5;
      for (byte i = 0; i < 8; i++)
      {
        instruments[i]->resetSequence();
      }
    }
    else if (c == 0xFC)
    {
      clockCounter = 0;
      for (byte i = 0; i < 8; i++)
      {
        instruments[i]->resetSequence();
      }
    }
    else if (c == 0xF8)
    {
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

void copyPattern(byte a, byte b, byte selectedInst)
{
  for (byte i = 0; i < SEQUENCE_LENGTH; i++)
  {
    instruments[selectedInst]->patterns[b]->values[i] =
        instruments[selectedInst]->patterns[a]->values[i];
  }
}

void readPotentiometer(byte i) {
  pots[i].setReading(analogRead(potsPort) >> 3);
}

void setupStepsPointers()
{
  for (byte i = 0; i < 8; i++)
    steps[i]->setPointer(instruments[selectedInstrument]->patterns[selectedPattern]->values + i);
}
