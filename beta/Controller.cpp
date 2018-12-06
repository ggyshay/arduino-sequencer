#include "Controller.h";

bool Sequence::getNextStep() {
  return values[currentPosition++];
}

void Sequence::setStep(byte i, bool value) {
  values[i] = values;
}

void Sequence::setLength(byte _length) {
  s_length = _length;
}

void Sequence::resetSequence() {
  currentPosition = 0;
}

bool Instrument::getStep (byte pat, byte idx){
  return patterns[pat] -> values[idx];
}

bool Instrument::setStep (byte pat, byte idx){
  patterns[pat] -> values[idx];
}

Instrument::Instrument (byte _note){
  note = _note;
}
