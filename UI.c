#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UI.h"
#include "errors.h"
#include "board.h"
#include "commands.h"
#include "cell.h"
#include "move.h"


/* General */

void print_welcome_messege(){
    printf("Welcome to the game! What would you like to do?\n");
    printf("To play, write 'solve', to edit your own game, write 'edit', to exit, write 'exit'.\n");
    printf("Enjoy!\n");
}

void print_goodbye_messege(){
    printf("\nThank you for playing! Goodbye\n");
}

void print_command_prompt_messege(){
    printf("\nPlease enter your next command\n");
}

/*--------------------------------------------------------------------------------------------------------*/

/* Error printing and related */

void print_validation_error(Move *move, char* info){
    ERROR error = move->state;
    if (error==SUCCESS){
        return;
    }
    printf("INPUT ERROR:\n");
    switch (error)
    {
        case UNKNOWN_COMMAND:
            printf("Unrecognized Command: '%s' is not a valid command name\n",move->path);
            break;
        case COMMAND_UNAVAILABLE:
            printf("Unavilable Command: The command '%s' is unavilabe in %s mode\n", command_to_str(move->COMMAND), info);
            break;
        case TOO_FEW_PARAMS:
            printf("Too Few Parameters For the Chosen Command - The correct syntax for '%s' command is:\n",command_to_str(move->COMMAND));
            printf("%s", correct_syntax(move->COMMAND));   
            break;
        case TOO_MANY_PARAMS:
            printf("Too Many Parameters For the Chosen Command - The correct syntax for '%s' command is:\n",command_to_str(move->COMMAND));
            printf("%s", correct_syntax(move->COMMAND));
            break;
        case NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED: /*for a command with single param*/
            printf("Parameter Error - Non numeric value: The command guess requires a numeric parameter\n");
            break;
        case EXTRAPARAM_FAULT:
            printf("Parameter Error - Parameter not in range: The parameter needs to be greater than 0 and smaller than 1\n");
            break;
        case MARK_ERRORS_FAULT:
            printf("Parameter Error - Parameter not in range: The parameter needs to be 1 or 0\n");
            break;
        case PARAM_NOT_IN_RANGE:/*for a command with single param*/
            printf("%s",info);
            break;
        case INDEVIDUAL_PARAM_ERROR:
            if (move->param1.exist && move->param1.param_state!=SUCCESS)
            {
                print_param_error(&(move->param1), 1, "1", info, move->COMMAND);
            }
            if (move->param2.exist && move->param2.param_state!=SUCCESS)
            {
                print_param_error(&(move->param2), 2, "1",info, move->COMMAND);
            }
            if (move->param3.exist && move->param3.param_state!=SUCCESS)
            {
                print_param_error(&(move->param3), 3, "0", info, move->COMMAND);
            }
            
            break;
        default:
            break;
    }
}

void print_param_error(Move_parameter *param, int index_param, char *min_range, char *max_range, Command command){
    if (param->param_state == NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED){
        printf("Parameter #%d Error - Non int value: The command '%s' requires integer parameters\n",index_param,command_to_str(command));
        return;
    }
    if (param->param_state == OUT_OF_RANGE){
        printf("Parameter #%d Error - Out of Parameter Range: The parameter needs to be an integer from %s to %s\n", index_param, min_range, max_range);
    }
    return;
}

