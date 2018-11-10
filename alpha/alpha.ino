////#define ledPort A0
////#define buttonsPort A1
////#define debounceDelay 5
////
////using namespace std;
//
#define ledBlink 13
long last = 0;
bool isOn = false;
bool testSteps[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
////
////class Button {
////  public:
////    bool value;
////    void setReading (bool);
////
////  private:
////    bool buttonState = false;
////    bool lastButtonState = false;
////    unsigned long lastDebounceTime = 0;
////
////};
////
////void Button::setReading (bool reading) {
////  if (reading != lastButtonState) {
////    lastDebounceTime = millis();
////  }
////
////  if ((millis() - lastDebounceTime) > debounceDelay) {
////    if (reading != buttonState) {
////      buttonState = reading;
////
////      if (buttonState == HIGH) {
////        value = !value;
////      }
////    }
////  }
////
////  lastButtonState = reading;
////}
////
byte clockCounter = 0;
byte stepIndex = 0;
////
////Button steps[8];
//
//
void setup()
{
  //  Serial.begin(31250);
  Serial.begin(115200);
  pinMode(ledBlink, OUTPUT);
  //  pinMode(6, OUTPUT);
  //  pinMode(7, OUTPUT);
  //  pinMode(8, OUTPUT);
  //  pinMode(ledPort, OUTPUT);
  //  pinMode(buttonsPort, INPUT);
  //  MIDI.setInputChannel(1);
  //  MIDI.begin();
  //  MIDI.turnThruOff();
}

void loop()
{
  //  updateSteps();

  if (Serial.available() > 0) {
    byte c = Serial.read();

    if (c == 0xFA) {
      clockCounter = 5;
    } else if (c == 0xFC) {
      clockCounter = 0;
    } else if (c == 0xF8) {
      clockCounter++;
      if (clockCounter == 6)
      {
        last = millis();
        clockCounter = 0;
        nextStep();
        isOn = true;
      }
    }
  }



  //  if (millis() - last > 100 && isOn) {
  //    noteOn(0x90, 0x3C, 0x00);
  //    isOn = false;
  //  }
  digitalWrite(ledBlink, millis() - last < 100);
}
//
////void sendBits(byte n, byte start, bool is16) { //funcao que manda as saidas mudarem no digital
////  //  portd são os pinos de 0 a 7
////  //  mexe tudo pra direita pra não afetar os pinos 0 e 1
////  //  zera o resto dos pinos
////  //  associa o tmp ao portd
////
////  //  byte tmp = n;
////  //  tmp = tmp << 2;
////  //  PORTD = PORTD & B00000011;
////  //  PORTD = PORTD | tmp;
////  byte  tmp = n;
////  if (tmp % 2 == 1)digitalWrite(start, HIGH);
////  else digitalWrite(start, LOW);
////  tmp = tmp / 2;
////
////  if (tmp % 2 == 1)digitalWrite(start + 1, HIGH);
////  else digitalWrite(start + 1, LOW);
////  tmp = tmp / 2;
////
////  if (tmp % 2 == 1)digitalWrite(start + 2, HIGH);
////  else digitalWrite(start + 2, LOW);
////  tmp = tmp / 2;
////
////  if (is16) {
////    if (tmp % 2 == 1)digitalWrite(start + 3, HIGH);
////    else digitalWrite(start + 3, LOW);
////  }
////}
////
void nextStep()
{
  //  if (steps[stepIndex].value)
    if (testSteps[stepIndex] == 1)
  {
    noteOn(0x90, 0x3C, 0x4F);
  }
  else
  {
    noteOn(0x90, 0x3C, 0x00);
  }

  stepIndex = (stepIndex + 1) % 16;
}

//void updateSteps() {
//  for (byte i = 0; i < 8; i++) {
//    sendBits(i, 6, false);
//    digitalWrite(ledPort, steps[i].value);
//    delay(1);
//    //            digitalWrite(ledPort, true);
//    bool value = digitalRead(buttonsPort);
//    steps[i].setReading(value);
//  }
//}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
