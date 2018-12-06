#include <Arduino.h>

class Sequence {
  public:
    bool getNextStep(void);
    void setStep(byte i, bool value);
    void setLength(byte _length);
    void resetSequence(void);
    bool values[16] = {false};
  private:
    byte s_length;
    
    byte currentPosition = 0;
};


class Instrument {
  public:
    bool getStep (byte pat, byte idx);
    bool setStep (byte pat, byte idx);
    Instrument(byte _note);
    Sequence *patterns [4];
  private:
    byte note;
};
