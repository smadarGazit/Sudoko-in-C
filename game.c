#include "game.h"

#include "board.h"
#include "cell.h"
#include "point.h"
#include "commands.h"
#include "main_aux.h"
#include "modes.h"
#include "errors.h"
#include "command_data.h"
#include "move.h"
#include "list_node.h"
#include "list.h"
#include "UI.h"
#include "file_handler.h"
#include "ilp_solver.h"
#include "commands.h"
#include "file_handler.h"


/*@brief - take the input from turn_input, that holds the command and the inputs relevent to it,
    apply the command if it is availble.
 *@returns the suitabale Error if ocurred in one of the the commands,
 *or COMMAND_UNAVAILABLE if the command that was chosen is no longer available (==board is finished)
  or SUCCESS if everything went fine
 *@param game_board is a pointer of the board
 *@param input arr is the out array of turn_input, that holds the inputs from the user*/
ERROR command_exe(Board *game_board, Move *user_move)
{
    int x, y, value;
    double threshold;
    char *path;

    typedef ERROR (*command_func_t)(Board *, int, int, int, double, char *);
    static command_func_t command_funcs[] = {exit_game, set, validate, hint, solve, edit, mark_errors, guess, generate, undo, redo, save, guess_hint, num_solutions, print_board_command, autofill_command, reset};


    Command command;
    command_func_t chosen_command;

    command = user_move->COMMAND;
    x = user_move->param1.value - 1;
    y = user_move->param2.value - 1;
    value = user_move->param3.value;
    threshold = user_move->extraParam;
    path = user_move->path;


    chosen_command = command_funcs[command]; 

    return  (*chosen_command)(game_board, x, y, value, threshold, path);

}





/*@brief - only return EXIT_GAME to main
 *@param game_board,x,y,val are not used in this method, and are here to make command_exe more elegent
 *returns EXIT_GAME, which the main interperts that it now need to exit*/
ERROR exit_game(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    (void)x;
    (void)y;
    (void)value;
    (void)game_board;
    (void)threshold;
    (void)path;
    return EXIT_GAME;
}

ERROR solve_set(Board *game_board, int col, int row, int val)

{
    int new_val, old_val;
    Cell *cur_cell;
    Cell *old_cell;
    ERROR error;

    cur_cell = &game_board->cells[row][col];
    new_val = val;
    old_val = cur_cell->value;

    if (cur_cell->is_fixed)
    {
        return SET_CANT_CHANGE_FIXED_CELL;
    }

    old_cell = (Cell *)malloc(sizeof(Cell));
    if (!old_cell)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    copy_cell(cur_cell, old_cell);

    /*surely, it ain't fixed now*/
    if (new_val == 0)
    {
        cur_cell->value = 0;
        cur_cell->is_fixed = FALSE;
        cur_cell->is_set_by_user = FALSE;
        cur_cell->is_valid = TRUE;
        error = add_set_node(game_board, old_cell, cur_cell);
        mark_visble_errors(game_board);
        free(old_cell);
        if (error != SUCCESS)
        {
            return error;
        }
        return SUCCESS;
    }
    /*this case, we add it to the node but we don't need to change anything*/
    if (old_val == new_val)
    {
        error = add_set_node(game_board, old_cell, cur_cell);
        free(old_cell);
        if (error != SUCCESS)
        {
            return error;
        }
        return SUCCESS;
    }

    cur_cell->value = new_val;
    /*asks if clashes only with fixed_cell*/
    if (clash_with_fixed_cell(game_board, cur_cell))
    {
        /*this case, nothing to undo-redo*/
        cur_cell->value = old_val;
        free(old_cell);
        return SET_CLASH_WITH_FIXED_CELL;
    }
    /*so the value is good and we can place it!*/

    cur_cell->is_set_by_user = TRUE;
    mark_visble_errors(game_board);
    error = add_set_node(game_board, old_cell, cur_cell);

    free(old_cell);

    if (error != SUCCESS)
    {
        return error;
    }


    if (is_board_finished(game_board))
    {
        if (is_board_erroneus(game_board))
        {
            return PUZZLE_FINISHED_BUT_ERRONEOUS;
        }
        else
        {
            return PUZZLE_SOLVED;
        }

    }
    return SUCCESS;
}

