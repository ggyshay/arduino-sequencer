byte lastBut = 0;
void setup() {
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  sendBits(lastBut);
  byte pValue = analogRead(A3)/64;
  
  for (byte i = 0; i < 16; i++) {
    sendBits(i);
    bool tmp = digitalRead(6);
    if(tmp) lastBut = i;
    digitalWrite(7, tmp);
    digitalWrite(5, digitalRead(4));
    digitalWrite(9, i <= pValue);
    delay(1);
  }
}

void sendBits(byte i) {
  digitalWrite(10, i & 0b1);
  digitalWrite(11, i & 0b10);
  digitalWrite(12, i & 0b100);
  digitalWrite(13, i & 0b1000);
}
