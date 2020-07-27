
#include "typedefs.h"
#include "ilp_solver.h"
#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include <math.h>
#include "board.h"
#include "point.h"
#include "main_aux.h"
#include "time.h"
#include "cell.h"

/*#include "gurobi_c.h"*/

void print_var_matrix(int **mat, int list_length)
{
    int i;
    for(i = 0; i < list_length; ++i)
    {
        if(mat[0][i] != 0)
        {
            printf("row %d, col %d: %d \n", mat[2][i], mat[1][i], mat[0][i]);
            printf("\n-------\n");
        }
        else
        {
            break;
        }
    }
}

ERROR get_sol_ilp_lp(Board *board,SOLVE_METHOD solve_method,  double threshold, int row, int col, int *val_op, double *val_chance)
{
    GRBenv   *env;
    GRBmodel *model;
    double    *sol;
    /*the optimum function. each index relates to xi (a variavle)
     *obj[i] it is its coefficient*/
    double    *obj;
    /*defining the type of every variable*/
    char      *vtype;
    int       optimstatus;
    int** variable_mat;
    ERROR res;
    int *ind;
    int list_length;
    double *val;
    if (is_board_erroneus(board))
    {
        return BOARD_ERRONEUOS;
    }
    /*make cur_sol all zeros - easier to debug*/
    restart_cur_sol(board);

    /*alocate in, val*/
    ind = calloc(board->num_of_rows,sizeof(int));

    if(!ind)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    
    val = calloc(board->num_of_rows,sizeof(double));
    if(!val)
    {
        free(ind);
        return MEMORY_ALLOCATION_ERROR;
    }

    /*init model, env*/
    res = init_gurobi(&env, &model);

    /*if init_gurobi failed*/
    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        return res;
    }
 
    /*loading the helping matrix
    init varivale matrix*/
    res = get_variable_matrix(board, &variable_mat, &list_length);
    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        return MEMORY_ALLOCATION_ERROR;
    }
    
    /*init obj, vtype*/
    if (solve_method == ILP)
    {
        res = enter_objective_func_ilp(env, model, &obj, &vtype, list_length);
    }
    else
    {
        res = enter_objective_func_lp(env, model, &obj, &vtype, list_length);
    }

    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        return res;
    }

    res = set_constraints_rows(env, board, variable_mat ,model, ind, val, list_length);

    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        free(obj);
        free(vtype);
        return res;
    }
    
    res = set_constraints_cols(env, board, variable_mat ,model, ind, val, list_length);
    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        free(obj);
        free(vtype);
        return res;
    }

    
    res = set_constraints_blocks(env, board, variable_mat ,model, ind, val, list_length);
    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        free(obj);
        free(vtype);
        return res;
    }

    res = set_constraints_all_cells(env, board, variable_mat ,model, ind, val, list_length);
    if(res != SUCCESS)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        free(obj);
        free(vtype);
        return res;
    }
    /*malloc sol*/
    sol = calloc(list_length, sizeof(double));
    if(!sol)
    {
        free(ind);
        free(val);
        GRBfreeenv(env);
        GRBfreemodel(model);
        free_mat(variable_mat, 3);
        free(obj);
        free(vtype);
        return MEMORY_ALLOCATION_ERROR;
    }
    
    res = optimize(env, model, &optimstatus);
    if (res == SUCCESS)
    {
        res = get_optimize_sol(env, board, variable_mat, list_length, sol, model, solve_method, threshold, val_op, val_chance, row, col);
    }

    free_gurobi(env, model, sol, obj, vtype, variable_mat, ind, val);
    return res;
}

/*3 lists
 * 1 - val
 * 2 - col
 * 3 - row
 * @brief - allocates place to maximun size for
 *  the matrix above, go over the board and make to 
 * diferent options for specific board[row][col] 
 * ret - the size of each list*/