ERROR edit_set(Board *game_board, int col, int row, int val)
{
    int new_val;
    Cell *cur_cell;
    Cell *old_cell;
    ERROR error;

    old_cell = (Cell *)malloc(sizeof(Cell));
    if (!old_cell)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    cur_cell = &game_board->cells[row][col];
    copy_cell(cur_cell, old_cell);
    new_val = val;

    if (new_val == 0)
    {
        cur_cell->value = 0;
        cur_cell->is_fixed = FALSE;
        cur_cell->is_set_by_user = FALSE;
        cur_cell->is_valid = TRUE;
        error = add_set_node(game_board, old_cell, cur_cell);
        mark_visble_errors(game_board);
        free(old_cell);
        if (error != SUCCESS)
        {
            return error;
        }
        return SUCCESS;
    }

    cur_cell->value = new_val;
    cur_cell->is_fixed = FALSE;
    cur_cell->is_set_by_user = FALSE;

    mark_visble_errors(game_board);

    error = add_set_node(game_board, old_cell, cur_cell);

    free(old_cell);

    if (error != SUCCESS)
    {
        return error;
    }

    return SUCCESS;
}

/*  @brief sets val to cell in location x,y in game_board->cells.
 *  @param game_board is the board
 *  @param x is the col coordinate
 *  @param y is the row coordinate
 *  @param val is the value to be set in cell in coordinates x,y
 *  @return suitable error for invalid input or when trying to reach fixed cell, else SUCCESS*/
ERROR set(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    ERROR res;
    Cell *before, *after;
    (void)threshold;
    (void)path;



    if (game_board->mode == SOLVE_STATE)
    {
        res = solve_set(game_board, x, y, value);
        if(res == SUCCESS ||res == PUZZLE_SOLVED ||res == PUZZLE_FINISHED_BUT_ERRONEOUS)
        {
            before = game_board->cur_node->command_data->set_params->old_cell;
            after = game_board->cur_node->command_data->set_params->new_cell;
            print_set(before, after);
        }
        return res;
    }

    /*if(game_board->mode == EDIT_STATE*/
    else 
    {
        res = edit_set(game_board, x, y, value);
        if(res == SUCCESS)
        {
            before = game_board->cur_node->command_data->set_params->old_cell;
            after = game_board->cur_node->command_data->set_params->new_cell;
            print_set(before, after);
        }
        return res;
    }
}

/*@brief - validate that the current configuration of the board is solvable
 *@param game_board is a pointer for the board struct user currently play on
 *@param x,y,val are not used in this function, and are only for elegance of command_exe
 *@return UNSOLVABLE_BOARD is the current configuration on the board is unsolvable
 *@return SOLVABLE_BOARD it is solvable*/
ERROR validate(Board *game_board, int x, int y, int value, double threshold, char *path)

{
    ERROR error;
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;

    if (is_board_erroneus(game_board))

    {
        return NO_VALIDATION_FOR_ERRONEOUS_BOARD;
    }
    
    error = get_sol_ilp_lp(game_board, ILP,0.0,0,0,NULL,NULL);

    print_validation(error);
    return error;

}



/*@brief prints the value of the coordinates x,y from field cur_sol of the board
 * it could be a wrong hint, unless validate was run just before
 *@param val is not used and for the elegance of command_exe
 *@param game_board is a pointer for the board struct user currently play on
 *@param x,y are the col,row coordinates correspondingly
 *@returns SUCCESS always */

ERROR hint(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    ERROR error;
    int row, col;
    (void)value;
    (void)threshold;
    (void)path;
    row = y;
    col = x;

    if (is_board_erroneus(game_board))
    {
        return CANT_HINT_ERRONEOUS_BOARD;
    }

    if (game_board->cells[row][col].is_fixed)
    {
        return CANT_HINT_FIXED_CELL;
    }

    if (game_board->cells[row][col].value != 0)
    {
        return CANT_HINT_FULL_CELL;
    }

    error = get_sol_ilp_lp(game_board, ILP,0.0,0,0,NULL,NULL);

    if (error != SUCCESS)
    {
        return error;
    }

    print_hint(row+1,col+1,game_board->cur_sol[row][col]);
    return SUCCESS;
}

