#include "ArduinoTransmitter_internals.h"

using namespace TransmitterInternals;

// ---

UART_Transmitter::~UART_Transmitter()
{
  if (SoftwareSerial_ != NULL)
  {
    // I've made destructor in <SoftwareSerial.h> virtual to avoid compiler warning.
    delete SoftwareSerial_;
    SoftwareSerial_ = NULL;
  }
}

void UART_Transmitter::ConnectTo(uint8_t Rx_Pin, uint8_t Tx_Pin, uint32_t Baud)
{
  if (SoftwareSerial_ != NULL)
  {
    delete SoftwareSerial_;
    SoftwareSerial_ = NULL;
  }

  if (SoftwareSerial_ == NULL)
  {
    SoftwareSerial_ = new SoftwareSerial(Rx_Pin, Tx_Pin);
  }

  SoftwareSerial_->begin(Baud);
}

bool UART_Transmitter::IsConnected()
{
  return SoftwareSerial_->isListening();
}

uint16_t UART_Transmitter::GetNumBytesAvailable()
{
  return SoftwareSerial_->available();
};

uint8_t UART_Transmitter::PeekByte()
{
  return SoftwareSerial_->peek();
};

uint8_t UART_Transmitter::ReadByte()
{
  return SoftwareSerial_->read();
};

uint8_t UART_Transmitter::WriteByte(uint8_t b)
{
  return SoftwareSerial_->write(b);
};

// ---
