#ifndef UI_H_   
#define UI_H_
#include "typedefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h" 
#include "board.h"
#include "commands.h"

#define MAX_COMMAND_LEN 256
#define MAX_ERROR_LEN 256

/*General*/
void print_welcome_messege();
void print_goodbye_messege();
void print_command_prompt_messege();
/*-------------------------------------------*/

/* Error printing and related */

void print_validation_error(Move *move, char* messege);
void print_param_error(Move_parameter *param, int index_param, char *min_range, char *max_range, Command command);
void print_error(ERROR error);
/*-----------------------------------------------------------------*/

/* Output of game functions */

void print_hint(int x, int y, int value);
void print_guess_hint(int row, int col, int *val_op, double *val_chance,int num_of_op);
void print_board(const Board *game_board);
/*---------------------------------------------------------------------------------------*/

/* User input and related */

/*@brief - reads user input into the provided string.
    if EOF is reached or if the line is longer than 256 chars, an error is returned.
    in the end_of_input var, the method will store the index of the "\0" char finishing the input string, 
    if the funcs result is SUCCESS */
ERROR get_user_input(char *user_input);
BOOL is_blank(char* str, int length); 
int find_end_of_input(char* str, int length);
/*---------------------------------------------------------------------------------------*/

void print_file_error(ERROR error, char *info);

void print_set(Cell *before, Cell *after);
ERROR print_board_changers(Cell **before, Cell **after, int main_length, int blocks_per_rows, int block_per_cols);
void print_ind_changed(int **mat, int list_length);
void print_autofill(int **ind_changed, int ind_changed_length);
void print_num_solutions(int num);
void print_validation(ERROR error);

#endif
