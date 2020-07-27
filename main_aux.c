#include "main_aux.h"
#include <stdio.h>
#include <time.h>
#include "UI.h"
/*#include "solver.h"*/
/*#include "parser.h"*/
#include "board.h"
#include "stack.h"
#include "commands.h"
#include "math.h"

extern int MAX_NUMxx;

void validate_params(Move *move, int *max_range, int parameter_num){
    int max = *max_range;
    if (move->COMMAND == GENERATE){
        max = max * max;
        *(max_range) = max;
    }
    if (move->param1.param_state == SUCCESS){
        if (move->param1.value<1 || move->param1.value>max){
            move->state = INDEVIDUAL_PARAM_ERROR;
            move->param1.param_state = OUT_OF_RANGE;
        }
    }
    if (move->param2.param_state == SUCCESS){
        if (move->param2.value<1 || move->param2.value>max){
            move->state = INDEVIDUAL_PARAM_ERROR;
            move->param2.param_state = OUT_OF_RANGE;
        }
    }
    if (parameter_num>2){
        if (move->param3.param_state == SUCCESS){
            if (move->param3.value<0 || move->param3.value>max){
                move->state = INDEVIDUAL_PARAM_ERROR;
                move->param3.param_state = OUT_OF_RANGE;
            }
        }
    }
}


ERROR validate_user_input(Board *game_board, Move *move){
    int max_range;
    char info[20] = {0};
    int parameter_num = get_param_num(move->COMMAND);
   
    max_range = game_board->num_of_rows;


    if (move->COMMAND == UNKNOWN){
        move->state = UNKNOWN_COMMAND;
        print_validation_error(move, NULL);
        return INPUT_ERROR;
    }

    if (!is_command_avilable(move->COMMAND, game_board->mode)){
        move->state = COMMAND_UNAVAILABLE;
        if (game_board->mode == INIT)
        {
            sprintf(info, "%s", "INIT");
    
        } 
        if (game_board->mode == EDIT_STATE)
        {
            sprintf(info, "%s", "EDIT");
        } 
        if (game_board->mode == SOLVE_STATE){
            sprintf(info, "%s", "SOLVE");
        } 
        print_validation_error(move, info); 
        return INPUT_ERROR;
    }

    if (move->state == TOO_FEW_PARAMS || move->state == TOO_MANY_PARAMS){
        print_validation_error(move, NULL);
        return INPUT_ERROR;
    }

    if (parameter_num==0){
        return SUCCESS;
    }
    
    if (move->COMMAND == MARK_ERRORS && (move->param3.value!=1 && move->param3.value!=0)){ 
        move->state = MARK_ERRORS_FAULT;
        print_validation_error(move, NULL);
        return INPUT_ERROR;
    }

    if (move->COMMAND == GUESS && (move->extraParam>=1 || move->extraParam<=0)){ 
        move->state = EXTRAPARAM_FAULT;
        print_validation_error(move, NULL);
        return INPUT_ERROR;
    }

    if (parameter_num>1){
        validate_params(move, &max_range, parameter_num);
        sprintf(info, "%d", max_range);
        print_validation_error(move, info);
        if (move->state!=SUCCESS){
            return INPUT_ERROR;
        }
    }
     
    return SUCCESS;
}


ERROR full_autofill(Board *game_board){
    int i, j, obvious_value;   

    /*check if there is an obvious feeling to board through row or col or block*/
    /*iterate over the board.
     *for each empty cell, check if there is only one option for it with one row,col,block
     *check that */ 
    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        for (j = 0; j < game_board->num_of_cols; ++j)
        {
            obvious_value = get_obvious_value(game_board, j, i);

            if(obvious_value > 0)
            {
                game_board->cells[i][j].value = obvious_value;
            } 
        }
    }

    return SUCCESS;
}

/*@brief - prints the board to screen*/
void print_int_mat(const Board *game_board)
{
    int i, j;
    int rows_in_block = game_board->num_of_rows / game_board->blocks_per_rows;
    int cols_in_block = game_board->num_of_cols / game_board->blocks_per_cols;
    int ** cur_sol = game_board->cur_sol;
    /*for each row*/
    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        if (i % rows_in_block == 0)
        {
            printf("----------------------------------\n");
        }
        /*for each cell in this row*/
        for (j = 0; j < game_board->num_of_cols; ++j)
        {
            if (j % cols_in_block == 0)
            {
                if (j==0){
                    printf("|");
                }
                else{
                printf(" |");
                }
            }
            if (game_board->cells[i][j].is_fixed)
            {
                printf(" .%d", cur_sol[i][j]);
            }
            else
            {
                if (cur_sol[i][j] == 0)
                {
                    printf("   ");
                }
                else
                {
                    printf("  %d", cur_sol[i][j]);
                }
            }
        }
        printf(" |\n");
    }
    printf("----------------------------------\n");
}