void print_error(ERROR error){
    switch (error)
    {
        case TOO_LONG_INPUT:
            printf("ERROR: Too long input - more than 256 characters.");
            break;
        case SET_CANT_CHANGE_FIXED_CELL:
            printf("GAME ERROR: can't set a value to a fixed cell.");
            break;
        case MEMORY_ALLOCATION_ERROR:
            printf("NO MAMORY - can't play!");
            break;
        case SET_CLASH_WITH_FIXED_CELL:
            printf("GAME ERROR: can't set a value that clashes with a fixed cell.");
            break;
        case PUZZLE_FINISHED_BUT_ERRONEOUS:
            printf("GAME ERROR: your finished puzzule has an error!");
            break;
        case PUZZLE_SOLVED:
            printf("Hurray! Puzzle solved successfuly!");
            return;
        case NO_VALIDATION_FOR_ERRONEOUS_BOARD:
            printf("GAME ERROR: can't validate an erroneous board.");
            break; 
        case CANT_HINT_ERRONEOUS_BOARD:
            printf("GAME ERROR: your board is erroneous. can't hint an erroneous board.");
            break;
        case CANT_HINT_FULL_CELL:
            printf("GAME ERROR: the cell you wanted is full. can't hint a full cell.");
            break;
        case CANT_HINT_FIXED_CELL:
            printf("GAME ERROR: the cell you wanted is fixed. can't hint a fixed cell.");  
            break;
        case ERROR_GUROBI:
            printf("ERROR with GUROBI OPTIMIZER. operation failed.");
            break;
        case CANT_GENERATE_ERRONEUOS_BOARD:
            printf("GAME ERROR: can't generate a puzzle of an erroneous board.");
            break;
        case NOT_ENOUGH_EMPTY_CELLS:
            printf("GAME ERROR: not enough empty cells to generate a puzzle.\nTo use 'generate' your first parameter needs to be smaller or equal to the number of empty cells in the board.");
            break;
        case NO_COMMAND_TO_UNDO:
            printf("No command to undo.");
            break;
        case NO_COMMAND_TO_REDO:
            printf("No command to redo.");
            break;
        case NO_SAVE_FOR_ERRONEUOS_BOARD:
            printf("GAME ERROR: your board is erroneous. can't save an erroneous board.");
            break;
        case NO_SAVE_FOR_UNSOLVABLE_BOARD:
            printf("GAME ERROR: your board is unsolvable. can't save an unsolvable board.");
            break;
        case CANT_GUESS_HINT_ERRONEOUS_BOARD:
            printf("GAME ERROR: your board is erroneous. can't save an erroneous board.");
            break;
        case CANT_GUESS_HINT_FOR_NON_EMPTY_CELL:
            printf("GAME ERROR: the cell you wanted is full. can't guess a hint a full cell.");
            break;
        case CANT_GUESS_HINT_FIXED_CELL:
            printf("GAME ERROR: the cell you wanted is fixed. can't guess a hint a fixed cell.");  
            break;
	case ERROR_PUZZLE_GENERATOR:
            printf("ERROR PUZZLE GENERATOR: over 1000 iterations, coudn't generate a board.");
            break;
	case NO_AUTOFILL_FOR_ERRONEUS_BOARD:
            printf("GAME ERROR: can't autofill on an erroneus board.");
	    break;
	case NO_AUTOFILL_FOR_UNSOLVABLE_BOARD:
            printf("GAME_ERROR: can't autofill an unsolvable board");
            break;
        default:
            return;
    }
    printf(" Please try again\n");
}



/*--------------------------------------------------------------------------------------------------------*/

/* Output of game functions */

void print_hint(int x, int y, int value){
    printf("Hint: The value of cell <%d,%d> should be %d\n", x,y,value);
}

void print_guess_hint(int row, int col, int *val_op, double *val_chance,int num_of_op){
    int i;
    if (num_of_op == 0){
        /*no options means no solution was found*/
        printf("GAME ERROR: can't guess a hint for an unsolvable board.\n");
    }
    printf("Guessed Hint - Theese are the results of the guess for cell <%d,%d>:\n",col+1,row+1);
    for (i=0; i<num_of_op; ++i){
        if (val_op[i]==0){
            break;
        }
        printf("value: %d, chance: %1.3f\n",val_op[i],val_chance[i]);
    }
}


/*@brief - prints the board to screen*/
void print_board(const Board *game_board)
{
    char *line;
    int i, r,c;
    /*int cnt*/
    int rows_in_block = game_board->blocks_per_rows;
    int cols_in_block = game_board->blocks_per_cols;
    Cell **cell_matrix = game_board->cells;
    
    /*prepare line*/
    line = calloc((4*(game_board->num_of_rows)+game_board->blocks_per_rows+2),sizeof(char));
    for (i=0;i<4*(game_board->num_of_rows)+game_board->blocks_per_rows+1;++i){
        line[i] = '-';
    }
    line[i] = '\0';

    printf("\n");
       /*for each row*/
    for (r = 0; r < game_board->num_of_rows; ++r)
    {
        if (r % rows_in_block == 0)
        {
            printf(("%s\n"),line);
        }
        /*for each cell in this row*/
        for (c = 0; c < game_board->num_of_cols; ++c)
        {
            if (c % cols_in_block == 0)
            {
               printf("|");
            }

            if (cell_matrix[r][c].is_fixed)
            {
                printf(" %2d.", cell_matrix[r][c].value);
            }
            else if (game_board->mode == EDIT_STATE || game_board->mark_errors){
                if (!cell_matrix[r][c].is_valid){
                    printf(" %2d*", cell_matrix[r][c].value);
                }
                else if (cell_matrix[r][c].value == 0)
                {
                    printf("    ");
                }
                else{
                    printf(" %2d ", cell_matrix[r][c].value);
                }
            }
            else if (cell_matrix[r][c].value == 0)
                {
                    printf("    ");
                }
            else
            {
                printf(" %2d ", cell_matrix[r][c].value);
            } 
        }
        printf("|\n");
    }
    printf(("%s\n"),line);
}

