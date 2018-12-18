#include "Structures.h"

void setup()
{
  Serial.begin(115200);
  setupStructures();
  setupStepsPointers(); 
}

void loop()
{
  repeating = nullptr;
  read8();
  if (shiftPressed) read16(true);
  else read16(false);

  handleMIDIMessage();
}

bool readControlButton(byte i)
{
  bool tmp = digitalRead(controlButtonsPort);
  controlButtons[i]->setReading(tmp);
  if (!tmp)
    return false;
  if (copyPressed) {
    if (selectedPattern == i - pat0) return false;
    copyPattern(selectedPattern, i - pat0, selectedInstrument);
    return false;
  }
  if (startPressed) {
    playingPattern = selectedPattern;
    return false;
  }
  if (i == pat0 || i == pat1 || i == pat2 || i == pat3)
  {
    if (selectedPattern == i - pat0)
      return false;
    selectedPattern = i - pat0;
    return true;
  }
  else
    return false;
}

void readInstrumentButton(byte i)
{
  bool value = digitalRead(instrumentsButtonsPort);
  if (beatRepeatPressed)
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
      //      readPotentiometer(i);
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  }
  else
  {
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      sendBits(i);
      if (*(steps[i]->value))
      {
        digitalWrite(stepsLedsPort, stepIndicator < 4); //liga posicao do beat
      }
      else if (instruments[selectedInstrument]->getPosition(selectedPattern) == i)
      {
        digitalWrite(stepsLedsPort, stepIndicator < 1); //percorre steps
      }
      else
      {
        digitalWrite(stepsLedsPort, false);
      }
      steps[i]->setReading(digitalRead(stepsButtonsPort));
      //read pots
      //      readPotentiometer(i);
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

void nextStep()
{
  for (byte i = 0; i < 8; i++)
  {
    if (instruments[i]->nextStep(playingPattern))
    {
      writeMIDI(0x90, instruments[i]->note, 0x7F);
    }
    else
    {
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
  instruments[selectedInst]->patterns[b]->s_length =
    instruments[selectedInst]->patterns[a]->s_length;
}

void setupStepsPointers()
{
  for (byte i = 0; i < 8; i++)
    steps[i]->setPointer(instruments[selectedInstrument]->patterns[selectedPattern]->values + i);
}

//void readPotentiometer(byte i) {
//  pots[i].setReading(analogRead(potsPort) >> 3);
//}
