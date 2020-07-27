#include "commands.h"
#include "bool.h"

/*#include "move.h"
*/

#include "modes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*needs to add RESET as a command!*/

BOOL is_printable(Command command){
    switch (command)
    {
        case SET:
            return TRUE;
        case SOLVE:
            return TRUE;
        case EDIT:
            return TRUE;
        case UNDO:
            return TRUE;
        case REDO:
            return TRUE;
        case GUESS:
            return TRUE;
        case GENERATE:
            return TRUE;
        case AUTOFILL:
            return TRUE;
	case RESET:
            return TRUE;
        default:
            return FALSE;
    }
 
    return FALSE; /*shouldn't be reached - this func should be only activated on valid commands*/
}

int get_param_num(Command command){
        switch (command)
    {
        case EXIT:
            return 0;
        case SET:
            return 3;
        case VALIDATE:
            return 0;
        case SOLVE:
            return 1;
        case EDIT:
            return -1; /*specil case to indicate the optional param*/
        case MARK_ERRORS:
            return 1;
        case GUESS:
            return 1;
        case GENERATE:
            return 2;
        case UNDO:
            return 0;
        case REDO:
            return 0;
        case SAVE:
            return 1;
        case HINT:
            return 2;
        case GUESS_HINT:
            return 2;
        case NUM_SOLUTIONS:
            return 0;
        case PRINT:
            return 0;
        case AUTOFILL:
            return 0;
        case RESET:
            return 0;
        default:
            return -2; /*shouldn't be reached - this func should be only activated on valid commands*/

    }

    return -2; /*shouldn't be reached - this func should be only activated on valid commands*/
}

BOOL is_command_avilable(Command command, Mode mode){
    
    if (mode == INIT){
        if (command == SOLVE || command == EDIT || command == EXIT){
            return TRUE;
        }
        return FALSE;
    }

    if (mode == SOLVE_STATE){
        if (command == GENERATE){
            return FALSE;
        }
        return TRUE;
    }

    /*else, we're in EDIT mode..*/

    switch (command)
    {
        case MARK_ERRORS:
            return FALSE;
        case GUESS:
            return FALSE;
        case HINT:
            return FALSE;
        case GUESS_HINT:
            return FALSE;
        case AUTOFILL:
            return FALSE;
        default:
            return TRUE;
    }

    return FALSE; /*this shouldnt be reached!*/

}


Command str_to_command(char *command_name){

if (!strcmp("solve" , command_name)){
    return SOLVE;
}
if (!strcmp("edit" , command_name)){
    return EDIT;
}
if (!strcmp("mark_errors" , command_name)){
    return MARK_ERRORS;
}
if (!strcmp("print_board" , command_name)){
    return PRINT;
}
if (!strcmp("set" , command_name)){
    return SET;
}
if (!strcmp("validate" , command_name)){
    return VALIDATE;
}
if (!strcmp("guess" , command_name)){
    return GUESS;
}
if (!strcmp("generate" , command_name)){
    return GENERATE;
}
if (!strcmp("undo" , command_name)){
    return UNDO;
}
if (!strcmp("redo" , command_name)){
    return REDO;
}
if (!strcmp("save" , command_name)){
    return SAVE;
}
if (!strcmp("hint" , command_name)){
    return HINT;
}
if (!strcmp("guess_hint" , command_name)){
    return GUESS_HINT;
}
if (!strcmp("num_solutions" , command_name)){
    return NUM_SOLUTIONS;
}
if (!strcmp("autofill" , command_name)){
    return AUTOFILL;
}
if (!strcmp("reset" , command_name)){
    return RESET;
}
if (!strcmp("exit" , command_name)){
    return EXIT;
}

return UNKNOWN;  
}

char* command_to_str(Command command){
    switch (command)
    {
        case EXIT:
            return "exit";
        case SET:
            return "set";
        case VALIDATE:
            return "validate";
        case SOLVE:
            return "solve";
        case EDIT:
            return "edit";
        case MARK_ERRORS:
            return "mark_errors";
        case GUESS:
            return "guess";
        case GENERATE:
            return "generate";
        case UNDO:
            return "undo";
        case REDO:
            return "redo";
        case SAVE:
            return "save";
        case HINT:
            return "hint";
        case GUESS_HINT:
            return "guess_hint";
        case NUM_SOLUTIONS:
            return "num_solution";
        case PRINT:
            return "print_board";
        case AUTOFILL:
            return "autofill";
        case RESET:
            return "reset";
        default:
            return "FALSE"; /*shouldn't be reached - this func should be only activated on valid commands*/
    }

    return "FALSE"; /*shouldn't be reached - this func should be only activated on valid commands*/

}

char* correct_syntax(Command command){
    switch (command)
    {
        case EXIT:
            return "'exit'\n";
        case SET:
            return "'set X Y Z' where X, Y are ints indicating the location of the cell in the board,\nand Z is the value to be inserted in that cell\n";
        case VALIDATE:
            return "'validate'\n";
        case SOLVE:
            return "'solve X' where X is a valid path to a sudoku file\n";
        case EDIT:
            return "'edit' or 'edit X' where X is a valid path to a sudoku file\n";
        case MARK_ERRORS:
            return "'mark_errors X' where X is 1 or 0\n";
        case GUESS:
            return "'guess X' where X is a number greater than 0 and smaller then 1 (in a decimal format)\n";
        case GENERATE:
            return "'generate X Y' where X and Y are ints\n";
        case UNDO:
            return "'undo'\n";
        case REDO:
            return "'redo'\n";
        case SAVE:
            return "'save X' where X is a valid path to save the file in\n";
        case HINT:
            return "'hint X Y' where X, Y are ints indicating the location of the cell\n";
        case GUESS_HINT:
            return "'guess_hint X Y' where X, Y are ints indicating the location of the cell\n";
        case NUM_SOLUTIONS:
            return "'num_solution'\n";
        case PRINT:
            return "'print_board'\n";
        case AUTOFILL:
            return "'autofill'\n";
        case RESET:
            return "'reset'\n";
        default:
            return "FALSE\n"; /*shouldn't be reached - this func should be only activated on valid commands*/
    }

    return "FALSE\n"; /*shouldn't be reached - this func should be only activated on valid commands*/

}