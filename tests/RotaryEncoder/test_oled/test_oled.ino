// Rotary encoder demo for ST7789 OLED display

/*
  Status: done
  Version: 1.0
  Last mod.: 2021-07-18
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

RotaryEncoder RotaryEncoder(PhaseA_pin, PhaseB_pin, Switch_pin);
Adafruit_ST7789 Display = Adafruit_ST7789(CS_pin, DC_pin, RST_pin);

void setup() {
  Serial.begin(9600);

  RotaryEncoder.Debounce_ms = Debounce_ms;

  attachInterrupt(digitalPinToInterrupt(PhaseA_pin), Phase_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PhaseB_pin), Phase_change, CHANGE);
  
  initDisplay();  
}

void Phase_change() {
  RotaryEncoder.UpdateState();
}

bool PrevSwitchState_printed = false;

void loop() {
  int32_t Position = RotaryEncoder.GetPosition();
    
  drawState(Position);
  RotaryEncoder.PositionHasChanged = false;

  RotaryEncoder.UpdateSwitch();
  if (RotaryEncoder.SwitchHasChanged) {
    bool NeedSwitchRedisplay = (RotaryEncoder.SwitchState != PrevSwitchState_printed);
    if (NeedSwitchRedisplay) {
      Serial.print("Switch ");
      Serial.println(RotaryEncoder.SwitchState);
      PrevSwitchState_printed = RotaryEncoder.SwitchState;
    }
    RotaryEncoder.SwitchHasChanged = false;
  }
}

void initDisplay() {
  Display.init(240, 240);
  Display.setRotation(2);
  Display.fillScreen(ST77XX_BLACK);
  Display.setTextSize(2);
  drawBackground();
}

void drawBackground() {
  uint16_t x, y, radius, color;
  x = Display.width() / 2;
  y = Display.height() / 2;
  
  radius = 120;
  color = ST77XX_BLUE;
  for (uint8_t i = 0; i < 4; ++i) {
    Display.drawCircle(x, y, radius - i, color);
  }
}

void drawState(int32_t Position) {
  static int32_t PrevPosition = -1;
  if (Position == PrevPosition)
    return; 
 
  /*  
  Serial.print("Position ");
  Serial.println(Position);
  */
  
  const uint16_t 
    x = 120 - 4,
    y = 120 - 4;
    
  Display.setTextColor(ST77XX_BLACK);
  Display.setCursor(x, y);
  Display.print(PrevPosition);

  Display.setTextColor(ST77XX_WHITE);
  Display.setCursor(x, y);
  Display.print(Position);

  drawPointer(PrevPosition, ST77XX_BLACK);    
  drawPointer(Position, ST77XX_YELLOW);    

  PrevPosition = Position;
}

void drawPointer(int32_t position, int16_t color) {
  static float scale = 2 * PI / 80.0;
  float circleX, circleY;
  uint16_t radius = 12;
  circleX = 120 + 100 * sin(position * scale);
  circleY = 120 - 100 * cos(position * scale);
  
  Display.fillCircle(circleX, circleY, radius, color); 

}