void print_validation(ERROR error){
    if (error == SUCCESS){
        printf("Board is solvable.");
    }
    if (error == ERROR_NO_SOLUTION){
        printf("Board is unsolvable.");
    }
    return;
}



/*--------------------------------------------------------------------------------------------------------*/

/* User input and related */


BOOL is_blank(char* str, int length){
    int i;
    for (i=0; i<length; ++i){
        if (str[i] != 9 && str[i] != 32 && str[i] != 10 && str[i] != 0){
            return FALSE;
        }
    }
    return TRUE;
}

int find_end_of_input(char* str, int length){
    int i;
    for (i=0; i<length; ++i){
        if (str[i]==0){
            return i;
        }
    }
    return -1;
}


/*@brief - reads user input into the provided string.
    if EOF is reached or if the line is longer than 256 chars, an error is returned.
    in the end_of_input var, the method will store the index of the "\0" char finishing the input string, 
    if the funcs result is SUCCESS */
ERROR get_user_input(char *user_input){
    int end=-1;
    memset(user_input, 0 , MAX_COMMAND_LEN+1); /*set memory to 0's*/
    
    if (!fgets(user_input,MAX_COMMAND_LEN+1, stdin)){
        return REACHED_EOF; /*if EOF was encountered, return FALSE*/
    }
    

    if (is_blank(user_input,  MAX_COMMAND_LEN+1)){
        return BLANK_LINE_INPUT;
    }

    end = find_end_of_input(user_input,  MAX_COMMAND_LEN+1);

    if (end<0){
        return TOO_LONG_INPUT;
    }

    return SUCCESS;

}

/*--------------------------------------------------------------------------------------------------------*/

void print_file_error(ERROR error, char *info){
    if (error==SUCCESS){
        return;
    }
    printf("\nFILE ERROR:\n");
    switch (error)
    {
        case ILLIGEAL_PATH:
            printf("File Not Found ");
            break;
        case WRONG_FILE_FORMAT_MISSING_VALS:
            printf("Wrong File Format -  Missing Values");
            break;
        case WRONG_FILE_FORMAT_TOO_MANY_VALS:
            printf("Wrong File Format -  Too Many Values");
            break;
        case WRONG_FILE_FORMAT_MISSING_VALS_TOP:
            printf("Wrong File Format -  Missing Values");
            break;
        case WRONG_FILE_FORMAT_RANGE:
            printf("Wrong File Format - Out of Range");
            break;
        case WRONG_FILE_FORMAT_ILLGEAL_CHAR:
            printf("Wrong File Format - Illegal Char");
            break;

        default:
            break;
    }
    printf(": %s", info);
 }


/*for set, unset, redo set*/
void print_set(Cell *before, Cell *after)
{
    printf("\nThe cell in row %d, col %d changed value from: %d to %d", (before->coordinates.y)+1, (before->coordinates.x)+1, before->value,after->value);
}



ERROR print_board_changers(Cell **before, Cell **after, int main_length, int blocks_per_rows, int block_per_cols)
{
    Board temp_board;
    ERROR error;
    error = new_board(&temp_board, main_length, main_length, blocks_per_rows, block_per_cols);
    if(error != SUCCESS)
    {
        return error;
    }

    temp_board.cells = before;
    printf("\nThe board before last operation was: \n");
    print_board(&temp_board);

    temp_board.cells = after;
    printf("\nThe board as it is now: \n");

    temp_board.cells = NULL;
    free_all_board_memory(&temp_board);
    return SUCCESS;
}


void print_ind_changed(int **mat, int list_length)
{
    int i;
    for(i = 0; i < list_length; ++i)
    {
        if(mat[0][i] != 0)
        {
            printf("\n row %d, col %d \n", mat[2][i]+1, mat[1][i]+1);
            printf("\n----------------\n");
        }
        else
        {
            break;
        }
    }
}

void print_autofill(int **ind_changed, int ind_changed_length)
{
    printf("\n Here are indices that changed: \n");
    print_ind_changed(ind_changed, ind_changed_length);
    printf("\n Its all changes \n");
    printf("\n --------------- \n");
}

void print_num_solutions(int num)
{
    printf("\n There are %d possible solutions for the current board \n", num);
}





