// Singly linked list structure plus stack and queue containers

/*
  Author: Martin Eden
  Last mod.: 2024-05-14
*/

#include "me_List.h"

#include <Arduino.h> // malloc(), free(), printf()
#include <me_Types.h>

using namespace me_List;

TBool me_List::SpawnNode(TNodePtr * NodePtr)
{
  *NodePtr = (TNodePtr) malloc(sizeof(TNode));

  TBool IsSpawned = (*NodePtr != 0);

  if (!IsSpawned)
    return false;

  (*NodePtr)->DataPtr = 0;
  (*NodePtr)->NextNode = 0;

  // printf("SpawnNode(0x%04X)\n", (TUint_2) *NodePtr);

  return true;
}

TBool me_List::KillNode(TNodePtr NodePtr)
{
  // printf("KillNode(0x%04X)\n", (TUint_2) NodePtr);

  if (NodePtr == 0)
    return false;

  free(NodePtr);

  return true;
}

TBool me_List::Traverse(TNodePtr StartPtr, THandler Handler)
{
  if (Handler == 0)
    return false;

  TNodePtr Cursor = StartPtr;
  while (Cursor != 0)
  {
    TBool HandlerResult;

    HandlerResult = Handler(Cursor);

    if (!HandlerResult)
      return false;

    Cursor = Cursor->NextNode;
  }

  return true;
}

TBool me_List::KillList(TNodePtr StartPtr)
{
  return Traverse(StartPtr, KillNode);
}

// --

void me_List::TStack::Add(TNodePtr NodePtr)
{
  NodePtr->NextNode = HeadPtr;
  HeadPtr = NodePtr;
}

// --

void me_List::TQueue::Add(TNodePtr NodePtr)
{
  if (HeadPtr == 0)
    HeadPtr = NodePtr;

  if (TailPtr != 0)
    TailPtr->NextNode = NodePtr;

  TailPtr = NodePtr;
}

// --

/*
  2024-05-14
*/
