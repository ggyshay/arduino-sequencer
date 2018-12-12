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

byte Sequence::getPosition() {
  return currentPosition;
}
bool Instrument::getStep (byte pat, byte idx) {
  return patterns[pat] -> values[idx];
}

void Instrument::setStep (byte pat, byte idx, bool value) {
  if (value) {
    patterns[pat] -> values[idx] = !patterns[pat] -> values[idx];
  }

}

Instrument::Instrument (byte _note) {
  note = _note;
  for(byte i = 0; i < 4; i++){
    patterns[i] = new Sequence();
  }
}

void Instrument::resetSequence() {
  for (byte i = 0; i < 4; i++) {
    patterns[i]->resetSequence();
  }
}

bool Instrument::nextStep(byte selectedPattern) {
  return patterns[selectedPattern]->getNextStep();
}

byte Instrument::getPosition(byte selectedPattern) {
  return patterns[selectedPattern]->getPosition();
}

void Button::setReading (bool reading) {
  if(!value || value == nullptr){
//    Serial.println("null value!!!");
  }
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if(isReleaseSensitive){
        *value = buttonState;
      }else {
        if (buttonState == HIGH) {
          *value = !*value;
        }
      }
    }
  }
  lastButtonState = reading;
}

Button::Button(bool *_value, bool _isReleaseSensitive) {
  value = _value;
  isReleaseSensitive = _isReleaseSensitive;
}

void Button::setPointer(bool *ptr){
  value = ptr;
}

void sendBits(byte n) {
  PORTB = PORTB & B100000;
  PORTB = PORTB | n; // numbers starting at port 8 (to 11)
}

void noteOn(byte cmd, byte pitch, byte velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
