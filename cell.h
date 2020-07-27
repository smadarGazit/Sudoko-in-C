#ifndef CELL_H
#define CELL_H
#include <stdio.h>
#include "typedefs.h"
#include "bool.h"
#include "point.h"
#include "commands.h"
#include "errors.h"
#include "list_node.h"


struct Cell
{
Point coordinates; /*a point that holds the coordinate col,row of the cell in its board cell matrix*/
BOOL is_fixed;
BOOL is_set_by_user;
int value;
BOOL is_valid;
BOOL options[50];/*relevent only to get_sol_inner_rand(),
                       *after create_cell_options runs it holds the options for the cell*/
int number_of_options;/*relevent only to get_sol_inner_rand(),
                        TODO: erase this before handing*/
};

/*constructor*/
void get_new_cell(Cell *out_new_cell, int val, int x, int y, BOOL is_set_by_user, BOOL is_fixed, 
                  BOOL is_valid);
void copy_cell(Cell *source, Cell *target);

#endif