// Rotary encoder demo for ST7789 OLED display

/*
  Status: done
  Version: 2.1
  Last mod.: 2021-09-26
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
  int32_t getPosition();
  void setPosition(int32_t _position);
  void initDisplay();
private:
  int32_t numPositions;
  int32_t position;
  int32_t prevPosition;
  bool forceRedraw;
  void erasePointer();
  void drawPointer();
  void erasePosition();
  void drawPosition();
  void eraseNumPositions();
  void drawNumPositions();
  void drawPointer_raw(int32_t _position, int16_t _color);
  void drawPositionText_raw(int32_t _position, int16_t _color);
  void drawNumPositionsText_raw(int32_t _numPositions, int16_t _color);
  void drawBackground();
};

EncoderGauge::EncoderGauge() {
  numPositions = 80;
  prevPosition = 0;
  position = 0;
  forceRedraw = false;
}

const uint16_t
  displayWidth = 240,
  displayHeight = 240;

void EncoderGauge::initDisplay() {
  display.init(displayWidth, displayHeight);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(2);

  drawBackground();

  forceRedraw = true;
  setPosition(position);
  forceRedraw = false;

  drawNumPositions();
}

int32_t EncoderGauge::getNumPositions() {
  return numPositions;
}

void EncoderGauge::setNumPositions(int32_t _numPositions) {
  if (_numPositions != 0) {
    erasePointer();
    eraseNumPositions();
    numPositions = _numPositions;
    drawBackground();
    drawNumPositions();
    drawPointer();
  }
}

int32_t EncoderGauge::getPosition() {
  return position;
}

void EncoderGauge::setPosition(int32_t _position) {
  if (!forceRedraw && (_position == position) && (position == prevPosition))
    return;

  position = _position;

  erasePointer();
  drawPointer();

  erasePosition();
  drawPosition();

  prevPosition = position;
}

void EncoderGauge::erasePointer() {
  drawPointer_raw(prevPosition, ST77XX_BLACK);
}

void EncoderGauge::drawPointer() {
  drawPointer_raw(position, ST77XX_YELLOW);
}

void EncoderGauge::erasePosition() {
  drawPositionText_raw(prevPosition, ST77XX_BLACK);
}

void EncoderGauge::drawPosition() {
  drawPositionText_raw(position, ST77XX_WHITE);
}

void EncoderGauge::eraseNumPositions() {
  drawNumPositionsText_raw(numPositions, ST77XX_BLACK);
}

void EncoderGauge::drawNumPositions() {
  drawNumPositionsText_raw(numPositions, ST77XX_WHITE);
}

void EncoderGauge::drawBackground() {
  uint16_t x, y, radius, color;

  x = (displayWidth - 1) / 2;
  y = (displayHeight - 1) / 2;
  radius = min(displayWidth - 1, displayHeight - 1) / 2;
  color = ST77XX_BLUE;

  display.fillCircle(x, y, radius, color);
}

void EncoderGauge::drawPointer_raw(int32_t _position, int16_t _color) {
  const int16_t
    markLongLen = 24,
    markShortLen = 12,
    centerX = (displayWidth - 1) / 2,
    centerY = (displayHeight - 1) / 2,
    markOuterRadius = displayWidth / 2 - 2;

  _position = _position % numPositions;
  if (_position < 0)
    _position += abs(numPositions);

  bool isLongMark = (_position % 4 == 0);

  float scale = 2 * PI / numPositions;

  float x = cos(_position * scale - PI / 2);
  float y = sin(_position * scale - PI / 2);

  float x1, y1, x2, y2;

  x2 = centerX + x * markOuterRadius;
  y2 = centerY + y * markOuterRadius;
  if (isLongMark) {
    x1 = centerX + x * (markOuterRadius - markLongLen);
    y1 = centerY + y * (markOuterRadius - markLongLen);
  }
  else {
    x1 = centerX + x * (markOuterRadius - markShortLen);
    y1 = centerY + y * (markOuterRadius - markShortLen);
  }

  display.drawLine(x1, y1, x2, y2, _color);
}

void EncoderGauge::drawPositionText_raw(int32_t _position, int16_t _color) {
  const uint16_t
    x = 0,
    y = 0;

  display.setTextColor(_color);
  display.setCursor(x, y);
  display.print(_position);
}

void EncoderGauge::drawNumPositionsText_raw(int32_t _numPositions, int16_t _color) {
  const uint16_t
    x = displayWidth - 4 * 12,
    y = 0;

  display.setTextColor(_color);
  display.setCursor(x, y);
  display.print(_numPositions);
}

EncoderGauge encoderGauge;
RotaryEncoder rotaryEncoder(PhaseA_pin, PhaseB_pin, Switch_pin);

void Phase_change() {
  rotaryEncoder.UpdateState();
}

void setup() {
  Serial.begin(9600);

  rotaryEncoder.Debounce_ms = Debounce_ms;

  attachInterrupt(digitalPinToInterrupt(PhaseA_pin), Phase_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PhaseB_pin), Phase_change, CHANGE);

  encoderGauge.initDisplay();
}

void loop() {
  int32_t position = rotaryEncoder.GetPosition();

  if (rotaryEncoder.PositionHasChanged) {
    rotaryEncoder.PositionHasChanged = false;
    encoderGauge.setPosition(position);
  }

  rotaryEncoder.UpdateSwitch();
  if (rotaryEncoder.SwitchHasChanged) {
    rotaryEncoder.SwitchHasChanged = false;
    // Do something only after switch was pressed and released:
    if (!rotaryEncoder.SwitchState) {
      if (position != 0.0) {
        encoderGauge.setNumPositions(position);
        encoderGauge.setPosition(0);
        rotaryEncoder.SetPosition(0);
      }
    }
  }
}
