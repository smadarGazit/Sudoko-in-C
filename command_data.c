
#include "command_data.h"
#include "board.h"
#include "main_aux.h"
#include "errors.h"
#include "list.h"

ERROR get_command_data(Command_data **command_data, Command command, Set_params *set_params, Autofill_params *autofill_params,
                    Generate_params *generate_params, Guess_params *guess_params)
{
    Command_data* cmd_data = (Command_data*)malloc(sizeof(Command_data));
    if(!cmd_data)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    cmd_data->command = command;
    cmd_data->autofill_params = NULL;
    cmd_data->generate_params = NULL;
    cmd_data->set_params = NULL;
    cmd_data->guess_params = NULL;
    switch (cmd_data->command)
    {
        case AUTOFILL:
            cmd_data->autofill_params = autofill_params;
            break;
        case GENERATE:
            cmd_data->generate_params = generate_params;
            break;
        case SET:
            cmd_data->set_params = set_params;
            break;
        case GUESS:
            cmd_data->guess_params = guess_params;
            break;
        default:
            break;
    }
   
    *command_data = cmd_data;
    return SUCCESS;
}

void free_command_data(Command_data *command_data)
{
    /*----------------AUTOFILL---------------------*/
    switch(command_data->command)
    {
        case AUTOFILL:
            free_autofill_params(command_data->autofill_params);
            break;
        case GENERATE:
            free_generate_params(command_data->generate_params);
            break;
        case GUESS:
            free_guess_params(command_data->guess_params);
            break;
        case SET:
            free_set_params(command_data->set_params);
            break;
        default:
            break;
    }
    /*<<<<<<<-------------------------->>>>>>>>>*/
    /*at last, we can free the command_data itself*/
    free(command_data);
}

ERROR get_set_params(Set_params **set_params, Cell *old_cell, Cell *new_cell)
{
    Set_params *params;

    params = (Set_params*)malloc(sizeof(Set_params));
    if(!params)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    
    params->old_cell = (Cell *)malloc(sizeof(Cell));
    if(!params->old_cell)
    {
        free(params);
        return MEMORY_ALLOCATION_ERROR;
    }
    params->new_cell = (Cell *)malloc(sizeof(Cell));
    if(!params->new_cell)
    {
        free(params->old_cell);
        free(params);
        return MEMORY_ALLOCATION_ERROR;
    }
    
    copy_cell(old_cell, params->old_cell);
    copy_cell(new_cell, params->new_cell);

    *set_params = params;
    return SUCCESS;
}

ERROR get_autofill_params(Autofill_params **autofill_params, int **ind_changed, int ind_changed_length)
{
    Autofill_params *params = (Autofill_params*)malloc(sizeof(Autofill_params));
    if(!params)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    params->ind_to_del = ind_changed;
    params->ind_to_del_length = ind_changed_length;
    *autofill_params = params;
    return SUCCESS;
}

ERROR get_generate_params(Generate_params **generate_params)
{
    Generate_params* params = (Generate_params*)malloc(sizeof(Generate_params));
    if(!params)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    params->old_cells = NULL;
    params->old_sol = NULL;
    params->new_cells = NULL;
    params->new_sol = NULL;
    params->dimension = 0;
    *generate_params = params;
    return SUCCESS;
}

ERROR init_old_generate_params(Board *game_board, Generate_params **out_params)
{
    ERROR error;
    Cell **old_cells;
    int **old_sol;
    /*Generate_params *generate_params;*/

    error = get_generate_params(out_params);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    error = init_cells(&old_cells, game_board->num_of_rows, game_board->num_of_cols);
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        free(*out_params);
        return MEMORY_ALLOCATION_ERROR;
    }

    error = init_int_mat(&old_sol, game_board->num_of_rows, game_board->num_of_cols);
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        free_cells(old_cells, game_board->num_of_rows);
        free(*out_params);
        return MEMORY_ALLOCATION_ERROR;
    }

    error = copy_cells(game_board->cells, old_cells, game_board->num_of_rows, NULL);  
    if(error != SUCCESS)
    {
        free_cells(old_cells, game_board->num_of_rows);
        free_mat(old_sol, game_board->num_of_rows);
        free(*out_params);
        return error;
    }
    copy_int_mat(game_board->cur_sol, old_sol, game_board->num_of_rows, game_board->num_of_cols); 
    
    (*out_params)->old_cells = old_cells;
    (*out_params)->old_sol = old_sol;
    (*out_params)->dimension = game_board->num_of_rows;

    return SUCCESS;
}

/*called in generate after generating the new board...*/
ERROR init_new_generate_params(Board *game_board, Generate_params *params)
{
    ERROR error;
    Cell **new_cells;
    int **new_sol;

    error = init_cells(&new_cells, game_board->num_of_rows, game_board->num_of_cols) == MEMORY_ALLOCATION_ERROR;
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    error = init_int_mat(&new_sol, game_board->num_of_rows, game_board->num_of_cols);
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        free_cells(new_cells, game_board->num_of_rows);
        return MEMORY_ALLOCATION_ERROR;
    }

    copy_cells(game_board->cells, new_cells, game_board->num_of_rows, NULL);
    copy_int_mat(game_board->cur_sol, new_sol, game_board->num_of_rows, game_board->num_of_cols);
    params->new_cells = new_cells;
    params->new_sol = new_sol;
    return SUCCESS;
}
/*TODO*/
ERROR get_guess_params(Guess_params **guess_params, int main_length)
{
    Cell **new_cells, **old_cells;
    Guess_params* params = (Guess_params*)malloc(sizeof(Guess_params));
    ERROR error; 

    if(!params)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    *guess_params = params;
    error = init_cells(&new_cells, main_length, main_length) == MEMORY_ALLOCATION_ERROR;
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        free(params);
        return MEMORY_ALLOCATION_ERROR;
    }
    error = init_cells(&old_cells, main_length, main_length) == MEMORY_ALLOCATION_ERROR;
    if(error == MEMORY_ALLOCATION_ERROR)
    {
        free_cells(new_cells, main_length);
        free(params);
        return MEMORY_ALLOCATION_ERROR;
    }
    params->new_cells = new_cells;
    params->old_cells = old_cells;
    params->dimension = main_length;

    return SUCCESS;
}