/*@brief - erases old sol*/
void restart_cur_sol(Board *board)
{
    int i,j;
    for(i = 0; i < board->num_of_rows; ++i)
    {
        for(j = 0; j < board->num_of_cols; ++j)
        {
            board->cur_sol[i][j] = 0;
        }
    }
}

/*@brief - free a matrix of size n*n */
void free_mat(int **matrix, int n)
{
    int i;
    for(i = 0; i < n; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void free_cells(Cell **matrix, int n)
{
    int i;
    for(i = 0; i < n; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void copy_int_mat(int **src, int **copy, int main_length, int secondary_length)
{
    int i, j;
    for (i = 0; i < main_length; ++i)
    {
        for (j = 0; j < secondary_length; ++j)
        {
            copy[i][j] = src[i][j];
        }
    }
}





void reset_cell_list(Board *game_board){
    free_cells(game_board->cells, game_board->num_of_rows);
    init_cells(&(game_board->cells),game_board->num_of_rows, game_board->blocks_per_cols);
    /*TODO - make sure init cells would return the cells reseted!*/
}

void choose_cells(Board *game_board, Point *points, int x){
    int r,c, i, length = 0;
    while(length!=x){
        c = rand()%game_board->num_of_cols;
        r = rand()%game_board->num_of_rows;
        
        for (i=0; i<length; ++i){
            if (points[i].x == r && points[i].y==c){
                continue;
            }
        }

        points[length] = new_point(r,c);
        ++length;
    }
}


void delete_Y_cells(Board *game_board, Point *to_delete, int y){
    int i;
    for (i=0; i<y; ++i){
        game_board->cells[to_delete[i].x][to_delete[i].y].value = 0;
    }
}


void delete_from_options(Cell *cell, BOOL *cell_options)
{
    cell_options[cell->value] = FALSE; /* delete from option */
    --(cell_options[0]); /* decrease number of options by 1 */
}

/* @brief iterate over game_board->cell.oprions and list them in options_in_func array*/
/* assumimng len(options in func) == the real number of options*/
void get_list_options(BOOL *cell_options, int *current_options, int max_value)
{
    int i, j = 0;
    for (i = 0; i < max_value; ++i) /* go over "options" array and whenever we see TRUE, copy index to the new array */
    {
        if (cell_options[i] == TRUE)
        {
            current_options[j] = i;
            j++;
        }
    }
}


/* @brief generates a random number, in respect to what is available in a current time */
int get_rand_value(BOOL *cell_options, int max_value)
{
    int ind_from_list_options;
    int res;
    int *current_options;


    if (cell_options[0] != 0) /*if the cell currently has options*/ 
    {
        current_options = calloc(cell_options[0], sizeof(int)); /* allocate a new array in the len of the number of valid options*/
        get_list_options(cell_options, current_options, max_value); /* load options to the shorter "fitted" array */
        if (cell_options[0] == 1) /* if only 1 option is available, use it without randomization */
        {
            res = current_options[0]; 
            free(current_options);
            return res;
        }
        /* more then 1 option is available */
        ind_from_list_options = rand() % cell_options[0]; /*choose an index randomaly */
        res = current_options[ind_from_list_options]; /*return the element that correspondes to the chosen index*/
        free(current_options);
        return res;
    }
    return 0; /* no options - return 0 */
}

int find_start_in_var_mat(int *rows, int *cols, int cell_row, int cell_col, int len){
    int i;
    for (i=0; i<len; ++i){
        if (rows[i]==cell_row && cols[i]==cell_col){
            return i;
        }
    }
    return -1; /*souldn't be reached..*/
}

int find_end_in_var_mat(int *rows, int *cols, int cell_row, int cell_col, int len, int start){
    int i;
    for (i=start; i<len; ++i){
        if (rows[i]!=cell_row || cols[i]!=cell_col){
            return i;
        }
    }
    return len; 
}

int count_options_inner(double *sol, int ind_start, int ind_end, double threshold, Board *game_board, int *vals, int row, int col){
    int count, i;
    i = ind_start;
    count = 0;
    while(i<ind_end){
        game_board->cells[row][col].value = vals[i];
        if (sol[i]>0.0 && sol[i]>=threshold && !is_in_neigbors(game_board, &game_board->cells[row][col])){
            ++count;
        }
        ++i;
    }
    game_board->cells[row][col].value = 0;
    return count;
}

void group_options(int *options, double *options_chance, double *sol, int *vals, int ind_start, int ind_end, double threshold, Board *board, int row, int col){
    int i, cur_ind;
    i = ind_start;
    cur_ind = 0;

    i = ind_start;
    while(i<ind_end){
        board->cells[row][col].value = vals[i];
        if (sol[i]>0.0 && sol[i]>=threshold && !is_in_neigbors(board, &board->cells[row][col])){
            options[cur_ind]=vals[i];
            options_chance[cur_ind] = sol[i];
            ++cur_ind;
        }
        ++i;
    }
    board->cells[row][col].value = 0;
}


/*previous to this method we skip all full cells, meaning that every cell that is treated by this methos is
    empty, and thus can be found in the rows&cols arrays*/
int count_cell_options(int *rows, int *cols, int len, int cell_row, int cell_col, double threshold, double *sol, int *strt, int *end, Board *board, int *vals)
{
    int ind_start, ind_end;

    ind_start = find_start_in_var_mat(rows, cols, cell_row, cell_col, len);
    ind_end = find_end_in_var_mat(rows, cols, cell_row, cell_col, len, ind_start);
    *strt = ind_start;
    *end = ind_end;
    
    return count_options_inner(sol, ind_start, ind_end, threshold, board,vals, cell_row, cell_col);

}

/*@brief counts the possible number of solutions for a given board*/
int count_solutions(Board *game_board)
{
    int counter = 0;
    Stack s;
    Cell* cell;
    init_stack(&s);
    push(&game_board->cells[0][0], &s);

    while(!stack_is_empty(&s))
    {
        pop(&s, &cell);
        if (cell == EOB || game_board->num_of_cols == 0 || game_board->num_of_rows == 0)
        { /*we enter here only if we moved forward from last cell<->if bord is solved*/
            ++counter;
            continue;
        }
        if (cell->is_fixed || cell->is_set_by_user)
        { /*if we encountered a fixed cell we should continue on the next one*/
            /*if we encountered a cell set by user we should continue on the next one*/
            push(next(game_board, cell), &s);
            continue;
        }
        /* So we have encountered a cell that is not set by user and is not fixed - we need to set its value*/
        ++cell->value;
        if (cell->value > MAX_NUMxx)
        { /*no options are available, backtrack*/
            cell->value = 0;
            continue;
        }
        push(cell, &s);
        if (!is_in_neigbors(game_board, cell))
        {
            /* So the current cell value is good!*/
            push(next(game_board, cell), &s);
        }
        /*  So the current cell value is not good... we should try a new value if possible
            this is the part in the presentstion where is says:
            "Invalid? Increment by one. Repeat"
        */
    }
    return counter;
}


int weighted_random_choice(int *options, double *options_chance, int length){ 
    int i; 
    double x;
    double *ranges, sum, res;

    /*build ranges array */
    ranges = malloc(sizeof(double)*length);
    sum = 0.0;

    for (i=0; i<length; ++i){
        sum += options_chance[i];
        ranges[i] = sum;
    }
     
     /*choose a random value from 0 to sum */
    x = (rand()/(double)RAND_MAX)*sum;

    i = 0;
    if(x>ranges[length-1]){
        res = options[length-1];
    }
    else if(x<ranges[0]){
        res = options[0];
    }
    else{
        while(x>ranges[i]){
            ++i;
        }
        res = options[i-1];
    }

    free(ranges);
    return res; 
}

void print_cur_sol(const Board *game_board)
{
    char *line;
    int i, j;
    int rows_in_block = game_board->num_of_rows / game_board->blocks_per_rows;
    int cols_in_block = game_board->num_of_cols / game_board->blocks_per_cols;
    int **cell_matrix = game_board->cur_sol;
    
    /*prepare line*/
    line = calloc((4*(game_board->num_of_rows)+game_board->blocks_per_rows+2),sizeof(char));
    for (i=0;i<4*(game_board->num_of_rows)+game_board->blocks_per_rows+1;++i){
        line[i] = '-';
    }
     line[i] = '\0';

    /*for each row*/
    for (i = 0; i < game_board->num_of_rows; ++i)
    {
        if (i % rows_in_block == 0)
        {
            printf(("%s\n"),line);
        }
        /*for each cell in this row*/
        for (j = 0; j < game_board->num_of_cols; ++j)
        {
            if (j % cols_in_block == 0)
            {
                if (j==0){
                    printf("|");
                    printf("  %2d", cell_matrix[i][j]);
                }
                else{
                printf(" |");
                printf("  %2d", cell_matrix[i][j]);
                }
            }

            else
            {
                if (cell_matrix[i][j] == 0)
                {
                    printf("  _");
                }
                else
                {
                    printf("  %2d", cell_matrix[i][j]);
                }
            }
        }
        printf(" |\n");
    }
    printf(("%s"),line);
}