ERROR solve(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    ERROR error;
    char error_info[MAX_ERROR_LEN] = {0};
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;

    /*reusing memory for all boards, so we need to free inner allocations before starting to work with another board*/
    free_all_board_memory(game_board); 

    error = import_board(path, game_board, error_info);
    if (error != SUCCESS){
        print_file_error(error, error_info);
        free_all_board_memory(game_board);
        return READ_FILE_ERROR;
    }

    mark_visble_errors(game_board); 
    game_board->mode = SOLVE_STATE;

    return SUCCESS;
}

/*path is an optional param -> if no path, this func will get an empty string */
ERROR edit(Board *game_board, int x, int y, int value, double threshold, char *path)

{
    ERROR error;
    char error_info[MAX_ERROR_LEN] = {0};
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;


    /*reusing memory for all boards, so we need to free inner allocations before starting to work with another board*/
    free_all_board_memory(game_board);


    if (path[0] == '\0'){ /*open default board - 9X9 board*/ 
        new_board(game_board, 9,9, 3,3);        
    }
    else{
        error = import_board(path, game_board, error_info);
        if (error != SUCCESS){
            print_file_error(error, error_info);
            free_all_board_memory(game_board);
            return READ_FILE_ERROR;
        }
        mark_visble_errors(game_board); 
    }

    game_board->mode = EDIT_STATE;

    fix_all_cells(game_board, FALSE); /*make all cells NOT FIXED*/
    return error;
    
}

ERROR mark_errors(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;
    game_board->mark_errors = value;

    return SUCCESS;
}


ERROR guess(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    Cell **backup;
    ERROR error;
    (void)x;
    (void)y;
    (void)value;
    (void)path;

    if (is_board_erroneus(game_board))
    {
        return NO_GUESSES_FOR_ERRONEUS_BOARD;
    }
    error = init_cells(&backup, game_board->num_of_rows, game_board->num_of_cols);
    if(error!=SUCCESS)
    {
        return error;
    }
    copy_cells(game_board->cells, backup, game_board->num_of_rows, NULL);
    full_autofill(game_board); 
    error = get_sol_ilp_lp(game_board,LP_GUESS,threshold, 0,0,NULL,NULL);
    if (error!=SUCCESS){
        free_cells(game_board->cells, game_board->num_of_cols);
        game_board->cells = backup;
        return error;
    }
    add_guess_node(game_board, backup);
    return SUCCESS;
}

