#include <stdlib.h>
#include <stdio.h>
#include "typedefs.h"
#include "board.h"
#include "cell.h"
#include "bool.h"
#include "commands.h"
#include "main_aux.h"
#include "list_node.h"
#include "list.h"

int MAX_NUMxx = 0;

ERROR init_cells(Cell ***cells, int num_of_rows, int num_of_cols)
{
    int i, j;
    Cell *cur_cell;
    Cell **cells_matrix;
    cells_matrix = malloc(sizeof(Cell *) * num_of_rows);
    if (cells_matrix == NULL)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    for (i = 0; i < num_of_rows; ++i)
    {
        cells_matrix[i] = malloc(sizeof(Cell) * num_of_cols);
        if (cells_matrix[i] == NULL)
        {
            free(cells_matrix);
            return MEMORY_ALLOCATION_ERROR;
        }
        for (j = 0; j < num_of_cols; ++j)
        {
            cur_cell = &cells_matrix[i][j];
            get_new_cell(cur_cell, 0, j, i, FALSE, FALSE, TRUE);
        }
    }
    *cells = cells_matrix;
    return SUCCESS;
}

ERROR init_int_mat(int ***mat, int num_of_rows, int num_of_cols)
{
    int i;
    int **matrix = calloc(num_of_rows, sizeof(int *));
    if (!matrix)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    for (i = 0; i < num_of_rows; ++i)
    {
        matrix[i] = calloc(num_of_cols, sizeof(int));
        if (!matrix[i])
        {
            free(matrix);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    *mat = matrix;
    return SUCCESS;
}

/*@brief - a constructor for a new board, initializes out_new_board
 *@param out_new_board - a pointer for a board to be initializes
 *@param num_of_rows, num_of_cols - the number of rows and cols of the board
 *@param blocks_per_rows, blocks_per_cols
 *@return TRUE if paramerter were valid (num_of_rows%block_per_row == 0, the same with cols)*/
ERROR new_board(Board *out_new_board, int num_of_rows, int num_of_cols, int blocks_per_rows, int blocks_per_cols)
{
    if (num_of_rows % blocks_per_rows == 0 && num_of_cols % blocks_per_cols == 0)
    {
        ERROR error;
        out_new_board->blocks_per_cols = blocks_per_cols; /*block width*/
        out_new_board->blocks_per_rows = blocks_per_rows; /* block hight */
        out_new_board->num_of_rows = num_of_rows;
        out_new_board->num_of_cols = num_of_cols;
        out_new_board->mode = INIT; /*is that a good initilization*/
        out_new_board->mark_errors = FALSE;
        out_new_board->exists = TRUE;
        MAX_NUMxx= out_new_board->num_of_rows = num_of_rows;
        error = get_list(&out_new_board->moves_list);
        if (error == MEMORY_ALLOCATION_ERROR)
        {
            return MEMORY_ALLOCATION_ERROR;
        }
        out_new_board->cur_node = NULL;


        /*init cells*/
        error = init_cells(&out_new_board->cells, num_of_rows, num_of_cols);
        if (error == MEMORY_ALLOCATION_ERROR)
        {
            return MEMORY_ALLOCATION_ERROR;
        }
        /*init cur_sol*/
        error = init_int_mat(&out_new_board->cur_sol, num_of_rows, num_of_cols);
        if (error == MEMORY_ALLOCATION_ERROR)
        {
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    else
    {
        printf("Invalid parameters");
        return INVALID_BOARD_PARAMETERS; /*this shouldn't be reached*/
    }
    return SUCCESS;
}

/*@brief - free the dinamic memory that the board allocated
        - frees cur_sol and cells arrays*/
void free_all_board_memory(Board *game_board)
{
    int i;
    if (game_board->exists == FALSE)
    {
        return;
    }
    if (game_board->cells)
    {
        for (i = 0; i < game_board->num_of_rows; ++i)
        {
            free(game_board->cells[i]);
        }
        free(game_board->cells);
    }
    if (game_board->cur_sol)
    {
        for (i = 0; i < game_board->num_of_rows; ++i)
        {
            free(game_board->cur_sol[i]);
        }
        free(game_board->cur_sol);
    }
    if (game_board->moves_list)
    {
        free_list(game_board->moves_list);
    }
}

/*@brief - checks if the board was finished
 *@return TRUE if the board is filled completely, else FALSE*/
BOOL is_board_finished(Board *game_board)
{
    int i, j;
    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        for (j = 0; j < game_board->num_of_cols; ++j)
        {
            if (game_board->cells[i][j].value == 0) /*signal for empty cell*/
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*@breif finds the next cell in board->cells
 *@param board is the current struct board that the user play on
 *@param cell is the cell that we want to find the next cell after this
 *@return the next cell in the matrix, from left to right and from up to down
 *@return EOB if the end of the board is reached*/
Cell *next(Board *board, Cell *cell)
{
    int x = cell->coordinates.x;
    int y = cell->coordinates.y;
    if (x == board->num_of_rows - 1 && y == board->num_of_cols - 1)
    {
        return EOB;
    }
    if (x == board->num_of_cols - 1)
    {
        return &(board->cells[y + 1][0]);
    }
    return &(board->cells[y][x + 1]);
}

/*@brief finding the left upper point of the block of cell
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell to find to which block it belongs
 *@return left upper point of the cur cell block*/
Point blocks_neigbor_range(Board *game_board, Cell *cell)
{
    int upper_border, left_border, block_row_size, block_col_size;
    block_row_size = game_board->blocks_per_rows;
    block_col_size = game_board->blocks_per_cols;

    upper_border = cell->coordinates.y - (cell->coordinates.y % block_row_size);
    left_border = cell->coordinates.x - (cell->coordinates.x % block_col_size);

    return new_point(left_border, upper_border);
}
/*@breif - checks if the cell->value is in its block
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell that we want to check is cell->value is in one of the cells in its block
 *@returns TRUE if the value already exit in cell's row, otherwise FALSE
 */
BOOL is_in_block(Board *game_board, Cell *cell)
{
    int i, j, block_row_size, block_col_size;
    Point upper_left_point;
    upper_left_point = blocks_neigbor_range(game_board, cell);

    block_row_size = game_board->blocks_per_rows;
    block_col_size = game_board->blocks_per_cols;

    for (i = upper_left_point.y; i < (upper_left_point.y + block_row_size); ++i)
    {
        for (j = upper_left_point.x; j < (upper_left_point.x + block_col_size); ++j)
        {
            /*if it is not the original coordinates and the neigbor hold the same value*/
            if (i == cell->coordinates.y && j == cell->coordinates.x)
            {
                continue;
            }
            else if (game_board->cells[i][j].value == cell->value)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
/*@breif - checks if the cell->value is in its row
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell that we want to check is cell->value is in one of the cells in its row
 *@returns TRUE if the value already exit in cell's row, otherwise FALSE
 */
BOOL is_in_row(Board *game_board, Cell *cell)
{
    int i, row;
    row = cell->coordinates.y;

    for (i = 0; i < game_board->num_of_cols; ++i)
    {
        if (i == cell->coordinates.x)
        {
            continue;
        }
        else if (cell->value == game_board->cells[row][i].value)
        {
            return TRUE;
        }
    }
    return FALSE;
}
/*@breif - checks if the cell->value is in its col
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell that we want to check is cell->value is in one of the cells in its col
 *@returns TRUE if the value already exit in cell's col, otherwise FALSE
 */
BOOL is_in_col(Board *game_board, Cell *cell)
{
    int i, col;
    col = cell->coordinates.x;

    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        if (i == cell->coordinates.y)
        {
            continue;
        }
        else if (cell->value == game_board->cells[i][col].value)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*@brief checks if the current value of cell is its row, col or block
 *@param game_board is the current struct board that the user play on
 *@param cell is the cell that its value is checked to be in its neigbors
 *@returns TRUE if in one of the neigbors, and FALSE otherwise
*/
BOOL is_in_neigbors(Board *game_board, Cell *cell)
{
    if (is_in_block(game_board, cell) || is_in_col(game_board, cell) || is_in_row(game_board, cell))
    {
        return TRUE;
    }

    else
    {
        return FALSE;
    }
}

/*copies only a game_board->cells, and adds a node when add_node is TRUE*/
ERROR copy_cells(Cell **src, Cell **out_copied_cells, int main_length, int **ind_changed)
{
    Cell *cur_cell;
    int row, col, cur_ind;
    cur_ind = 0;

    for (row = 0; row < main_length; ++row)
    {
        for (col = 0; col < main_length; ++col)
        {
            /*only in autofill...*/
            if (ind_changed != NULL && out_copied_cells[row][col].value != src[row][col].value)
            {
                ind_changed[2][cur_ind] = row;
                ind_changed[1][cur_ind] = col;
                ind_changed[0][cur_ind] = src[row][col].value;
                ++cur_ind;
            }
            cur_cell = &out_copied_cells[row][col];

            get_new_cell(cur_cell, src[row][col].value, col, row, src[row][col].is_set_by_user, src[row][col].is_fixed, src[row][col].is_valid);
        }
    }
    return SUCCESS;
}

/*@brief - iterates over all not fixed cells and
 asks is_in_neigbors*/
BOOL is_board_erroneus(Board *board)
{
    int i, j;
    for (i = 0; i < board->num_of_rows; ++i)
    {
        for (j = 0; j < board->num_of_cols; ++j)
        {
            if (board->cells[i][j].value != 0 && is_in_neigbors(board, &(board->cells[i][j])))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*doesnt copy cur_sol or available command, for now*/
Board *copy_board(Board *board)
{

    Board *board_copied = malloc(sizeof(Board));
    new_board(board_copied, board->num_of_rows, board->num_of_cols, board->blocks_per_rows, board->blocks_per_cols);
    copy_cells(board->cells, board_copied->cells, board->num_of_rows, NULL);

    board_copied->mark_errors = board->mark_errors;
    board_copied->mode = board->mode;

    /*copy cur_sol*/
    copy_int_mat(board->cur_sol, board_copied->cur_sol, board->num_of_rows, board->num_of_cols);

    board_copied->mark_errors = board->mark_errors;
    board_copied->mode = board->mode;

    return board_copied;
}

/*the width of each block == blocks_per_col; the hight of eack block == blocks_per_row*/
void find_all_upper_left(Board *board, Point *block_list)
{
    int row, col;
    /*enter the first cell in the list the 0,0 - this is the upper left corner of the first block at every board*/
    /*y is for the row, x is for the col*/
    int i;
    i = 0;

    for (row = 0; row < (board->num_of_rows); ++row)
    {
        for (col = 0; col < (board->num_of_cols); ++col)
        {
            if (col % board->blocks_per_cols == 0 && row % board->blocks_per_rows == 0)
            {
                block_list[i] = new_point(col, row);
                ++i;
            }
        }
    }
}

/*$ret 0 if there is no obvious value,
 *     -1 if there are two neigbors with the same value
 *     -2 if the cell already holds a value;
 *     obibous_value if there is
 * @param number_of_values - holds the number of options of values for the cell, a counter */

int get_obvious_value(Board *game_board, int col, int row)
{
    int n, temp_obvious_value, number_of_values;
    number_of_values = 0;
     temp_obvious_value = 0;
    /*if there is already a values, we don't want to change anything*/
    if (game_board->cells[row][col].value > 0)
    {
        return -2;
    }
    /*so the cell currently holds no values.
    iterating over all possible values */
    for (n = 1; n <= game_board->num_of_cols; ++n)
    {
        /*if we already reached more than one option for the cell,
          no point in continuing, cause there is more than one posibbility*/
        if (number_of_values > 1)
        {
            return 0;
        }
        /*needs to place the optinal value in the original board->cells,
            beacuse this is what is_in_neigobors needs...*/
        game_board->cells[row][col].value = n;
        /*if it is not in neigbors, so it is a legal value for this cell*/
        if (!is_in_neigbors(game_board, &(game_board->cells[row][col])))
        {
            temp_obvious_value = n;
            ++number_of_values;
        }
        /*beacuse of the first if in the method,
        shouldn't reach here if there is already a value in the cell originally
        so we want it empty for the next iteration*/
        game_board->cells[row][col].value = 0;
    }

    if (number_of_values == 1)
    {
        return temp_obvious_value;
    }
    return 0;
}

void copy_from_sol_to_board(Board *game_board)
{
    int row, col;
    for (row = 0; row < game_board->num_of_rows; ++row)
    {
        for (col = 0; col < game_board->num_of_cols; ++col)
        {
            game_board->cells[row][col].value = game_board->cur_sol[row][col];
            game_board->cells[row][col].is_fixed = FALSE;
            game_board->cells[row][col].is_valid = TRUE;
            game_board->cells[row][col].is_set_by_user = FALSE;
        }
    }
}

/*@note to be used only in solve mode
 *@note should copy the board in order to work on a non changing board
 *$ret BOARD_ERRONEUOS when input is an erroneus board (has two neigbors with same value!)
       MEMORY_ALLOCATION_ERROR if a problem in this occured. need to check for this an
                                free upper alocations */

/*probably should be in main:*/
/*if(game_board->mode != SOLVE_STATE)
{
    return COMMAND_ONLY_AVAILABLE_IN_SOLVE_MODE;
}*/
ERROR autofill(Board *game_board)
{
    return autofill_inner(game_board, NULL, NULL);
}

ERROR init_ind_changed(int ***ind_changed, int autofill_counter)
{
    int j;
    int **index_changed;

    index_changed = malloc(sizeof(int *) * 3);
    if (!index_changed)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    /*init three*/
    for (j = 0; j < 3; ++j)
    {
        index_changed[j] = malloc(sizeof(int) * autofill_counter);
        if (!index_changed[j])
        {
            free(index_changed);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    *ind_changed = index_changed;
    return SUCCESS;
}
ERROR autofill_inner(Board *game_board, int *changes_counter, int ***ind_changed)
{
    int i, j, obvious_value, changes_cnt;
    Cell **copied_cells;
    ERROR error;
    /*if the board is erouneous, no point in autofill*/
    if (is_board_erroneus(game_board))
    {
        return BOARD_ERRONEUOS;
    }

    /*init cells memory-wise*/
    if (init_cells(&copied_cells, game_board->num_of_rows, game_board->num_of_cols) == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    /*we need to copy board->cells to copied_cells, question the copy and change the original board*/
    error = copy_cells(game_board->cells, copied_cells, game_board->num_of_cols, NULL);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    /*check if there is an obvious feeling to board through row or col or block*/
    /*iterate over the board.
     *for each empty cell, check if there is only one option for it with one row,col,block
     *check that */

    changes_cnt = 0;
    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        for (j = 0; j < game_board->num_of_cols; ++j)
        {
            obvious_value = get_obvious_value(game_board, j, i);

            if (obvious_value > 0)
            {
                ++changes_cnt;
                copied_cells[i][j].value = obvious_value;
                copied_cells[i][j].is_set_by_user = obvious_value; /*TODO:maybe to change it???*/
            }
        }
    }
    if (changes_counter && ind_changed)
    {
        *changes_counter = changes_cnt;
        /*allocate space to ind_changes, which hold all coordinates that 
        are changed during autofill...*/
        error = init_ind_changed(ind_changed, *changes_counter);
        if (error == MEMORY_ALLOCATION_ERROR)
        {
            free_cells(copied_cells, game_board->num_of_rows);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    error = copy_cells(copied_cells, game_board->cells, game_board->num_of_cols, ind_changed ? *ind_changed : NULL);
    if (error == MEMORY_ALLOCATION_ERROR)
    {
        /* TODO: maybe free here? */
        return MEMORY_ALLOCATION_ERROR;
    }
    /*free copied_cells*/
    free_cells(copied_cells, game_board->blocks_per_cols);
    return SUCCESS;
}

/*param make_fixed will be 1 for fix and 0 for unfix*/
void fix_all_cells(Board *board, BOOL make_fixed)
{
    int r, c;
    for (r = 0; r < board->num_of_rows; ++r)
    {
        for (c = 0; c < board->num_of_cols; ++c)
        {
            if (board->cells[r][c].value != 0)
            {
                board->cells[r][c].is_fixed = make_fixed;
            }
        }
    }
}

/*if we got here we know that in the row of the cell there are 2 cells at laest with the value of the cell*/
void set_validity_row(Board *game_board, Cell *cell)
{
    int row, ind;
    row = cell->coordinates.x;

    for (ind = 0; ind < game_board->num_of_cols; ++ind)
    {
        if (game_board->cells[row][ind].value == cell->value)
        {
            game_board->cells[row][ind].is_valid = FALSE;
        }
    }
}

/*if we got here we know that in the row of the cell there are 2 cells at laest with the value of the cell*/
void set_validity_col(Board *game_board, Cell *cell)
{
    int col, ind;
    col = cell->coordinates.y;

    for (ind = 0; ind < game_board->num_of_rows; ++ind)
    {
        if (game_board->cells[ind][col].value == cell->value)
        {
            game_board->cells[ind][col].is_valid = FALSE;
        }
    }
}

void set_validity_block(Board *game_board, Cell *cell)
{
    int i, j, block_row_size, block_col_size;
    Point upper_left_point;
    upper_left_point = blocks_neigbor_range(game_board, cell);

    block_row_size = game_board->blocks_per_rows;
    block_col_size = game_board->blocks_per_cols;

    for (i = upper_left_point.y; i < (upper_left_point.y + block_row_size); ++i)
    {
        for (j = upper_left_point.x; j < (upper_left_point.x + block_col_size); ++j)
        {
            /*if it is not the original coordinates and the neigbor hold the same value*/
            if (i == cell->coordinates.y && j == cell->coordinates.x)
            {
                continue;
            }
            else if (game_board->cells[i][j].value == cell->value)
            {
                game_board->cells[i][j].is_valid = FALSE;
            }
        }
    }
}

void reset_validity_all_cells(Board *game_board)
{
    int r, c;
    for (r = 0; r < game_board->num_of_rows; ++r)
    {
        for (c = 0; c < game_board->num_of_rows; ++c)
        {
            game_board->cells[r][c].is_valid = TRUE;
        }
    }
}

BOOL clash_with_fixed_cell(Board *game_board, Cell *cell)
{
    int i, j, row, col, block_row_size, block_col_size;
    Point upper_left_point;

    /*check row clash*/
    row = cell->coordinates.y;
    for (i = 0; i < game_board->num_of_cols; ++i)
    {
        if (i == cell->coordinates.x)
        {
            continue;
        }
        else if (cell->value == game_board->cells[row][i].value && game_board->cells[row][i].is_fixed)
        {
            return TRUE;
        }
    }

    /*check col clash*/
    col = cell->coordinates.x;

    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        if (i == cell->coordinates.y)
        {
            continue;
        }
        else if (cell->value == game_board->cells[i][col].value && game_board->cells[i][col].is_fixed)
        {
            return TRUE;
        }
    }

    /*check block clash*/
    upper_left_point = blocks_neigbor_range(game_board, cell);

    block_row_size = game_board->blocks_per_rows;
    block_col_size = game_board->blocks_per_cols;

    for (i = upper_left_point.y; i < (upper_left_point.y + block_row_size); ++i)
    {
        for (j = upper_left_point.x; j < (upper_left_point.x + block_col_size); ++j)
        {
            /*if it is not the original coordinates and the neigbor hold the same value*/
            if (i == cell->coordinates.y && j == cell->coordinates.x)
            {
                continue;
            }
            else if (game_board->cells[i][j].value == cell->value && game_board->cells[i][j].is_fixed)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*we assume clash with fixed cell was already checked,
 so if we got here 'set' func should return success and
    thus we can change the validity of the cells
    
    assumes also fixed cells ahould be marked*/
void mark_visble_errors(Board *game_board)
{
    int c, r;
    Cell *cell;

    /*makes all cell. cell->is_valid = TRUE;*/
    reset_validity_all_cells(game_board);

    /*now, iterate over the board and change the invalid ones*/
    for (r = 0; r < game_board->num_of_rows; ++r)
    {
        for (c = 0; c < game_board->num_of_cols; ++c)
        {
            cell = &game_board->cells[r][c];
            if(cell->value == 0)
            {
                continue;
            }
            
            if(is_in_neigbors(game_board, cell))
            {
                cell->is_valid = FALSE;
            }
        }
    }
}

int count_empty_cells(Board *game_board)
{
    int row, col, empty_cells_counter = 0;
    for (row = 0; row < game_board->num_of_rows; ++row)
    {
        for (col = 0; col < game_board->num_of_cols; ++col)
        {
            if (game_board->cells[row][col].value == 0)
            {
                ++empty_cells_counter;
            }
        }
    }
    return empty_cells_counter;
}

