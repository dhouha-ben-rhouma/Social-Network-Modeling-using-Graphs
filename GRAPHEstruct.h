#ifndef GRAPHESTRUCT_H_INCLUDED
#define GRAPHESTRUCT_H_INCLUDED

#include "ELTUSER.h"

typedef struct structNode
{
    ELEMENT user;
    int weight;
    struct structNode *next;
} structNode, *Node;

typedef struct
{
    int v;
    ELEMENT * utilisateurs;//Tableau dynamique (via malloc)
    Node * adjList;//Tableau dynamique (via malloc)
} *graphePondere;

#endif // GRAPHESTRUCT_H_INCLUDED