/* @brief init the options for a certain time for the cell */
void create_cell_options(Board *game_board, Cell *cell, int *cell_options)
{
    int n = 0; /*whould contain the number of option for values this cell has*/
    int i;     /*var for iterations*/

    for (i = 1; i <= game_board->num_of_rows; ++i)
    {
        cell->value = i;
        if (!is_in_neigbors(game_board, cell)) /*if this is a valid value for cell, update the array "options" in the cells definition*/
        {
            cell_options[i] = 1;
            ++n; /*increase number of options by 1*/
        }
        else
        {
            cell_options[i] = 0;
        }
    }

    cell->value = 0;     /*reset cell's value*/
    cell_options[0] = n; /* the first cell in each list will hold the number of possible values for that cell*/
}
/*$ret a legal value for the cell, chosen randomly*/
ERROR rand_legal_val_for_cell(Board *game_board, int chosen_row, int chosen_col)
{
    int chosen_val;
    int *cell_options = malloc((game_board->num_of_rows + 1) * sizeof(int));
    if (!cell_options)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    create_cell_options(game_board, &game_board->cells[chosen_row][chosen_col], cell_options);

    /*this index holds the number of options*/
    if (cell_options[0] == 0)
    {
        free(cell_options);
        return NO_OPTIONS_FOR_CELL;
    }

    do
    {
        chosen_val = (rand() % game_board->num_of_rows) + 1;
    } while (cell_options[chosen_val] == 0);

    /*so we have a value in*/
    game_board->cells[chosen_row][chosen_col].value = chosen_val;
    game_board->cells[chosen_row][chosen_col].is_fixed = TRUE;
    game_board->cells[chosen_row][chosen_col].is_valid = TRUE;
    free(cell_options);
    return SUCCESS;
}
ERROR fill_x_cells_with_legal_random_values(Board *game_board, int x)
{
    ERROR error, val_res;
    int chosen_col, chosen_row, iter_cnt;
    int x_left;
    Cell **orig_cells;
    iter_cnt = 0;
    val_res = SUCCESS;
    error = init_cells(&orig_cells, game_board->num_of_rows, game_board->num_of_cols);
    if (error != SUCCESS)
    {
        return SUCCESS;
    }
    copy_cells(game_board->cells, orig_cells, game_board->num_of_rows, NULL);

    for (; iter_cnt < 1000; ++iter_cnt)
    {
        /*if we're trying again, we need to restart the board*/
        copy_cells(orig_cells, game_board->cells, game_board->num_of_rows, NULL);

        /*while there are still cells to choose..*/
        for (x_left = x; x_left != 0;)
        {
            /*choose a random empty cell*/
            chosen_col = rand() % game_board->num_of_cols;
            chosen_row = rand() % game_board->num_of_rows;

            if (game_board->cells[chosen_row][chosen_col].value != 0)
            {
                continue;
            }

            /*choose a random number for this cell, and make sure it's not in neigbors*/
            val_res = rand_legal_val_for_cell(game_board, chosen_row, chosen_col);
	    --x_left;
            if (val_res == MEMORY_ALLOCATION_ERROR)
            {
                free_cells(orig_cells, game_board->num_of_rows);
                return MEMORY_ALLOCATION_ERROR;
            }
            if (val_res == NO_OPTIONS_FOR_CELL)
            {
                break;
            }
        }
        if (val_res == NO_OPTIONS_FOR_CELL)
        {
            continue;
        }
        /*make sure there is a solution for the board...*/
        error = get_sol_ilp_lp(game_board, ILP, 0.0, 0, 0, NULL, NULL);

        if (error != SUCCESS && error != BOARD_ERRONEUOS && error != ERROR_NO_SOLUTION)
        {
            free_cells(orig_cells, game_board->num_of_rows);
            return error;
        }
        if (error == SUCCESS)
        {
            break;
        }
    }
    if (error == BOARD_ERRONEUOS || error == ERROR_NO_SOLUTION)
    {
        /*we need to restart the board*/
        copy_cells(orig_cells, game_board->cells, game_board->num_of_rows, NULL);
        free_cells(orig_cells, game_board->num_of_rows);
        return ERROR_PUZZLE_GENERATOR;
    }
    free_cells(orig_cells, game_board->num_of_rows);
    return error;
}
void erase_num_random_cells_from_board(Board *game_board, int num_to_erase)
{
    int chosen_col, chosen_row, i;
    /*erasing num_to_erase cells from board*/
    for (i = 0; i < num_to_erase; ++i)
    {
        do
        {
            chosen_col = rand() % game_board->num_of_cols;
            chosen_row = rand() % game_board->num_of_rows;
        } while (game_board->cells[chosen_row][chosen_col].value == 0);
        game_board->cells[chosen_row][chosen_col].value = 0;
        game_board->cells[chosen_row][chosen_col].is_fixed = FALSE;
        game_board->cells[chosen_row][chosen_col].is_valid = TRUE;
    }
}
/*only in edit mode*/
/*@brief makes new puzzle given an initialize game_board*/
ERROR generate(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    int num_to_erase, empty_cells;
    ERROR error;
    /*for entering a node...*/
    Generate_params *params;
    /*----------------------*/

    empty_cells = count_empty_cells(game_board);
    if (is_board_erroneus(game_board))
    {
        return CANT_GENERATE_ERRONEUOS_BOARD;
    }
    if (empty_cells < x)
    {
        return NOT_ENOUGH_EMPTY_CELLS;
    } /* TODO: check that y < num of cells */
    /*malloc generate_params*/
    /*for the List_node sake... */
    error = init_old_generate_params(game_board, &params);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    error = fill_x_cells_with_legal_random_values(game_board, x);
    if (error != SUCCESS)
    {
        free_generate_params(params);
        return error;
    }

    /*if we reached here, we succesfully made a good puzzle which is now
      loaded to cur_sol*/
    copy_from_sol_to_board(game_board);

    num_to_erase = game_board->num_of_rows * game_board->num_of_cols - y;
    erase_num_random_cells_from_board(game_board, num_to_erase);

    error = add_generate_node(game_board, params);
    if (error != SUCCESS)
    {
        free_generate_params(params);
        return error;
    }
    print_board_changers(game_board->cur_node->command_data->generate_params->old_cells,
                         game_board->cur_node->command_data->generate_params->new_cells,
                         game_board->num_of_rows,
                         game_board->blocks_per_rows,
                         game_board->blocks_per_cols);

    fix_all_cells(game_board, FALSE);

    (void)value;
    (void)threshold;
    (void)path;
    return SUCCESS;
}