ERROR get_variable_matrix(Board* board, int ***out_variable_mat, int *list_length)
{
    int row,col,n, length_counter;
    int i;
    Board *copied_board;
    int **res_variable_mat;
    int current_size = 16;
    length_counter = 0;
    /*init out_variable_mat*/
    res_variable_mat = calloc(3, sizeof(int*));
    if (!res_variable_mat)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    
    for(row = 0; row < 3; ++row)
    {
        res_variable_mat[row] = calloc(current_size, sizeof(int));
        if (!res_variable_mat[row])
        {
            free(res_variable_mat);
            return MEMORY_ALLOCATION_ERROR;   
        }
    }
    /*so we can autofill it...*/
    copied_board = copy_board(board);
    /*fill cells with only one option*/
    autofill(copied_board); 
    /*for each cell*/
    for (row = 0; row < board->num_of_rows; ++row)
    {
        for (col = 0; col < board->num_of_cols; ++col)
        {
            if (copied_board->cells[row][col].value > 0)
            {
                board->cur_sol[row][col] = copied_board->cells[row][col].value;
            }

            if(copied_board->cells[row][col].is_fixed || copied_board->cells[row][col].is_set_by_user)
            {
                continue;
            }

            /*enter every option of cells[i][j] to variable mat*/
            for (n = 1; n <= board->num_of_rows; ++n)
            {
                copied_board->cells[row][col].value = n;
                /*if we found a new posible value:*/
                if(!is_in_neigbors(copied_board, &copied_board->cells[row][col]))
                {
                    if (length_counter == current_size)
                    {
                        current_size <<= 1;
                        for(i = 0; i < 3; ++i)
                        {
                            res_variable_mat[i] = realloc(res_variable_mat[i], current_size * sizeof(int));
                        }
                    }
                    res_variable_mat[0][length_counter] = n;/*add  the value*/
                    res_variable_mat[1][length_counter] = col;/*col index*/
                    res_variable_mat[2][length_counter] = row;/*row index*/
                    ++length_counter;
                }
            }
            copied_board->cells[row][col].value = 0;
        }
    }
    *out_variable_mat = res_variable_mat;
    *list_length = length_counter;
    free_all_board_memory(copied_board);
    return SUCCESS;
}


ERROR set_constraints_rows(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int *ind, double *val, int list_length)
{
    int row ,n, error;
    int cur_var_mat;/*current position in list of variable_mat*/
    int cur_ind;/*holds the position we curently in, in ind and val */
    int row_start;/*variable_mat[2][row_start] means row i starts in row_start*/
    cur_ind = 0;

    /*iterating over the rows, in order to set a all the contraints
    * suitable for each row*/
    for(row = 0; row < board->num_of_rows; ++row)
    {
        /*find the first index in variable_mat[2] that refers to row i*/
        row_start = find_start(variable_mat,list_length, row, -1, TRUE);
        /*if this row is not in varaibale_mat[2]*/
        if(row_start < 0)
        {
            continue;
        }
        /*we starting a new constraint for row i, then we need:*/

        /*for each posible value (from 1 to n, when n is the maximal digit of sudoku)*/
        for(n = 0; n < board->num_of_rows; ++n)
        {
            cur_var_mat = row_start;
            /*finding all the options to place 'n' in row i*/
            while(variable_mat[2][cur_var_mat] == row && cur_var_mat < list_length)
            {
                if(variable_mat[0][cur_var_mat] == n)
                {
                    /*adding to the cuurent constraint*/
                    ind[cur_ind] = cur_var_mat;
                    val[cur_ind] = 1.0;
                    ++cur_ind;
                }
                ++cur_var_mat;
            }
            
            /*if there are no options with 'n' for this row..*/
            if (cur_ind == 0)
            {
                continue;
            }
            error = enter_constraint_to_model(env, model, ind, val, cur_ind);
            if (error == ERROR_GUROBI)
            {
                return ERROR_GUROBI;
            }
            cur_ind = 0;
        }
    }

    return SUCCESS;
}

