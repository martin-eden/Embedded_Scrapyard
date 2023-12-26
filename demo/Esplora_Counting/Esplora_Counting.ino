// Counting code demo on Esplora.

/*
  That's a hello-world type program. Value in the code design.
*/

/*
  Status: done
  Version: 1
  Last mod.: 2023-12-26
*/

/*
  Board: Esplora (Wemos D1)
*/

const
  uint32_t SerialSpeed = 115200;

const uint16_t Message_MaxLength = 500;
char Message[Message_MaxLength];

const uint32_t Iteration_Duration_Ms = 3000;
const uint32_t Iterations_Limit = 5;
static uint32_t Iterations_NumDone = 0;

void setup()
{
  Serial.begin(SerialSpeed);
  PrintGreeting();
}

void loop()
{
  if (Iterations_NumDone == 0)
  {
    PrintGreeting();
  }

  delay(Iteration_Duration_Ms);

  Iterations_NumDone = Iterations_NumDone + 1;

  snprintf(Message, Message_MaxLength, "Iterations done: %u", Iterations_NumDone);
  Serial.println(Message);

  if (Iterations_NumDone > Iterations_Limit)
  {
    // WTF?!. Somehow we overshoot limit. Okay, set it to limit.
    Serial.println("@ Somehow we've overshot our limit. Setting counter to limit.");

    Iterations_NumDone = Iterations_Limit;
  }

  if (Iterations_NumDone == Iterations_Limit)
  {
    Serial.println("@ Iterations limit reached. Setting counter to zero.");

    Iterations_NumDone = 0;
  }
}

void PrintGreeting()
{
  static const char GreetingMessageFmt[] PROGMEM =
    "Hello-world tier demo for Esplora. Value in the code.\n"
    "\n"
    "We will do %u iterations (each iteration is %u ms) and start again.\n";

  snprintf(Message, Message_MaxLength, GreetingMessageFmt, Iterations_Limit, Iteration_Duration_Ms);

  Serial.println();
  Serial.println();
  Serial.println(Message);
}

/*
  2023-12-26
*/