/*needs to print what changed in some consessive format*/
/*I want to add to each of the 4 funcs BOOL add_node that tells whether
  its a func that ic called for redo porpuse, or a whole new call for
  the function*/
/*real params for undo: board(espacially move_list and cur_move)*/
ERROR undo(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    List_node *node_to_undo;
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;
    /*if cur_node is NULL && list_length >0 || list_length = 0*/
    if (game_board->cur_node == NULL)
    {
        return NO_COMMAND_TO_UNDO;
    }
    node_to_undo = game_board->cur_node;
    switch (node_to_undo->command_data->command)
    {
    case AUTOFILL:
        un_autofill(game_board, node_to_undo);

        print_autofill(node_to_undo->command_data->autofill_params->ind_to_del,
                       node_to_undo->command_data->autofill_params->ind_to_del_length);
        break;

    case GENERATE:
        un_generate(game_board, node_to_undo);

        print_board_changers(node_to_undo->command_data->generate_params->new_cells,
                             node_to_undo->command_data->generate_params->old_cells,
                             game_board->num_of_rows,
                             game_board->blocks_per_rows,
                             game_board->blocks_per_cols);
        break;

    case GUESS:
        un_guess(game_board, node_to_undo);

        print_board_changers(node_to_undo->command_data->guess_params->new_cells,
                             node_to_undo->command_data->guess_params->old_cells,
                             game_board->num_of_rows,
                             game_board->blocks_per_rows,
                             game_board->blocks_per_cols);
        break;

    case SET:
        un_set(game_board, node_to_undo);

        print_set(node_to_undo->command_data->set_params->new_cell,
                  node_to_undo->command_data->set_params->old_cell);
        break;

    default:
        break;
    }
    /*now is left only to traverse back on the list...*/
    game_board->cur_node = node_to_undo->prev;
    return SUCCESS;
}

