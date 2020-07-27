#include "typedefs.h"
#include <stdio.h>
#include <stdlib.h>
#include "cell.h"
#include "errors.h"

extern int MAX_NUMxx;


void get_new_cell(Cell *out_new_cell, int val, int x, int y, BOOL is_set_by_user, BOOL is_fixed, 
                  BOOL is_valid)
{
    int i;
    
    out_new_cell->value = val;
    out_new_cell->coordinates.x = x;
    out_new_cell->coordinates.y = y;
    out_new_cell->is_fixed = is_fixed;
    out_new_cell->is_set_by_user = is_set_by_user;
    out_new_cell->is_valid = is_valid;
    out_new_cell->number_of_options = 0;
    for (i = 0; i < MAX_NUMxx; ++i)
    {
        out_new_cell->options[i] = FALSE;
    }
    out_new_cell->is_valid = TRUE;
}

void copy_cell(Cell *source, Cell *target)
{
    target->coordinates.x = source->coordinates.x;
    target->coordinates.y = source->coordinates.y;
    target->is_fixed = source->is_fixed;
    target->is_set_by_user = source->is_set_by_user;
    target->is_valid = source->is_valid;
    target->value = source->value;

}