void free_autofill_params(Autofill_params *autofill_params)
{
    int i;
    for(i = 0; i < 2; ++i)
    {
        free(autofill_params->ind_to_del[i]);
    }
        
    free(autofill_params->ind_to_del);

    /*<<<<<<<>>>>>>>>>*/
    free(autofill_params);
}

void free_generate_params(Generate_params *generate_params)
{

    if(generate_params->old_cells)
    {
        free_cells(generate_params->old_cells, generate_params->dimension);
    }
    if(generate_params->old_sol)
    {
        free_mat(generate_params->old_sol, generate_params->dimension);
    }
    
    if(generate_params->new_cells)
    {
        free_cells(generate_params->new_cells, generate_params->dimension);
    }
    
    if(generate_params->new_sol)
    {
        free_mat(generate_params->new_sol, generate_params->dimension);
    }

    /*<<<<<<<anyway!>>>>>>>>>*/
    free(generate_params);
}

void free_guess_params(Guess_params *guess_params)
{

    if(guess_params->old_cells)
    {
        free_cells(guess_params->old_cells, guess_params->dimension);
    }
  
    if(guess_params->new_cells)
    {
        free_cells(guess_params->new_cells, guess_params->dimension);
    }

    /*<<<<<<<anyway!>>>>>>>>>*/
    free(guess_params);
}

void free_set_params(Set_params *params)
{
    free(params->old_cell);
    free(params->new_cell);
    free(params);
}

ERROR add_node_autofill(Board *game_board, int **ind_changed, int changes_counter)
{
    ERROR res;
    Command_data *command_data;
    Autofill_params *autofill_params;
    List_node *new_node;

    res = get_autofill_params(&autofill_params, ind_changed, changes_counter);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    res = get_command_data(&command_data, AUTOFILL, NULL, autofill_params, NULL, NULL);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        free_autofill_params(autofill_params);
        return MEMORY_ALLOCATION_ERROR;
    }
    res = get_new_node(&new_node, command_data);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        free_command_data(command_data);
        return res;
    }
    add_node_after_cur(game_board->moves_list, game_board->cur_node, new_node);

    game_board->cur_node = new_node;

    return SUCCESS;
}

ERROR add_set_node(Board *game_board, Cell *old_cell, Cell *new_cell)
{
    Set_params *set_params;
    List_node *new_node;
    Command_data *command_data;
    ERROR error;

    error = get_set_params(&set_params, old_cell, new_cell);
    if(error != SUCCESS)
    {
        return error;
    }

    error = get_command_data(&command_data, SET, set_params, NULL, NULL, NULL);
    if(error != SUCCESS)
    {
        free_set_params(set_params);
        return error;
    }

    error = get_new_node(&new_node, command_data);
    if(error != SUCCESS)
    {
        free_command_data(command_data);
        return error;
    }

    add_node_to_tail(game_board->moves_list, new_node);
    game_board->cur_node = new_node;
    return SUCCESS;
}

ERROR get_generate_node(Board *game_board, Generate_params *params, List_node **new_node)
{
    ERROR error;
    Command_data *command_data;

    error = init_new_generate_params(game_board, params);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    error = get_command_data(&command_data, GENERATE, NULL, NULL, params, NULL);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        free_generate_params(params);
        return MEMORY_ALLOCATION_ERROR;
    }
    error = get_new_node(new_node, command_data);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        free_command_data(command_data);
        return MEMORY_ALLOCATION_ERROR;
    }

    return SUCCESS;
}

ERROR add_generate_node(Board *game_board, Generate_params *params)
{
    List_node *new_node;
    ERROR error;

    error = get_generate_node(game_board, params, &new_node);
    if(error != SUCCESS)
    {
        return error;
    }

    add_node_after_cur(game_board->moves_list, game_board->cur_node, new_node);
    game_board->cur_node = new_node;
    return SUCCESS;
}

ERROR add_guess_node(Board *game_board, Cell **old_cells)
{
    Guess_params *params;
    Command_data *command_data;
    List_node *new_node;

    params = calloc(1, sizeof(params));
    command_data = calloc(1,sizeof(command_data));


    get_guess_params(&params, game_board->num_of_rows);
    copy_cells(old_cells, params->old_cells, game_board->num_of_rows, NULL);
    copy_cells(game_board->cells, params->new_cells, game_board->num_of_rows, NULL);

    get_command_data(&command_data, GUESS, NULL, NULL, NULL, params);
    get_new_node(&new_node, command_data);
    add_node_after_cur(game_board->moves_list, game_board->cur_node, new_node);
    game_board->cur_node = new_node;
    return SUCCESS;
}
