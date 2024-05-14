// [me_List] test/debug

/*
  Last mod.: 2024-05-14
*/

#include <me_List.h>

#include <me_InstallStandardStreams.h>
#include <me_UartSpeeds.h>
#include <me_Types.h>

using namespace me_List;

void setup()
{
  InstallStandardStreams();
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);

  delay(1500);

  printf("[me_List] Hello there.\n");

  // TStack List;
  TQueue List;

  for (TUint_2 Counter = 1; Counter <= 4; ++Counter)
  {
    TNodePtr NodePtr;

    TBool SpawnResult = SpawnNode(&NodePtr);

    if (!SpawnResult)
      break;

    NodePtr->DataPtr = Counter;

    List.Add(NodePtr);
  }

  PrintList(List.HeadPtr);
  KillList(List.HeadPtr);
}

void loop()
{
}

TBool PrintList(TNodePtr StartPtr)
{
  return Traverse(StartPtr, PrintNode);
}

TBool PrintNode(TNodePtr NodePtr)
{
  printf(
    "[0x%04X](DataPtr: 0x%04X, NextNode: 0x%04X)\n",
    (TUint_2) NodePtr,
    NodePtr->DataPtr,
    (TUint_2) NodePtr->NextNode
  );

  return true;
}

/*
  2024-05-14
*/
