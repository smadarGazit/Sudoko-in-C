#include "game_flow.h"
#include "move.h"
#include "game.h"
#include "parser.h"
#include "board.h"
#include "commands.h"
#include "main_aux.h"
#include "modes.h"
#include "list.h"
#include "list_node.h"


int main_draft(){
    ERROR error;
    Move *user_move;
    Board *game_board;
    char user_input[MAX_COMMAND_LEN+1]; /*we want to check that in this range we see '\0', 
    since the user can write up to MAX_COMMAND_LEN chars, the "\n" char can appear at the
        MAX_COMMAND_LEN+1 place */
    
    error = SUCCESS;
    game_board = malloc(sizeof(Board));
    if(!game_board)
    {   
        print_error(MEMORY_ALLOCATION_ERROR);
        return MEMORY_ALLOCATION_ERROR;
    }
    new_board(game_board, 9, 9, 3,3); /* init a default board */
    user_move = malloc(sizeof(Move));
    if(!user_move)
    {
        free(game_board);
        print_error(MEMORY_ALLOCATION_ERROR);
        return MEMORY_ALLOCATION_ERROR;
    }

    print_welcome_messege();
    /*we exit in three cases: 1.user requested; 2: the input file ended; 3: mamory allocation problem*/
    while (error != EXIT_GAME && error != REACHED_EOF && error != MEMORY_ALLOCATION_ERROR){
        
        print_command_prompt_messege(); 

        error = get_user_input(user_input); /*read input*/
        if (error != SUCCESS){ /* reched end of a file/blank line was inserted/input is longer than 256 chars */
            print_error(error); 
            continue;
        }

         /*parse input and insert the requested command and parameters to a Move struct*/
        parse_user_input(user_input, user_move);

        /*checking command is execurable at the current state and the validity of the user 
        params. if an error is found, details are provided to the user*/
        error = validate_user_input(game_board, user_move); 
        if (error != SUCCESS){ 
            continue;
        }

        /*execute user command*/
        error = command_exe(game_board, user_move); 
        if (error != SUCCESS){
            print_error(error); /*if an error was found, notify user and explain*/
            continue;
        }
        
        /*if the changes to the board are "visble" (can seen by printing the values of the cells), print the board*/
        if (is_printable(user_move->COMMAND)){ 
            print_board(game_board); /* reched end of a file/blank line was inserted/input is longer than 256 chars */
        }
    }
    
    print_goodbye_messege();

    /*before exiting we free all the resources allocated in this func*/
    free_all_board_memory(game_board);
    free(game_board);
    free(user_move);

    return 0;
}




   