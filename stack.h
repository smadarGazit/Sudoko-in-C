#ifndef STACK_H
#define STACK_H

#include "bool.h"
#include "cell.h"
#include "errors.h"

# define EMPTY 0

typedef struct elem
{
    Cell* cell;
    struct elem *next;
} Elem;


typedef struct Stack{
    int cnt;
    Elem *top;
}Stack;

void init_stack(Stack *stk); /*constuctor*/

ERROR push(Cell *cell, Stack *stk);   
void pop(Stack *stk, Cell **cell);
BOOL stack_is_empty(Stack *stk);

#endif
