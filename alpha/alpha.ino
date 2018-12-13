#define ledPort 11
#define buttonsPort 4
#define debounceDelay 5
#define instrumentPort A0


byte stepIndicator = false;

class SimpleButton {
  public:
    byte value = 0;
    void setReading(bool);

  private:
    bool buttonState = false;
    bool lastButtonState = false;
    unsigned long lastDebounceTime = 0;
};


void SimpleButton::setReading (bool reading) {
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        value = (value + 1) % 4;
      }
    }
  }

  lastButtonState = reading;
}

class Button {
  public:
    bool values[4] = {0, 0, 0, 0};
    void setReading(bool, byte);

  private:
    bool buttonState = false;
    bool lastButtonState = false;
    unsigned long lastDebounceTime = 0;

};

void Button::setReading (bool reading, byte selected) {
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        values[selected] = !values[selected];
      }
    }
  }

  lastButtonState = reading;
}

byte clockCounter = 0;
byte stepIndex = 0;
byte selectedInst = 0;

Button steps[8];
SimpleButton instButton;


void setup()
{
  //  Serial.begin(31250);
  Serial.begin(115200);
  pinMode(ledPort, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(buttonsPort, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  updateSteps();
  instButton.setReading(digitalRead(instrumentPort));
  selectedInst = instButton.value;
  digitalWrite(LED_BUILTIN, selectedInst);

  if (Serial.available() > 0) {
    byte c = Serial.read();

    if (c == 0xFA) {
      clockCounter = 5;
      stepIndex = 0;
    } else if (c == 0xFC) {
      clockCounter = 0;
      stepIndex = 0;
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

void sendBits(byte n, byte start) {
  PORTB = PORTB & B100000;
  PORTB = PORTB | n; // numbers starting at port 8 (to 11)
}

void nextStep()
{
  stepIndex = (stepIndex + 1) % 8;
  for (byte i = 0; i < 4; i++) {
    if (steps[stepIndex].values[i])
    {
      noteOn(0x90, 0x24 + i, 0x4F);
    }
    else
    {
      noteOn(0x90, 0x24 + i, 0x00);
    }
  }
}

void updateSteps() {
  for (byte i = 0; i < 8; i++) {
    sendBits(i, 6);
    if (steps[i].values[selectedInst]) {
      digitalWrite(ledPort, stepIndicator < 4);
    } else if (stepIndex == i) {
      digitalWrite(ledPort, stepIndicator < 1);
    } else {
      digitalWrite(ledPort, false);
    }
    steps[i].setReading(digitalRead(buttonsPort), selectedInst);
  }
  stepIndicator = stepIndicator > 7 ? 0 : stepIndicator + 1;
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
