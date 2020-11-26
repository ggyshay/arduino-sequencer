#include "Structures.h"

Instrument *repeating;
byte copyingPattern = -1;
byte pressedPattern = -1;
byte playingPattern = 0;
byte selectedInstrument = 0;
byte selectedPattern = 0;
byte clockCounter = 0;
byte stepIndicator = 0;

bool Sequence::getStep(byte i)
{
  return values[i];
}

void Sequence::setStep(byte i, bool value)
{
  values[i] = values;
}

void Sequence::setLength(byte _length)
{
  s_length = _length;
}

void Sequence::reset(){
  for(byte i = 0; i < SEQUENCE_LENGTH; ++i){
    values[i] = false;
  }
}

bool Instrument::getStep(byte pat, byte idx)
{
  return patterns[pat]->values[idx];
}

void Instrument::setStep(byte pat, byte idx, bool value)
{
  if (value)
  {
    patterns[pat]->values[idx] = !patterns[pat]->values[idx];
  }
}

Instrument::Instrument(byte _note)
{
  note = _note;
  for (byte i = 0; i < 4; i++)
  {
    patterns[i] = new Sequence();
  }
  pot = new Potentiometer();
}

void Instrument::resetSequence()
{
  currentPosition = 0;
}

void Instrument::erasePattern(){
  patterns[selectedPattern]->reset();
}

bool Instrument::nextStep(byte selectedPattern)
{
  bool tmp = repeating ? true : patterns[selectedPattern]->getStep(currentPosition);
  currentPosition = (currentPosition + 1) % patterns[selectedPattern]->s_length;
  return tmp;
}

byte Instrument::getPosition(byte selectedPattern)
{
  return (currentPosition + patterns[selectedPattern]->s_length - 1) % patterns[selectedPattern]->s_length;
}

void Instrument::setVelocityReading(byte value, unsigned long now)
{
  pot->setReading(value, now);
}

byte Instrument::getVelocity()
{
  return pot->_value;
}

Instrument *instruments[8];
Button *controlButtons[8];
Button *steps[16];
Button *instrumentsButtons[8];
Potentiometer pots[16];
bool instrumentsButtonsAux[8] = {false};

void setupStructures()
{
  for (byte i = 0; i < 8; i++)
  {
    instruments[i] = new Instrument(36 + i);
    instrumentsButtons[i] = new Button(instrumentsButtonsAux + i, true);
    pots[i] = Potentiometer(48 + i);
  }
  for (byte i = 0; i < 16; i++)
  {
    steps[i] = new Button(nullptr, false);
  }
  controlButtons[0] = new Button(&shiftPressed, true);
  controlButtons[1] = new Button(&copyPressed, true);
  controlButtons[2] = new Button(&beatRepeatPressed, true);
  controlButtons[3] = new Button(&pat0Pressed, true);
  controlButtons[4] = new Button(&pat1Pressed, true);
  controlButtons[5] = new Button(&pat2Pressed, true);
  controlButtons[6] = new Button(&pat3Pressed, true);
  controlButtons[7] = new Button(&startPressed, true);
}