/*needs to print what changed in some consessive format*/
/*to add suitable print !!!!*/
ERROR redo(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    List_node *node_to_redo;
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)game_board;
    (void)path;
  

    if (game_board->cur_node == game_board->moves_list->tail || game_board->moves_list->length == 0)
    {
        return NO_COMMAND_TO_REDO;
    }

    /*first we want to find the right node to redo...*/
    /*if everyting was undo - ed (we were dropped out of the list from the left*/
    if (game_board->cur_node == NULL)
    {
        node_to_redo = game_board->moves_list->head;
    }
    else
    {
        node_to_redo = game_board->cur_node->next;
    }

    switch (node_to_redo->command_data->command)
    {
    case AUTOFILL:
        redo_autofill(game_board, node_to_redo);
        print_autofill(node_to_redo->command_data->autofill_params->ind_to_del,
                       node_to_redo->command_data->autofill_params->ind_to_del_length);
        break;

    case GENERATE:
        redo_generate(game_board, node_to_redo);
        print_board_changers(node_to_redo->command_data->generate_params->old_cells,
                             node_to_redo->command_data->generate_params->new_cells,
                             game_board->num_of_rows,
                             game_board->blocks_per_rows,
                             game_board->blocks_per_cols);
        break;
    case GUESS:
        redo_guess(game_board, node_to_redo);
        print_board_changers(node_to_redo->command_data->guess_params->old_cells,
                             node_to_redo->command_data->guess_params->new_cells,
                             game_board->num_of_rows,
                             game_board->blocks_per_rows,
                             game_board->blocks_per_cols);
        break;
    case SET:
        redo_set(game_board, node_to_redo);
        print_set(node_to_redo->command_data->set_params->old_cell, 
                  node_to_redo->command_data->set_params->new_cell);
        break;
    default:
        break;
    }
    
    game_board->cur_node = node_to_redo;
    return SUCCESS;
}
ERROR redo_guess(Board *game_board, List_node *node_to_redo)
{
    copy_cells(node_to_redo->command_data->guess_params->new_cells, game_board->cells, game_board->num_of_rows, NULL);
    return SUCCESS;
}
ERROR reset(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    int i;
    ERROR res;
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;
    if ( game_board->moves_list->length == 0){
        print_error(NO_COMMAND_TO_UNDO);
    }

    for (i = 0; i < game_board->moves_list->length; ++i)
    {
        res = undo(game_board, 0, 0, 0, 0.0, NULL);
        if (res != SUCCESS)
        {
            return res;
        }
    }

    return SUCCESS;
}
ERROR save(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    ERROR error;
    Board *copied_board;
    (void) x;
    (void) y;
    (void) value;
    (void) game_board;
    (void) threshold;

    
    if (game_board->mode==SOLVE_STATE){
        return export_board(path, game_board);
    }
    
    /* else, we're in EDIT mode */
    if (is_board_erroneus(game_board))
	{
        return NO_SAVE_FOR_ERRONEUOS_BOARD;
    }

    error = get_sol_ilp_lp(game_board, ILP, 0.0, 0,0,NULL,NULL);
    if (error != SUCCESS)
	{
        return NO_SAVE_FOR_UNSOLVABLE_BOARD;
    }

    copied_board = malloc(sizeof(Board));
    if(!copied_board)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    copied_board = copy_board(game_board);
    fix_all_cells(copied_board, TRUE); 

    error = export_board(path, copied_board);

    free_all_board_memory(copied_board);
    free(copied_board);
    return error;

}

ERROR guess_hint(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    ERROR error;
    int *val_op;
    int row, col;
    double *val_chance;
    (void)value;
    (void)threshold;
    (void)path;
    row = y;
    col = x;    

    if (is_board_erroneus(game_board))
    {
        return CANT_GUESS_HINT_ERRONEOUS_BOARD;
    }
    if (game_board->cells[row][col].is_fixed)
    {
        return CANT_GUESS_HINT_FIXED_CELL;
    }
    if (game_board->cells[row][col].value != 0)
    {
        return CANT_GUESS_HINT_FOR_NON_EMPTY_CELL;
    }

    val_op = calloc(sizeof(int),game_board->num_of_rows);
    if(!val_op)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    val_chance = calloc(sizeof(double),game_board->num_of_rows);
    if(!val_chance)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    error = get_sol_ilp_lp(game_board, LP_GUESS_HINT, 0.0, row, col, val_op, val_chance); 
    if (error!=SUCCESS){
        free(val_chance);
        free(val_op);
	 print_guess_hint(row,col,val_op,val_chance,0);
        return error;
    }
    print_guess_hint(col,row,val_op,val_chance,game_board->num_of_rows);
    free(val_chance);
    free(val_op);
    return SUCCESS;
}
ERROR num_solutions(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    int res;
    (void)x;
    (void)y;
    (void)value;
    (void)game_board;
    (void)threshold;
    (void)path;
    res = count_solutions(game_board);
    print_num_solutions(res);
    return SUCCESS;
}

ERROR print_board_command(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;
    (void)game_board;

    print_board(game_board);
    return SUCCESS;
}

/*@note to be used only in solve mode
 *@note should copy the board in order to work on a non changing board
 *$ret BOARD_ERRONEUOS when input is an erroneus board (has two neigbors with same value!)
       MEMORY_ALLOCATION_ERROR if a problem in this occured. need to check for this an
                                free upper alocations */

/* ind-changed - organized the same as variable mat:
   row 1 : value
   row 2 : col
   row 3 : row */
