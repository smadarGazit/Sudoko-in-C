#ifndef MAIN_AUX_H
#define MAIN_AUX_H


#include "board.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "UI.h"
#include "commands.h"


/*#include "solver.h"*/

/*@brief generate a solved board using get_sol_rand
 * copy the sol to game_board.cur_sol
 * repeat num_of_fixed times: 
 *                              - randomly select an empty cell
 *                              - if the cell is fixed, pick another until it is fixed
 *                              - assign fix to the cell
 * delete from game_board.cells every cell that is not fixed */
/*ERROR init_board(Board* game_board);*/
ERROR init_board(Board *game_board);

void free_mat(int **varibale_mat, int main_list_length);
void free_cells(Cell **matrix, int n);
void copy_int_mat(int **src, int **copy, int main_length, int secondary_length);
void print_int_mat(const Board *game_board);
void restart_cur_sol(Board *board);
int count_solutions(Board *game_board);

ERROR validate_user_input(Board *game_board, Move *move);

void reset_cell_list(Board *game_board);
void choose_cells(Board *game_board, Point *to_fill,  int x);
ERROR fill_X_cells(Board *game_board, Point *to_fill,  int x);
void delete_Y_cells(Board *game_board, Point *to_delete,  int y);


/*guess help methods*/
ERROR full_autofill(Board *game_board);
int weighted_random_choice(int *options, double *options_chance, int length);
int count_cell_options(int *rows, int *cols, int len, int cell_row, int cell_col, double threshold, double *sol, int *strt, int *end, Board *board, int *vals);
void group_options(int *options, double *options_chance, double *sol, int *vals, int ind_start, int ind_end, double threshold, Board *board, int row, int col);

void print_cur_sol(const Board *game_board);


#endif