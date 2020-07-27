#ifndef COMMANDS_H
    #define COMMANDS_H

    #include "modes.h"
    #include "bool.h"
 
    #define BOOL int
    #define TRUE 1  
    #define FALSE 0 


/*enum for the possible commands, to make command_exe in 'game' more elegant
    * N_commands helps in intializing an array of the siutable size corresponding to the commands
    * */
    typedef enum
    {                   
        UNKNOWN = -1,
        EXIT,
        SET,
        VALIDATE,
        HINT,
        SOLVE,
        EDIT,
        MARK_ERRORS,
        GUESS,
        GENERATE,
        UNDO,
        REDO,
        SAVE,
        GUESS_HINT,
        NUM_SOLUTIONS,
        PRINT,
        AUTOFILL,
        RESET,
        N_COMMANDS
    } Command;



BOOL is_printable(Command command);
int get_param_num(Command command);
BOOL is_command_avilable(Command command, Mode mode);
Command str_to_command(char *command_name);
char* command_to_str(Command command);
char* correct_syntax(Command command);

#endif
