#ifndef COMMAND_DATA_H
#define COMMAND_DATA_H

#include <stdlib.h>
#include "typedefs.h"
#include "commands.h"
#include "errors.h"
#include "bool.h"


struct autofill
{
    int **ind_to_del;      /*for utofill - autofill will init it to a good size and fill it*/
    int ind_to_del_length; /*for autofill, the length of the 2nd dimension (how man cols.. 2 rows olways,
                              0 - rows, 1 - cols)*/
};

struct set
{
    Cell *old_cell;
    Cell *new_cell;
};

struct generate
{
    Cell **old_cells; 
    int **old_sol;

    Cell **new_cells; 
    int **new_sol;
    int dimension;
};

struct guess
{
    Cell **old_cells; 
    Cell **new_cells;
    int dimension;
};

/*each node holds data relevent to one of the 4 funcs*/
struct command_data
{
    Command command;
    Autofill_params *autofill_params;
    Guess_params *guess_params;
    Set_params *set_params;
    Generate_params *generate_params;
};

ERROR get_command_data(Command_data **command_data, Command command, Set_params *set_params, Autofill_params *autofill_params,
                       Generate_params *generate_params, Guess_params *guess_params);

void free_command_data(Command_data *command_data);

ERROR get_set_params(Set_params **set_params, Cell *old_cell, Cell *new_cell);

ERROR get_autofill_params(Autofill_params **autofill_params, int **ind_changed, int ind_changed_length);



ERROR init_old_generate_params(Board *game_board, Generate_params **params);

ERROR init_new_generate_params(Board *game_board, Generate_params *params);

ERROR get_guess_params(Guess_params **guess_params, int main_length);

void free_autofill_params(Autofill_params *autofill_params);

void free_generate_params(Generate_params *generate_params);

void free_guess_params(Guess_params *guess_params);

void free_set_params(Set_params *params);

ERROR add_node_autofill(Board *game_board, int **ind_changed, int changes_counter);

ERROR add_set_node(Board *game_board, Cell *old_cell, Cell *new_cell);

ERROR get_generate_node(Board *game_board, Generate_params *params, List_node **new_node);

ERROR add_generate_node(Board *game_board, Generate_params *params);

ERROR add_guess_node(Board *game_board, Cell **backup);

#endif