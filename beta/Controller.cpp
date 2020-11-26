
#include "Controller.h"
#define debounceDelay 50

bool shiftPressed = false;
bool copyPressed = false;
bool beatRepeatPressed = false;
bool startPressed = false;
bool patPlaying = false;
bool pat0Pressed = false;
bool pat1Pressed = false;
bool pat2Pressed = false;
bool pat3Pressed = false;

bool Sequence::getStep(byte i) {
  //  bool tmp = values[currentPosition++];
  //  currentPosition %= s_length;
  return values[i];
}

void Sequence::setStep(byte i, bool value) {
  values[i] = values;
}

void Sequence::setLength(byte _length) {
  s_length = _length;
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
  for (byte i = 0; i < 4; i++) {
    patterns[i] = new Sequence();
  }
}

void Instrument::resetSequence() {
  currentPosition = 0;
}

bool Instrument::nextStep(byte selectedPattern) {
  bool tmp = repeating ? true : patterns[selectedPattern]->getStep(currentPosition);
  currentPosition = (currentPosition + 1) % patterns[selectedPattern]->s_length;
  return tmp;
}

byte Instrument::getPosition(byte selectedPattern) {
  return (currentPosition + patterns[selectedPattern]->s_length - 1) % patterns[selectedPattern]->s_length;
}

void Button::setReading (bool reading) {
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (isReleaseSensitive) {
        *value = buttonState;
      } else {
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

void Button::setPointer(bool *ptr) {
  value = ptr;
}

Potentiometer::Potentiometer(byte control) {
  _control = control;
  _value = 0;
}

Potentiometer::Potentiometer() {
  _value = 0;
}

void Potentiometer::setReading(byte value) {
  if (_value - value > POT_RESOLUTION || _value - value < - POT_RESOLUTION){
    potTime = millis();
  }
    
  if (potTime + POT_TIMEOUT > millis()){
    moving = true;
  } else {
    moving = false;
  }

  if (moving){
    _value = value;
    writeMIDI(_control, _value, 0);
  }
}

void sendBits(byte n) {
  PORTB = PORTB & B100000;
  PORTB = PORTB | n; // numbers starting at port 8 (to 11)
}

void writeMIDI(byte cmd, byte pitch, byte velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
