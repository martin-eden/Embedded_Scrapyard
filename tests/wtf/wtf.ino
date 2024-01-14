// Test of C++ pecularities

/*
*/

#include <me_Types.h>

// Array
const TUint_1 TArray_Size = 4;
typedef TUint_1 TArray[TArray_Size];

// Just a generic buffer for text output:
const TUint_2 Message_MaxLength = 2 * 256;
TChar Message [Message_MaxLength];

// Serial speed (baud):
const TUint_4 SerialSpeed = 115200;

void FillArray(TArray Array)
{
  Array[0] = 0;
  Array[1] = 1;
  Array[2] = 2;
  Array[3] = 3;
}

void PrintArray(TArray Array)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%03d.%03d.%03d.%03d",
    Array[0],
    Array[1],
    Array[2],
    Array[3]
  );

  Serial.println(Message);
}

void setup()
{
  {
    const TUint_2 SerialSetupDelay_Ms = 500;

    Serial.begin(SerialSpeed);
    delay(SerialSetupDelay_Ms);
    Serial.println();
    Serial.println();
  }

  TArray Array;

  Array[0] = 4;
  Array[1] = 3;
  Array[2] = 2;
  Array[3] = 1;
  PrintArray(Array);

  FillArray(Array);
  PrintArray(Array);
}


void loop()
{
}
