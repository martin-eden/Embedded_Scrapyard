// Changing pin state at fixed intervals of time.

/*
  Status: works
  Version: 1.0
  Last mod.: 2021-06-27
*/

/*
  Based on great SparkFun Electronic lecture by Shawn Hymel.
  
  https://www.youtube.com/watch?v=2kr5A350H7E
*/

/*
  Wiring
  
  Add something for square wave on pin 9. Piezo of LED works fine.
*/

void setupTimerInterrupt() {
  // 31250 is 0.5Hz with 256 prescaler.  
  const uint32_t Timer1_Mark = 31250;
  
  cli();  
  
  // Set OCR1A pin (PB1 == 9) for output:  
  DDRB |= _BV(PB1);
  
  // Set CTC-to-OCR1A mode:
  TCCR1B &= ~_BV(WGM13);
  TCCR1B |= _BV(WGM12);
  TCCR1A &= ~_BV(WGM11);
  TCCR1A &= ~_BV(WGM10);
  
  // Set prescaler to 256:
  TCCR1B |= _BV(CS12);
  TCCR1B &= ~_BV(CS11);
  TCCR1B &= ~_BV(CS10);
  
  // Reset counter:  
  TCNT1 = 0;
  
  // Set counting limit:
  OCR1A = Timer1_Mark;
  
  // (01) Toggle OC1A on compare match:   
  TCCR1A &= ~_BV(COM1A1);
  TCCR1A |= _BV(COM1A0);
  
  // Enable interrupt on OCR1A match:  
  TIMSK1 |= _BV(OCIE1A);
  
  sei();  
} 
  

void setup() {
  setupTimerInterrupt();
}

void loop() {
}

