#ifndef PARSER_H_   /* Include guard */
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "move.h"
#include "errors.h" 


#define MAX_COMMAND_LEN 256

/*recives the string we got from the user.
matchs the first word to a command and the following words to parameters*/
ERROR parse_user_input(char *user_input, Move *user_move);

/*----------------------HELP-METHODS---------------------------------*/

/*manually convert string to integer, and insert the recived number to the relevant parameter in the move.
if the string didn't repesent the a number, enter NON_NUMERIC error to the relevant param*/
ERROR string_to_int(char *token, Move_parameter *move_parameter); 

/*manually convert string to double, and returns the recived number.
if the string didn't repesent the a double, enter NON_NUMERIC error to the relevant param */
double string_to_double(char *token, Move *move);

 /* relevant for "edit" only */
/* checks if the number of params entered is 0 or 1 (if not, returns an error). If 1 param is found,
   enters it to the path field in the move recived as input */ 
ERROR optional_param(Move *move); 

/* relevant for commands that don't require input at all */
/* checks if the number of params entered is 0 (if not, returns an error)*/
ERROR zero_param(Move *move);

/* relevant for "solve", "mark_errors", "guess", "save" */
/* checks if the number of params entered is 1 (if not, returns an error). If 1 param is found,
   enters it to the move recived as input */ 
ERROR one_param(Move *move);

/*relevant for all oher commands. inserts the values to the relevant parameter in the move.*/
/* checks if the number of params entered (2 or 3) - if not, returns an error. the correct num of params was found,
   enters it to the move recived as input */ 
ERROR get_params(Move *move); 
 
#endif
