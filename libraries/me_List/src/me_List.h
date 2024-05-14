// Singly linked list, stack and queue containers

/*
  Author: Martin Eden
  Last mod.: 2024-05-14
*/

#pragma once

#include <me_Types.h>

namespace me_List
{
  typedef TUint_2 TPtr;

  // List node
  struct TNode
  {
    TPtr DataPtr;
    struct TNode * NextNode;
  };

  // Pointer to node
  typedef TNode * TNodePtr;

  // Signature of list node handler function
  typedef TBool (*THandler)(TNodePtr);

  // Allocate memory for list node
  TBool SpawnNode(TNodePtr * NodePtr);
  // Free memory of list node
  TBool KillNode(TNodePtr NodePtr);
  // Iterate over list calling handler for every node
  TBool Traverse(TNodePtr StartPtr, THandler Handler);
  // Shortcut for Traverse(.., KillNode)
  TBool KillList(TNodePtr StartPtr);

  // Stack container
  struct TStack
  {
    TStack() : HeadPtr(0) {};

    TNodePtr HeadPtr;
    void Add(TNodePtr NodePtr);
  };

  // Queue container
  struct TQueue
  {
    TQueue() : HeadPtr(0), TailPtr(0) {};

    TNodePtr HeadPtr;
    TNodePtr TailPtr;
    void Add(TNodePtr NodePtr);
  };
}

/*
  2024-05-14
*/
