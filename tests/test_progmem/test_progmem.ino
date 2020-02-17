#include <Flash.h>

FLASH_STRING(description, "\"Flower friend\" gardening system\n");
FLASH_STRING(version, "5.2.1\n");

void setup()
{
  Serial.begin(9600);
  description.print(Serial);
  version.print(Serial);
}

void loop()
{
}

/*
const char
 description[] = "\"Flower friend\" gardening system",
 version[] = "5.2.1";

void setup()
{
  Serial.begin(9600);
  Serial.println(description);
  Serial.println(version);
}

void loop()
{
}
*/
