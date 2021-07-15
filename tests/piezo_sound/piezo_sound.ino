const uint8_t
  Piezo_pin = 9;
  
const uint16_t 
  StartFreq = 10,
  MaxFreq = 14000,
  Duration_ms = 150,
  Overlap_ms = 5;
  
const float 
  IncrementCoeff = 1.05;
  
const uint16_t 
  MinIncrement = 1;

void setup() {
  pinMode(Piezo_pin, OUTPUT);  
  Serial.begin(9600);
  
  uint16_t CurFreq = StartFreq;
  while (CurFreq <= MaxFreq) {
    Serial.println(CurFreq);
    tone(Piezo_pin, CurFreq, Duration_ms);
    delay(Duration_ms - Overlap_ms);
    CurFreq = max((float) CurFreq * IncrementCoeff, CurFreq + MinIncrement);
  }
}

void loop() {

}