ERROR autofill_command(Board *game_board, int x, int y, int value, double threshold, char *path)
{
    int changes_counter;
    ERROR error;
    int **ind_changed;
    
    if(is_board_erroneus(game_board)){
        return NO_AUTOFILL_FOR_ERRONEUS_BOARD;
    }
    error = get_sol_ilp_lp(game_board,ILP,0.0,0,0,NULL,NULL);
    if (error!=SUCCESS){
        return NO_AUTOFILL_FOR_UNSOLVABLE_BOARD;
    }

    changes_counter = 0;

    (void)x;
    (void)y;
    (void)value;
    (void)threshold;
    (void)path;

    error = autofill_inner(game_board, &changes_counter, &ind_changed);
    if (error != SUCCESS)
    {
        return error;
    }

    /*--------enter to moves_list!!--------*/
    error = add_node_autofill(game_board, ind_changed, changes_counter);
    if (error != SUCCESS)
    {
        /*is it good to free it here??*/
        free_mat(ind_changed, 3);
        return error;
    }
    print_autofill(game_board->cur_node->command_data->autofill_params->ind_to_del,
                   game_board->cur_node->command_data->autofill_params->ind_to_del_length);

    return SUCCESS;
}

ERROR un_autofill(Board *game_board, List_node *node_to_undo)
{
    int i, ind_to_del_length, row, col;
    int **ind_to_del;
    ind_to_del = node_to_undo->command_data->autofill_params->ind_to_del;
    ind_to_del_length = node_to_undo->command_data->autofill_params->ind_to_del_length;

    for (i = 0; i < ind_to_del_length; ++i)
    {
        row = ind_to_del[2][i];
        col = ind_to_del[1][i];

        game_board->cells[row][col].value = 0;
        game_board->cells[row][col].is_fixed = FALSE;
        game_board->cells[row][col].is_set_by_user = FALSE;
    }

    return SUCCESS;
}

/*@ shouldn't run generate, just take out from memory the old state of the board*/
ERROR un_generate(Board *game_board, List_node *node_to_undo)
{
    copy_cells(node_to_undo->command_data->generate_params->old_cells, game_board->cells, game_board->num_of_cols,
               NULL);

    copy_int_mat(node_to_undo->command_data->generate_params->old_sol, game_board->cur_sol,
                 game_board->num_of_rows, game_board->num_of_cols);
    return SUCCESS;
}

/*@ shouldn't run generate, just take out from memory the old state of the board (as uses lp solver...)*/
ERROR un_guess(Board *game_board, List_node *node_to_undo)
{
    copy_cells(node_to_undo->command_data->guess_params->old_cells, game_board->cells, game_board->num_of_rows, NULL);
    return SUCCESS;
}

/*TODO!*/
ERROR un_set(Board *game_board, List_node *node_to_undo)
{
    Cell *old_cell = node_to_undo->command_data->set_params->old_cell;
    copy_cell(old_cell, &game_board->cells[old_cell->coordinates.y][old_cell->coordinates.x]);

    
    return SUCCESS;
}

/*@param node_to_redo - gets the node that holds the parameters to redo*/
ERROR redo_autofill(Board *game_board, List_node *node_to_redo)
{
    int i, row, col, val;

    int **ind_changes = node_to_redo->command_data->autofill_params->ind_to_del;
    int ind_changes_length = node_to_redo->command_data->autofill_params->ind_to_del_length;

    for (i = 0; i < ind_changes_length; ++i)
    {
        row = ind_changes[2][i];
        col = ind_changes[1][i];
        val = ind_changes[0][i];

        game_board->cells[row][col].value = val;
        game_board->cells[row][col].is_set_by_user = TRUE;
    }
    return SUCCESS;
}

ERROR redo_generate(Board *game_board, List_node *node_to_redo)
{
    copy_cells(node_to_redo->command_data->generate_params->new_cells, game_board->cells, game_board->num_of_cols,
               NULL);

    copy_int_mat(node_to_redo->command_data->generate_params->new_sol, game_board->cur_sol,
                 game_board->num_of_rows, game_board->num_of_cols);

    return SUCCESS;
}

ERROR redo_set(Board *game_board, List_node *node_to_redo)
{
    Cell *new_cell = node_to_redo->command_data->set_params->new_cell;
    copy_cell(new_cell, &game_board->cells[new_cell->coordinates.y][new_cell->coordinates.x]);

    return SUCCESS;
}
