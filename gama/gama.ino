#include "Structures.h"
#include <Wire.h>
void setup()
{
  //  Serial.begin(31250);
  Serial.begin(112500);
  Wire.begin(0x66);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(handleClock);

  setupController();
  setupStructures();
  setupStepsPointers();
  pinMode(12, OUTPUT);
}

void loop()
{
  repeating = nullptr;
  read16(shiftPressed);
  handleMIDIMessage();
  writePattern();
  changeControlState(shiftPressed);
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

void read16(bool shift)
{
  unsigned long now = millis();
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

      readPotentiometer(i, now);
      readControlI(7 - (i & 7));
    }
    instruments[selectedInstrument]->patterns[selectedPattern]->s_length = newLength;
  }
  else
  {
    for (byte i = 0; i < SEQUENCE_LENGTH; ++i)
    {
      bool active = *(steps[i]->value);
      bool steping = instruments[selectedInstrument]->getPosition(selectedPattern) == i;
      sendBits(i);
      if (active && steping)
      {
        digitalWrite(stepsLedsPort, true);
      }
      else if (active)
      {
        digitalWrite(stepsLedsPort, stepIndicator < 2);
      }
      else if (steping)
      {
        digitalWrite(stepsLedsPort, stepIndicator < 1);
      }
      steps[i]->setReading(digitalRead(stepsButtonsPort));
      readPotentiometer(i, now);
      digitalWrite(stepsLedsPort, false);
      readControlI(7 - (i & 7));
    }
    stepIndicator = (stepIndicator + 1) & 0b00000011;
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
      writeMIDI(0x90, instruments[i]->note, instruments[i]->getVelocity());
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
  for (byte i = 0; i < 16; i++)
    steps[i]->setPointer(instruments[selectedInstrument]->patterns[selectedPattern]->values + i);
}

void eraseSelectedPattern() {
  for (byte i = 0; i < 16; ++i) {
    *(steps[i]->value) = false;
  }
  for (byte i = 0; i < 8; ++i) {
    instruments[i]->erasePattern();
  }
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
  switch (selectedPattern) {
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

void changeControlState(bool shift)
{
  if (shift) {
    if (copyPressed) {
      eraseSelectedPattern();
    }

    //muda a pagina mostrando agora
    //    if (pat0Pressed)
    //    {
    //    }
    //    else if (pat1Pressed)
    //    {
    //    }
    //    else if (pat2Pressed)
    //    {
    //
    //    }
    //    else if (pat3Pressed)
    //    {
    //    }
  } else {
    if (copyPressed && playingPattern != selectedPattern)
      copyPattern(playingPattern, selectedPattern, selectedInstrument);

    if (startPressed)
      playingPattern = selectedPattern;

    if (pat0Pressed)
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

void receiveEvent(int){
  byte c = Wire.read();
  if(c == 0xFA || c == 0xFC){
    for (byte i = 0; i < 8; i++)
      {
        instruments[i]->resetSequence();
      }
  }else if(c == 0xFC){
         for (byte i = 0; i < 8; i++)
      {
        instruments[i]->resetSequence();
      }
  }
  
}


void handleClock() {
 nextStep(); 
}
 
