#ifndef ILP_SOLVER
#define ILP_SOLVER


#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include <math.h>
#include "board.h"
#include "point.h"
#include "main_aux.h"
#include "time.h"
#include "Gurobi.h"


typedef enum{
    ILP = 0,
    LP_GUESS,
    LP_GUESS_HINT
}SOLVE_METHOD;
/*#include "gurobi_c.h"*/

ERROR get_sol_ilp_lp(Board *board, SOLVE_METHOD solve_method,  double threshold, int row, int col, int *val_op, double *val_chance);


/*3 lists
 * 1 - val
 * 2 - col
 * 3 - row
 * @brief - allocates place to maximun size for
 *  the matrix above, go over the board and make to 
 * diferent options for specific board[row][col] 
 * ret - the size of each list*/
ERROR get_variable_matrix(Board* board, int ***out_varibale_mat, int *list_length);

ERROR set_constraints_rows(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int *ind, double *val, int list_length);

ERROR set_constraints_cols(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, int list_length);


ERROR set_constraints_blocks(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, int list_length);
ERROR set_constraints_all_cells(GRBenv *env, Board *board, int** variable_mat ,GRBmodel *model, int* ind, double *val, int list_length);

ERROR load_sol_to_matrix(Board *board, int **varibale_mat,int list_length, double *sol);



ERROR enter_constraint_to_model(GRBenv *env, GRBmodel *model, int* ind, double *val, int cur_ind);
/*@brief finds the index of the first occurence of row_wanted in variable_mat[2] */
int find_start(int** variable_mat,int list_length, int row_wanted,int col_wanted, BOOL is_row);
/*
ERROR optimize(GRBenv *env, Board *board, int **varibale_mat,int list_length, double *sol, GRBmodel *model, int *optimstatus, BOOL is_integer, double threshold, int *val_op, double *val_chance, int row, int col);
*/
ERROR optimize(GRBenv *env,GRBmodel *model, int *optimstatus);
ERROR get_optimize_sol(GRBenv *env,Board *board, int **varibale_mat,int list_length, double *sol, GRBmodel *model, SOLVE_METHOD solve_method, double threshold, int *val_op, double *val_chance, int row, int col);
/*initialize the objective function for the sudoku
x(1)+x(2)+...x(var_num)*/
/* TODO: should be double** instead, and so on */
ERROR enter_objective_func_ilp(GRBenv *env, GRBmodel *model, double **obj, char **vtype, int list_length);

/*initialize the objective function for the sudoku
x(1)+x(2)+...x(var_num)*/
/* TODO: same as for ilp, should be double** */
ERROR enter_objective_func_lp(GRBenv *env, GRBmodel *model, double **obj, char **vtype, int list_length);


ERROR init_gurobi(GRBenv **env, GRBmodel **model);

void free_gurobi(GRBenv *env, GRBmodel *model, double *sol, double *obj, char *vtype,
                   int** variable_mat, int *ind, double *val);


ERROR gurobi_init_vars(GRBenv *env, GRBmodel *model, int var_number, double *obj, char *vtype);
#endif