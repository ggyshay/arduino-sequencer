#include "Structures.h"
#include <Wire.h>

void setup()
{
  //setup comunication through serial and wire
  Serial.begin(112500);
  Wire.begin(0x66);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(nextStep);

  setupController();    // pinModes
  setupStructures();    // setup instruments, steps, pots and controls
  setupStepsPointers(); // exactly that
}

void loop()
{
  repeating = nullptr; // should be somewhere else
  readInterface();
  writePattern(); // TODO: optimize for outputs in the same PORT
  changeControlState();
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

void readControlI(byte i)
{
  digitalWrite(instrumentsLedsPort, i == selectedInstrument);
  controlButtons[7 - i]->setReading(digitalRead(controlButtonsPort));
  readInstrumentButton(i);
  if (*instrumentsButtons[i]->value && i != selectedInstrument)
  {
    selectedInstrument = i;
    setupStepsPointers();
  }
  if (i == selectedInstrument)
  {
    delayMicroseconds(150);
    digitalWrite(instrumentsLedsPort, false);
  }
}

void readInterface()
{
  unsigned long now = millis();
  if (shiftPressed)
  {
    byte oldLength = instruments[selectedInstrument]->patterns[selectedPattern]->s_length;
    byte newLength = oldLength;
    byte endPage = oldLength >> 4;
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      sendBits(i);
      if (digitalRead(stepsButtonsPort))
        newLength = i + 1 + selectedPage * 16;

      if (selectedPage < endPage)
      {
        digitalWrite(stepsLedsPort, HIGH);
      }
      else if (selectedPage > endPage)
      {
        digitalWrite(stepsLedsPort, LOW);
      }
      else
      {
        digitalWrite(stepsLedsPort, i < oldLength % 16);
      }

      readPotentiometer(i, now);
      readControlI(7 - (i & 7));
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  }
  else
  {
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      sendBits(i); // manda os bits
      stepBank->updateI(i, now, instruments[selectedInstrument]->getPosition(selectedPattern) == (i + selectedPage * 16));

      readPotentiometer(i, now); // le um potenciometro
      readControlI(7 - (i & 7)); // le botões de conrle
    }
    stepBank->updateStepIndicator();
  }
}

void nextStep()
{
  globalStepPosition = (globalStepPosition + 1) & 0xF;
  if (globalStepPosition == 0 && forceSyncQueued) {
    for (byte i = 0; i < 8; ++i)
    {
      instruments[i]->currentPosition = 0;
    }
    forceSyncQueued = false;
  }
  for (byte i = 0; i < 8; i++)
  {
    Wire.write(instruments[i]->nextStep(playingPattern) ? instruments[i]->getVelocity() : 0);
  }
}

void setupStepsPointers()
{
  stepBank->values = &(instruments[selectedInstrument]->patterns[selectedPattern]->pages[selectedPage]);
}

void readPotentiometer(byte i, unsigned long now)
{
  byte value = analogRead(potsPort) >> 3;
  if (i >= 8)
  {
    instruments[15 - i]->setVelocityReading(value, now);
  }
  else
  {
    pots[7 - i].setReading(value, now);
  }
}

void writePattern()
{
  PORTC &= B110001;
  switch (shiftPressed ? selectedPage : selectedPattern)
  {
    case 0:
      digitalWrite(12, HIGH);
      break;
    case 1:
      digitalWrite(A3, HIGH);
      digitalWrite(12, LOW);
      break;
    case 2:
      digitalWrite(A2, HIGH);
      digitalWrite(12, LOW);
      break;
    case 3:
      digitalWrite(A1, HIGH);
      digitalWrite(12, LOW);
      break;
  }
}

void changeControlState()
{
  if (shiftPressed)
  {
    if (copyPressed && playingPattern != selectedPattern)
    {
      copyWholePattern(playingPattern, selectedPattern);
    }

    if (startPressed)
    {
      forceSyncQueued = true ;
    }

    // optimize
    if (pat0Pressed)
    {
      selectedPage = 0;
      setupStepsPointers();
    }
    else if (pat1Pressed)
    {
      selectedPage = 1;
      setupStepsPointers();
    }
    else if (pat2Pressed)
    {
      selectedPage = 2;
      setupStepsPointers();
    }
    else if (pat3Pressed)
    {
      selectedPage = 3;
      setupStepsPointers();
    }
  }
  else if (beatRepeatPressed && copyPressed)
  {
    eraseSelectedPattern();
  }
  else
  {
    if (copyPressed && playingPattern != selectedPattern)
      copyPattern(playingPattern, selectedPattern, selectedInstrument);

    if (startPressed)
      playingPattern = selectedPattern;

    if (pat0Pressed) // TODO: optimize
    {
      selectedPattern = 0;
      setupStepsPointers();
    }
    else if (pat1Pressed)
    {
      selectedPattern = 1;
      setupStepsPointers();
    }
    else if (pat2Pressed)
    {
      selectedPattern = 2;
      setupStepsPointers();
    }
    else if (pat3Pressed)
    {
      selectedPattern = 3;
      setupStepsPointers();
    }
  }
}

void receiveEvent(int)
{
  byte c = Wire.read();
  if (c == 0xFA || c == 0xFC)
  {
    globalStepPosition = 0;
    for (byte i = 0; i < 8; i++)
    {
      instruments[i]->resetSequence();
    }
  }
}
