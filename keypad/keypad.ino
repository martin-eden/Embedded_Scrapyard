#include <Keypad.h>

const uint8_t
  NUM_ROWS = 4,
  NUM_COLS = 4;

const char keymap[NUM_ROWS][NUM_COLS] =
  {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
  };

uint8_t
  rowPins[NUM_ROWS] = {9, 8, 7, 6},
  colPins[NUM_COLS]= {5, 4, 3, 2};

Keypad myKeypad =
  Keypad(makeKeymap(keymap), rowPins, colPins, NUM_ROWS, NUM_COLS);

void setup() {
  Serial.begin(9600);
}

void loop() {
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY) {
    Serial.print(keypressed);
  }
}