ERROR set_constraints_cols(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, int list_length)
{
    int cur_var_mat,n,col,cur_ind;
    ERROR res;
    cur_ind = 0;
    n = 0;

    /*iterating over the cols, in order to set a all the contraints
    * suitable for each col*/
    for(col = 0; col < board->num_of_cols; ++col)
    {
        /*for each posible value (from 1 to n, when n is the maximal digit of sudoku*/
        for(n = 1; n <= board->num_of_rows; ++n)
        {
            /*iterating over variable_mat*/
            for (cur_var_mat = 0; cur_var_mat < list_length; ++cur_var_mat)
            {
                if(variable_mat[0][cur_var_mat] == n && variable_mat[1][cur_var_mat] == col)
                {
                    /*adding to the cuurent constraint*/
                    ind[cur_ind] = cur_var_mat;
                    val[cur_ind] = 1.0;
                    ++cur_ind;
                }
                
            }
            
            /*if there are no options with 'n' for this col..*/
            if (cur_ind == 0)
            {
                continue;
            }

            res = enter_constraint_to_model(env, model, ind, val, cur_ind);
            if (res == ERROR_GUROBI)
            {
                return ERROR_GUROBI;
            }
            cur_ind = 0;
        }
        
    }

    return SUCCESS;
}


ERROR set_constraints_blocks(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, int list_length){

    int cur_var_mat, n, block, cur_ind;
    Point *block_list;
    int col_left_border, col_right_border, row_up_border, row_down_border=1;

    block_list = calloc(board->blocks_per_cols * board->blocks_per_rows, sizeof(Point));
    if(!block_list)
    {
        return MEMORY_ALLOCATION_ERROR;
    }    
    cur_ind = 0;

    find_all_upper_left(board, block_list); 
    /*for each option from 1 to n*/
    for(n = 1; n <= board->num_of_rows; ++n)
    {
        /*for each block*/
        for(block = 0; block < board->blocks_per_cols * board->blocks_per_rows; ++block)
        {   
        /*col_left_border, row_up_border are part of the block, the others are of the next block */
            /*if two adjacent blocks are in the same tow in the board...*/
            col_left_border = block_list[block].x;
            if(block_list[block].x + board->blocks_per_cols < board->num_of_cols)
            {
                col_right_border = block_list[block].x + board->blocks_per_cols;
            }
            else
            {
                col_right_border = board->num_of_cols;
            }
            row_up_border = block_list[block].y;

            if(block_list[block].y + board->blocks_per_rows < board->num_of_rows)
            {
                row_down_border = block_list[block].y + board->blocks_per_rows;
            }
            else
            {
                col_right_border = board->num_of_rows;
            }

            for(cur_var_mat = 0; cur_var_mat < list_length; ++cur_var_mat)
            {

                if(variable_mat[0][cur_var_mat] != n)
                {
                    continue;
                }

                if(variable_mat[2][cur_var_mat] >= row_up_border && variable_mat[2][cur_var_mat] < row_down_border &&
                   variable_mat[1][cur_var_mat] >= col_left_border && variable_mat[1][cur_var_mat] < col_right_border)
                   {
                        ind[cur_ind] = cur_var_mat;
                        val[cur_ind] = 1.0;
                        ++cur_ind;
                   }
            }
            /*if there are no options with 'n' for this col..*/
            if (cur_ind == 0)
            {
                continue;
            }
            if (enter_constraint_to_model(env, model, ind, val, cur_ind) == ERROR_GUROBI)
            {
                return ERROR_GUROBI;
            }
            cur_ind = 0;
        }
    }
    /*free_point_mat(block_list, board->blocks_per_cols * board->blocks_per_rows);*/
    free(block_list);
    return SUCCESS;
}

