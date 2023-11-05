#include <Arduino.h>

#include <SoftwareSerial.h>

namespace TransmitterInternals
{
  class UART_Transmitter
  {
    public:
      ~UART_Transmitter();

      void ConnectTo(uint8_t Rx_Pin, uint8_t Tx_Pin, uint32_t Baud);

      bool IsConnected();
      uint16_t GetNumBytesAvailable();

      uint8_t PeekByte();
      uint8_t ReadByte();
      uint8_t WriteByte(uint8_t b);

    private:
      SoftwareSerial * SoftwareSerial_ = NULL;
  };
};
