// Test of digital signal recorder with DHT11 hygrometer

/*
  Status: works
  Version: 1.0
  Last mod.: 2022-03-06
*/

#include <me_DigitalSignalRecorder.h>
#include <me_DHT11.h>

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

me_DigitalSignalRecorder DSR;

class me_DHT11_explore : me_DHT11
{
  public:
    me_DHT11_explore(uint8_t aSignalPin) : me_DHT11(aSignalPin) {}

    void EmitRequest()
    {
      me_DHT11::EmitRequest();
    }
};

me_DHT11_explore Hygrometer(SignalPin);

void setup()
{
  Serial.begin(115200);
  Serial.println("Digital signal recorder test for DHT11 hygrometer.");

  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
}

void OnSignalChange()
{
  DSR.Add(micros(), digitalRead(SignalPin));
}

void loop()
{
  Hygrometer.EmitRequest();
  delay(5000);
  if (DSR.HasEvents())
  {
    DSR_PrintJSON(&DSR);
    DSR.Clear();
  }
}