ERROR set_constraints_all_cells(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, 
    int list_length)
{
    int row, cur_var_mat ,cur_ind, col;
    cur_ind = 0;

    /*iterating over*/
    for(row = 0; row < board->num_of_rows; ++row)
    {
        /*for each cell in this row (go over cells in row from col number 0 to num_of_cols)*/
        for(col = 0; col < board->num_of_cols; ++col)
        {
            cur_var_mat = find_start(variable_mat, list_length, row, col, FALSE);
            
            if(cur_var_mat < 0)
            {
                continue;
            }
            /*we want a new constraint for cells[row][col]*/
        
            /*while we are in the same row, find all the options that are relevant for this cell (cell[row][col])*/
            while(cur_var_mat < list_length && variable_mat[1][cur_var_mat] == col && variable_mat[2][cur_var_mat] == row)
            {
                /*adding to the cuurent constraint*/
                ind[cur_ind] = cur_var_mat;
                val[cur_ind] = 1.0;
                ++cur_ind;
                ++cur_var_mat;
            }
            
            /*if there are no options with 'n' for this row..*/
            if (cur_ind == 0)
            {
                continue;
            }

            if (enter_constraint_to_model(env, model, ind, val, cur_ind) == ERROR_GUROBI)
            {
                return ERROR_GUROBI;
            }

            cur_ind = 0;
        }
    }
    return SUCCESS;
}

ERROR load_sol_to_matrix(Board *board, int **varibale_mat,int list_length, double *sol)
{
    int i, val, col, row;
    for (i = 0; i < list_length; ++i)
    {
        if(sol[i] == 1)
        {
            val = varibale_mat[0][i];
            col = varibale_mat[1][i];
            row = varibale_mat[2][i];
            board->cur_sol[row][col] = val;
        }
    }

    return SUCCESS;
}


ERROR enter_constraint_to_model(GRBenv *env, GRBmodel *model, int* ind, double *val, int cur_ind){
    /*add the constaint to the model*/
    int error;
    error = GRBaddconstr(model, cur_ind , ind, val, GRB_EQUAL, 1.0, NULL);/*if doesnt work - to change to GRB.EQUAL*/
    
    if (error) 
    {
        printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
        return ERROR_GUROBI;
    }

    return SUCCESS;
}



/*@brief finds the index of the first occurence of row_wanted in variable_mat[2] */
int find_start(int** variable_mat,int list_length, int row_wanted,int col_wanted, BOOL is_row)
{
    int cur_var_mat;
    
    for(cur_var_mat = 0; cur_var_mat < list_length; ++cur_var_mat)
    {
        if(variable_mat[2][cur_var_mat] == row_wanted && variable_mat[0][cur_var_mat] != 0)
        {
            if (is_row)
            {
                return cur_var_mat;
            }
            else
            {
                break;
            }
        }
    }
    if(!is_row)
    {
        while(cur_var_mat < list_length && variable_mat[2][cur_var_mat] == row_wanted)
        {
            if(variable_mat[1][cur_var_mat] == col_wanted)
            {
                return cur_var_mat;
            }
            else
            {
                ++cur_var_mat;
            }
        }
    }
    return -1;
}

ERROR build_guess_hint(Board *game_board, int **varibale_mat,int list_length, double *sol, double threshold, int* val_op, double *val_chance, int row, int col){
    int *rows, *cols, *vals; 
    int ind_start, ind_end;
    
    rows = varibale_mat[2];
    cols = varibale_mat[1];
    vals = varibale_mat[0];

    count_cell_options(rows, cols, list_length, row , col, threshold, sol, &ind_start, &ind_end, game_board, vals);
    group_options(val_op, val_chance, sol, vals, ind_start, ind_end, threshold, game_board, row, col);

    return SUCCESS;
}


