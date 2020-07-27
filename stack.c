#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "stack.h"
#include "cell.h"
#include "errors.h"

void init_stack(Stack *stk)
{
    stk->cnt = 0;
    stk->top = NULL;
}

ERROR push(Cell *cell, Stack *stk)
{
    Elem *p = NULL;
    p = malloc(sizeof(Elem));
    
    if(!p)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    
    p->cell = cell;
    p->next = stk->top;
    stk->top = p;
    ++stk->cnt;
    return SUCCESS;
}

void pop(Stack *stk, Cell **out_cell)
{
    Cell* c;
    Elem *p = NULL;
    
    c = stk->top->cell;
    p=stk->top;

    stk->top = stk->top->next;
    --stk->cnt;
    free(p);
    *out_cell = c;
}

BOOL stack_is_empty(Stack *stk)
{
    BOOL res = (stk->cnt == 0);
    return res;
}