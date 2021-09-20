// Rotary encoder demo for ST7789 OLED display

/*
  Status: done
  Version: 2.0
  Last mod.: 2021-09-19
*/

#include <me_RotaryEncoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

const uint8_t
  PhaseA_pin = 2,
  PhaseB_pin = 3,
  Switch_pin = 4,
  CS_pin = 8,
  DC_pin = 9,
  RST_pin = 10;

const int32_t
  Debounce_ms = 40;

Adafruit_ST7789 display = Adafruit_ST7789(CS_pin, DC_pin, RST_pin);

class EncoderGauge {
public:
  EncoderGauge();
  int32_t getNumPositions();
  void setNumPositions(int32_t _numPositions);
  void setPosition(int32_t _position);
  void initDisplay();
private:
  int32_t numPositions;
  int32_t position;
  int32_t prevPosition;
  void erasePointer();
  void drawPointer();
  void drawPointer_raw(int32_t _position, int16_t _color);
  void drawText_raw(int32_t _position, int16_t _color);
  void drawBackground();
};

EncoderGauge::EncoderGauge() {
  numPositions = 80;
}

void EncoderGauge::initDisplay() {
  display.init(240, 240);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(2);

  drawBackground();

  prevPosition = numPositions;
  position = 0;
  setPosition(position);
}

int32_t EncoderGauge::getNumPositions() {
  return numPositions;
}

void EncoderGauge::setNumPositions(int32_t _numPositions) {
  if (_numPositions != 0) {
    erasePointer();
    numPositions = _numPositions;
    drawPointer();
  }
}

void EncoderGauge::setPosition(int32_t _position) {
  if (_position == prevPosition)
    return;

  position = _position;

  erasePointer();
  drawPointer();

  prevPosition = position;
}

void EncoderGauge::erasePointer() {
  drawPointer_raw(prevPosition, ST77XX_BLACK);
  drawText_raw(prevPosition, ST77XX_BLACK);
}

void EncoderGauge::drawPointer() {
  drawPointer_raw(position, ST77XX_YELLOW);
  drawText_raw(position, ST77XX_WHITE);
}

void EncoderGauge::drawBackground() {
  uint16_t x, y, radius, color;
  x = display.width() / 2;
  y = display.height() / 2;

  radius = 120;
  color = ST77XX_BLUE;
/*  for (uint8_t i = 0; i < 6; ++i) {
    display.drawCircle(x, y, radius - i, color);
  }
*/
  display.fillCircle(x, y, radius, color);
}

void EncoderGauge::drawPointer_raw(int32_t _position, int16_t _color) {
  float scale = 2 * PI / numPositions;
  float circleX, circleY;
  uint16_t radius = 6;
  circleX = 120 + (104 + radius) * sin(_position * scale);
  circleY = 120 - (104 + radius) * cos(_position * scale);

  display.fillCircle(circleX, circleY, radius, _color);
}

void EncoderGauge::drawText_raw(int32_t _position, int16_t _color) {
  const uint16_t
    x = 0,
    y = 0;

  display.setTextColor(_color);
  display.setCursor(x, y);
  display.print(_position);
}

EncoderGauge encoderGauge;
RotaryEncoder rotaryEncoder(PhaseA_pin, PhaseB_pin, Switch_pin);

void setup() {
  Serial.begin(9600);

  rotaryEncoder.Debounce_ms = Debounce_ms;

  attachInterrupt(digitalPinToInterrupt(PhaseA_pin), Phase_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PhaseB_pin), Phase_change, CHANGE);

  encoderGauge.initDisplay();
}

void Phase_change() {
  rotaryEncoder.UpdateState();
}

void loop() {
  int32_t position = rotaryEncoder.GetPosition();
  rotaryEncoder.PositionHasChanged = false;

  encoderGauge.setPosition(position);

  rotaryEncoder.UpdateSwitch();
  if (rotaryEncoder.SwitchHasChanged) {
    rotaryEncoder.SwitchHasChanged = false;
    // Do something only after switch was pressed and released:
    if (!rotaryEncoder.SwitchState) {
      if (position != 0.0) {
        encoderGauge.setNumPositions(position);
        Serial.println(encoderGauge.getNumPositions());
      }
    }
  }
}