ERROR build_guess(Board *game_board, int **varibale_mat,int list_length, double *sol, double threshold){
    int i, j, *rows, *cols, *vals; 
    int number_of_options;
    int ind_start, ind_end;
    int *options;
    double *options_chance;
    
    rows = varibale_mat[2];
    cols = varibale_mat[1];
    vals = varibale_mat[0];
    for (i=0; i<game_board->num_of_rows; ++i){
        for (j=0; j<game_board->num_of_cols; ++j){
            if (game_board->cells[i][j].value != 0){
                continue;
            }

            number_of_options = count_cell_options(rows, cols, list_length, i, j, threshold, sol, &ind_start, &ind_end, game_board, vals);
            if (number_of_options==0){
                continue;
            }

            options_chance = calloc(number_of_options,sizeof(double));
            if(!options_chance)
            {
                return MEMORY_ALLOCATION_ERROR;
            }
            options = calloc(number_of_options, sizeof(int));
            if(!options)
            {   
                free(options_chance);
                return MEMORY_ALLOCATION_ERROR;
            }

            group_options(options, options_chance, sol, vals, ind_start, ind_end, threshold, game_board, i,j);
            if (number_of_options == 1){
                game_board->cells[i][j].value = options[0];
            }
            else{
                game_board->cells[i][j].value = weighted_random_choice(options, options_chance, number_of_options);
            }
            
            free(options);
            free(options_chance);
        }
    }

    return SUCCESS;
}

/*previous declaration:
  ERROR optimize(GRBenv *env, Board *board, int **varibale_mat,int list_length, double *sol, GRBmodel *model, int *optimstatus, BOOL is_integer, double threshold, int *val_op, double *val_chance, int row, int col)
*/
ERROR optimize(GRBenv *env,GRBmodel *model, int *optimstatus)
{
    int error;
    
      /* Optimize model - need to call this before calculation */
     error = GRBoptimize(model);
     if (error) 
    {
    	  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
    	  return ERROR_GUROBI;
    }

    /* Write model to 'mip1.lp' - this is not necessary but very helpful */
    error = GRBwrite(model, "mip1.lp");
    if (error)
    {
    	  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
    	  return ERROR_GUROBI;
    }

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, optimstatus);
    if (error)
    {
    	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
    	  return ERROR_GUROBI;
    }
    /*---------------------------------------*/
    
    /* solution found */
    if (*(optimstatus) == GRB_OPTIMAL) 
    {    
        return SUCCESS;
    }

    /* no solution found */
    if (*(optimstatus) == GRB_INF_OR_UNBD || *(optimstatus) == GRB_INFEASIBLE) 
    {
        return ERROR_NO_SOLUTION;
    }
    
    /* error or calculation stopped */
    return ERROR_GUROBI;
}



ERROR get_optimize_sol(GRBenv *env,Board *board, int **varibale_mat,int list_length, double *sol, GRBmodel *model, SOLVE_METHOD solve_method, double threshold, int *val_op, double *val_chance, int row, int col)
{
    int grb_res;
    ERROR res = SUCCESS;
 
    /* get the solution - the assignment to each variable */
    /* list_length -- number of variables, the size of "sol" should match */
    grb_res = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, list_length, sol);
    if (grb_res) 
    {
        printf("ERROR %d GRBgetdblattrarray(): %s\n", grb_res, GRBgeterrormsg(env));
        return ERROR_GUROBI;
    }
    
    /* solution found */
    if (solve_method == ILP){
        load_sol_to_matrix(board, varibale_mat,list_length, sol);
    }
    else if (solve_method == LP_GUESS){
        res = build_guess(board,varibale_mat,list_length,sol,threshold);
    }
    else{
        res = build_guess_hint(board,varibale_mat,list_length,sol,threshold,val_op, val_chance, row, col);
    }
    
    return res;
}






/*initialize the objective function for the sudoku
x(1)+x(2)+...x(var_num)*/
/* TODO: should be double** instead, and so on */
ERROR enter_objective_func_ilp(GRBenv *env, GRBmodel *model, double **obj, char **vtype, int list_length)
{
    int i, error;
    double *res_obj;
    char *res_vtype;

    res_obj = malloc(sizeof(double) * list_length);
    if(!res_obj)
    {   
        return MEMORY_ALLOCATION_ERROR;
    }

    res_vtype =  malloc(sizeof(char) * list_length);
    if(!res_vtype)
    {
        free(res_obj);
        return MEMORY_ALLOCATION_ERROR;
    }
    
    for(i = 0; i < list_length; ++i)
    {
        res_obj[i] = 1;
        res_vtype[i] = GRB_BINARY;
    }

    *obj = res_obj;
    *vtype = res_vtype;

    error = gurobi_init_vars(env, model, list_length, res_obj, res_vtype);
    if (error != SUCCESS)
    {
        free(res_obj);
        free(res_vtype);
    }

    return error;
}

