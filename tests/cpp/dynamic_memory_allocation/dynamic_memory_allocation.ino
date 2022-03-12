const uint16_t BlockSize = 256;

uint16_t * CreateBuffer()
{
  uint16_t * ptr;

  ptr = malloc(BlockSize);
  if (!ptr)
    return 0;

  Serial.println((uint16_t) ptr, HEX);

  uint8_t addon = random(256);
  for (uint16_t i = 0; i < BlockSize; ++i)
  {
    *(ptr + i) = i + addon;
  }

  return ptr;
}

void FreeBuffer(uint16_t * ptr)
{
  if (ptr)
    free(ptr);
}

void PrintBuffer(uint16_t * ptr)
{
  PrintBuffer(ptr, BlockSize);
}

void PrintBuffer(uint16_t * ptr, uint16_t BlockSize)
{
  Serial.print("ptr: ");
  Serial.print((uint16_t) ptr, HEX);
  Serial.print("\n");

  if (!ptr)
    return;

  for (uint16_t i = 0; i < BlockSize; ++i)
  {
    Serial.print((uint8_t) *(ptr + i), HEX);
    if ((i + 1) % 16 == 0)
      Serial.print("\n");
    else
      Serial.print(" ");
  }
  Serial.print("\n");
}


void setup()
{
  Serial.begin(115200);

  pinMode(A0, INPUT);
  srand(analogRead(A0));

  Serial.print("Memory routines test.\n");

  uint16_t * ptr1 = CreateBuffer();
  PrintBuffer(ptr1);

  uint16_t * ptr2 = CreateBuffer();
  PrintBuffer(ptr2);

  FreeBuffer(ptr2);
  ptr2 = NULL;
  FreeBuffer(ptr1);
  ptr1 = NULL;
  PrintBuffer(ptr1);
  PrintBuffer(ptr2);

  PrintBuffer(16, 2048 - 16);

  Serial.print("Done.\n");
}

void loop()
{
}
