// Changing pin state at fixed intervals of time.

/*
  Status: done
  Version: 1.0
  Last mod.: 2021-07-01
*/

/*
  Wiring
  
  Add something for square wave on pin 9. Piezo of LED works fine.
*/

// Set CTC-to-OCR1A mode.
void Timer1_CountToOcr() {
  // WGM1 = B0100:
  bitWrite(TCCR1B, WGM13, 0);
  bitWrite(TCCR1B, WGM12, 1);
  bitWrite(TCCR1A, WGM11, 0);
  bitWrite(TCCR1A, WGM10, 0);
}

// Set prescaler to 1024.
void Timer1_Prescale1024() {
  // CS1 = B101:
  bitWrite(TCCR1B, CS12, 1);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS10, 1);
}

// Set prescaler to 256.
void Timer1_Prescale256() {
  // CS1 = B100:
  bitWrite(TCCR1B, CS12, 1);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS10, 0);
}

// Set prescaler to 64.
void Timer1_Prescale64() {
  // CS1 = B011:
  bitWrite(TCCR1B, CS12, 0);
  bitWrite(TCCR1B, CS11, 1);
  bitWrite(TCCR1B, CS10, 1);
}

// Set prescaler to 8.
void Timer1_Prescale8() {
  // CS1 = B010:
  bitWrite(TCCR1B, CS12, 0);
  bitWrite(TCCR1B, CS11, 1);
  bitWrite(TCCR1B, CS10, 0);
}

// Set prescaler to 1.
void Timer1_Prescale1() {
  // CS1 = B001:
  bitWrite(TCCR1B, CS12, 0);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS10, 1);
}

// Enable interrupt on OCR1A match.  
void Timer1_EnableCompareAInt() {
  // OCIE1A = 1:
  bitSet(TIMSK1, OCIE1A);  
}

// Toggle OC1A on compare match.   
void Timer1_ToggleOutputA_nonPwm() {
  // COM1A = B01:
  bitWrite(TCCR1A, COM1A1, 0);  
  bitWrite(TCCR1A, COM1A0, 1);  
}

// Reset counter.  
void Timer1_ResetCounter() {
  TCNT1 = 0;
}

// Set counting limit.
void Timer1_SetCompareValue(uint16_t value) {
  OCR1A = value;  
}

// Set OCR1A pin (PB1 == 9) for output.  
void Timer1_PrepareOutputA() {
  bitSet(DDRB, PB1); 
}

void setupTimerInterrupt(uint16_t phaseDuration) {
  uint8_t sreg = SREG;  
  cli();  
  
  Timer1_PrepareOutputA();
  Timer1_CountToOcr();
  //Timer1_Prescale256();  
  Timer1_Prescale1();  
  Timer1_ResetCounter();  
  Timer1_SetCompareValue(phaseDuration);  
  Timer1_ToggleOutputA_nonPwm();  
  
  SREG = sreg;  
}   

void setup() {
  // 31250 is 0.5Hz with 256 prescaler.  
  const uint16_t duration = 12; //31250 * 2;
  setupTimerInterrupt(duration);
  
  Serial.begin(9600);
}

void loop() {
  /*
  uint16_t input = analogRead(A0); 
  uint16_t output = map(input, 0, 1023, 0, 0xFFFF);
  Serial.println(output);
  delay(2000);
  */
}

