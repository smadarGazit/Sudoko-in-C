#ifndef BOARD_H
#define BOARD_H
#include "typedefs.h"
#include "bool.h"
#include "cell.h"
#include <stdio.h>
#include "commands.h"
#include "modes.h"
#include "errors.h"


#define EOB (NULL)

/*@brief - the matrix that we implemented is an array that holds array that they are the rows
 *the meaning is that it's a moves_list of rows(and not cols...)      */
struct board
{
    int num_of_rows;
    int num_of_cols;/*TODO - redundant*/
    /* blocks per row is the number of block fits vertically */
    int blocks_per_rows;
    /* blocks per row is the number of block fits horizontally 
     * blocks_per_rows*blocks_per_cols gives howmany blocks in total */
    int blocks_per_cols;
    /*a matrix, cells[x][y] holds a creature which is of struct Cell*/
    Cell **cells;
    /*holds the solution from board, from initaliztion or from the last time validate ran and returned TRUE*/
    int **cur_sol;
    /*from the five commands - the ones that are availavble will be set to TRUE, otherwise FALSE*/
    BOOL mark_errors;
    Mode mode; /*Init /edit /solve*/

    List* moves_list; /*the history of moves that are guess, generate, */
    List_node* cur_node; /*NULL when the list is empty, 
                            or when undo -ing the first command that was called
                            sits on the node that suit the last command that eas executed*/
    BOOL exists;
};

/*@brief - a constructor for a new board, initializes out_new_board
 *@param out_new_board - a pointer for a board to be initializes
 *@param num_of_rows, num_of_cols - the number of rows and cols of the board
 *@param blocks_per_rows, blocks_per_cols
 *@return TRUE if paramerter were valid (num_of_rows%block_per_row == 0, the same with cols)*/

ERROR new_board(Board *out_new_board, int num_of_rows, int num_of_cols, int blocks_per_rows, int blocks_per_cols);

/*@brief - free the dinamic memory that the board allocated
        - frees cur_sol and cells arrays*/
void free_all_board_memory(Board *game_board);

/*@brief - checks if the board was finished
 *@return TRUE if the board is filled completely, else FALSE*/
BOOL is_board_finished(Board *game_board);

/*@breif finds the next cell in board->cells
 *@param board is the current struct board that the user play on
 *@param cell is the cell that we want to find the next cell after this
 *@return the next cell in the matrix, from left to right and from up to down
 *@return EOB if the end of the board is reached*/
Cell *next(Board *board, Cell *cell);
/*@brief checks if the current value of cell is its row, col or block
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell that its value is checked to be in its neigbors
 *@returns TRUE if in one of the neigbors, and FALSE otherwise
*/
BOOL is_in_neigbors(Board *game_board, Cell *cell);
ERROR copy_cells(Cell **src, Cell **out_copied_cells, int main_length, int **ind_changed);
Board* copy_board(Board *board);
BOOL is_board_erroneus(Board *board);
void find_all_upper_left(Board *board, Point *block_list);
    /*enter the first cell in the list the 0,0 - this is the upper left corner of the first block at every board*/
ERROR init_cells(Cell ***cells, int num_of_rows, int num_of_cols);
void copy_from_sol_to_board(Board *game_board);
int get_obvious_value(Board *game_board, int col, int row);
ERROR init_int_mat(int ***mat, int num_of_rows, int num_of_cols);
ERROR autofill(Board* game_board);
ERROR autofill_inner(Board* game_board, int* changes_counter, int*** ind_changed);

BOOL clash_with_fixed_cell(Board *game_board, Cell *cell);
void fix_all_cells(Board *board, BOOL make_fixed);

/*we assume clash with fixed cell was already checked, so if we got here 'set' func should return success and
    thus we can change the validity of the cells*/
void mark_visble_errors(Board *game_board);
int count_empty_cells(Board *game_board);
/*if we got here we know that in the row of the cell there are 2 cells at laest with the value of the cell*/
void set_validity_row(Board *game_board, Cell *cell);

/*if we got here we know that in the row of the cell there are 2 cells at laest with the value of the cell*/
void set_validity_col(Board *game_board, Cell *cell);

void set_validity_block(Board *game_board, Cell *cell);
ERROR init_cells(Cell ***cells, int num_of_rows, int num_of_cols);
void reset_validity_all_cells(Board *game_board);
#endif