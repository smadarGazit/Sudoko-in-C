#ifndef GAME_H
#define GAME_H

#include "typedefs.h"
#include "board.h"
#include "errors.h"
#include "move.h"
#include "file_handler.h"


/*@brief - take the input from turn_input, that holds the command and the inputs relevent to it,
    apply the command if it is availble.
 *@returns the suitabale Error if ocurred in one of the the commands,
 *or COMMAND_UNAVAILABLE if the command that was chosen is no longer available (==board is finished)
  or SUCCESS if everything went fine
 *@param game_board is a pointer of the board
 *@param input arr is the out array of turn_input, that holds the inputs from the user*/
ERROR command_exe(Board *game_board, Move* user_move);
ERROR exit_game(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR set(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR validate(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR hint(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR solve(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR edit(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR mark_errors(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR guess(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR generate(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR undo(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR redo(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR save(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR guess_hint(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR num_solutions(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR print_board_command(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR autofill_command(Board* game_board, int x, int y, int value, double threshold, char *path);
ERROR un_autofill(Board *game_board, List_node *node_to_undo);
ERROR un_generate(Board *game_board, List_node *node_to_undo);
ERROR un_set(Board *game_board, List_node *node_to_undo);
ERROR redo_set(Board *game_board, List_node *node_to_redo);
ERROR redo_autofill(Board *game_board, List_node *node_to_redo);
ERROR redo_generate(Board *game_board, List_node *node_to_redo);
ERROR un_guess(Board *game_board, List_node *node_to_undo);

ERROR reset(Board *game_board, int x, int y, int value, double threshold, char *path);
ERROR redo_guess(Board *game_board, List_node *node_to_redo);

#endif