/*initialize the objective function for the sudoku
x(1)+x(2)+...x(var_num)*/
/* TODO: same as for ilp, should be double** */
ERROR enter_objective_func_lp(GRBenv *env, GRBmodel *model, double **obj, char **vtype, int list_length)
{
    int i;
    int error;
    int error2;
    double val[1];
    int ind[1];
    time_t t;
    double *res_obj;
    char *res_vtype;
    srand((unsigned) time(&t));

    res_obj = malloc(sizeof(double) * list_length);
    if(!res_obj)
    {   
        return MEMORY_ALLOCATION_ERROR;
    }

    res_vtype =  malloc(sizeof(char) * list_length);
    if(!res_vtype)
    {   
        free(res_obj);
        return MEMORY_ALLOCATION_ERROR;
    }
    
    for(i = 0; i < list_length; ++i) /*TODO - need to understand how to assign the values!*/
    {
        res_obj[i] = (rand()%100)/100;
        res_vtype[i] = GRB_CONTINUOUS;
    }

    error = gurobi_init_vars(env, model, list_length, res_obj, res_vtype);
    if (error)
    {
        free(res_obj);
        free(res_vtype);
        return ERROR_GUROBI;
    }

    val[0] = 1;
    for (i = 0; i < list_length ; ++i)
    {
        ind[0] = res_obj[i];
        error = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1, "c0");
        error2 = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0, "c0");
        if (error || error2) 
        {
	        printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
            free(res_obj);
            free(res_vtype);
	        return ERROR_GUROBI;
        } 
    }
    
    *obj = res_obj;
    *vtype = res_vtype;

    return SUCCESS;
}


ERROR init_gurobi(GRBenv **env, GRBmodel **model)
{
    int error; 
    /* Create environment - log file is mip1.log */

    error = GRBloadenv(env, "mip1.log");
    if (error) 
    {
    	  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(*env));
          GRBfreeenv(*env);
    	  return ERROR_GUROBI;
    }
    
    /*tells gurobi to stop writing errors to the console*/
    error = GRBsetintparam(*env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) 
    {
    	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(*env));
          GRBfreeenv(*env);
    	  return ERROR_GUROBI;
    }
    
    /* Create an empty model named "mip1" */
    error = GRBnewmodel(*env, model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) 
    {
    	  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(*env));
          GRBfreeenv(*env);
          GRBfreemodel(*model);
    	  return ERROR_GUROBI;
    }
    return SUCCESS;
}

void free_gurobi(GRBenv *env, GRBmodel *model, double *sol, double *obj, char *vtype,
                   int** variable_mat, int *ind, double *val)
{
    free(ind);
    free(sol);
    free(obj);
    free(vtype);
    free_mat(variable_mat,3);
    
    free(val);
    GRBfreemodel(model);
    GRBfreeenv(env);
}


ERROR gurobi_init_vars(GRBenv *env, GRBmodel *model, int var_number, double *obj, char *vtype)
{
    int grb_res;
    
    grb_res = GRBaddvars(model, var_number, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
    if (grb_res)
    {
    	  printf("ERROR %d GRBaddvars(): %s\n", grb_res, GRBgeterrormsg(env));
    	  return ERROR_GUROBI;
    }

      /* Change objective sense to maximization */
    grb_res = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (grb_res)
    {
	    printf("ERROR %d GRBsetintattr(): %s\n", grb_res, GRBgeterrormsg(env));
	    return ERROR_GUROBI;
    }
    grb_res = GRBupdatemodel(model);
    if (grb_res) 
    {
	    printf("ERROR %d GRBupdatemodel(): %s\n", grb_res, GRBgeterrormsg(env));
	    return ERROR_GUROBI;
    }
    return SUCCESS;
}
