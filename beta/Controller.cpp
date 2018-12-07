#include "Controller.h"
#define debounceDelay 50

bool shiftPressed = false;
bool copyPressed = false;
bool beatRepeatPressed = false;
bool startPressed = false;
bool pat0Pressed = false;
bool pat1Pressed = false;
bool pat2Pressed = false;
bool pat3Pressed = false;

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

void Instrument::setStep (byte pat, byte idx, bool value){
  patterns[pat] -> values[idx] = value;
}

Instrument::Instrument (byte _note){
  note = _note;
}

void Instrument::resetSequence(){
  for(byte i = 0; i < 4; i++){
    patterns[i]->resetSequence();
  }
}

bool Instrument::nextStep(){
  
}

void Button::setReading (bool reading, byte selected) {
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        *value = !*value;
      }
    }
  }

  lastButtonState = reading;
}

Button::Button(bool *_value){
  value = _value;
}

void copyPattern(byte a, byte b, byte selectedInst){
  // copy a to b
}
