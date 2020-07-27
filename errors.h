#ifndef ERRORS_H
#define ERRORS_H

/*an enum for errors, helping to control the flow in main */
typedef enum
{
    SUCCESS = 0,
    /*EXIT_GAME is returned when a user asks to exit the game*/
    EXIT_GAME,
    /*after solving the puzzle, the user may exit and restart only,
     and if tried to command something else, COMMAND_UNAVAILABLE is returned*/
    COMMAND_UNAVAILABLE,
    /*retured when user tries to change a fixed cell*/
    CANT_CHANGE_FIXED_CELL,
    /*retured when user tries to apply invalid number*/
    VALUE_IS_INVALID,
    /*return when board is unsolvable*/
    UNSOLVABLE_BOARD,
    /*return when board is solvable*/
    SOLVABLE_BOARD,
    /*return when board is solved*/
    PUZZLE_SOLVED,
    /*return when reaching an eof*/
    EOF_NUM_OF_FIXED_CELLS,
    SET_SUCCESS,
    INIT_SUCCESS,
    /*when trying to reach a command that is availble only in Solve mode*/
    COMMAND_ONLY_AVAILABLE_IN_SOLVE_MODE,
    MEMORY_ALLOCATION_ERROR,
    BOARD_ERRONEUOS,
    ERROR_GUROBI, /*ilpSolver*/
    ERROR_NO_SOLUTION,/*ilpSolver - when there's no sol for the board*/
    ILLIGEAL_PATH, /*FileHandler - when there's a problem with opening the file that it's path the user provided*/
    WRONG_FILE_FORMAT, /*FileHandler - when the provided file is not matching the format in the instructions*/
    TOO_MANY_PARAMS, /*Parser - if the user entered too many params*/
    TOO_FEW_PARAMS, /*Parser - if the user entered too few params*/
    UNRECOGNIZED_COMMAND, /*Parser - when the user typed a command that doesn't exist*/
    REACHED_EOF, /*Parser - encountered EOF in this line of input*/
    NO_COMMAND_TO_UNDO, /*Game - returned from undo func when user trying to 
                        undo when there is nothing to undo anymore*/
    NO_COMMAND_TO_REDO,
    NOT_ENOUGH_EMPTY_CELLS,
    NO_VALIDATION_FOR_ERRONEOUS_BOARD,
    CANT_HINT_FIXED_CELL,
    CANT_HINT_FULL_CELL,
    CANT_HINT_ERRONEOUS_BOARD,
    INVALID_BOARD_PARAMETERS,
    SET_CLASH_WITH_FIXED_CELL,
    SET_CANT_CHANGE_FIXED_CELL,
    NO_AVILABLE_VAL,
    PUZZLE_GENERATOR_ERROR,
    NO_GUESSES_FOR_ERRONEUS_BOARD,
    NO_SAVE_FOR_ERRONEUOS_BOARD,
    NO_SAVE_FOR_UNSOLVABLE_BOARD,
    EXTRAPARAM_FAULT,
    MARK_ERRORS_FAULT,
    UNKNOWN_COMMAND,
    DONT_PRITNT,
    NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED,
    PARAM_NOT_IN_RANGE,
    BLANK_LINE_INPUT,
    TOO_LONG_INPUT,
    CANT_GUESS_HINT_ERRONEOUS_BOARD,
    CANT_GUESS_HINT_FIXED_CELL,
    CANT_GUESS_HINT_FOR_NON_EMPTY_CELL,
    INDEVIDUAL_PARAM_ERROR,
    OUT_OF_RANGE,
    WRONG_FILE_FORMAT_MISSING_VALS,
    WRONG_FILE_FORMAT_RANGE,
    WRONG_FILE_FORMAT_ILLGEAL_CHAR,
    READ_FILE_ERROR,
    WRONG_FILE_FORMAT_MISSING_VALS_TOP,
    WRONG_FILE_FORMAT_TOO_MANY_VALS,
	PUZZLE_FINISHED_BUT_ERRONEOUS,
    NO_OPTIONS_FOR_CELL,
    ERROR_PUZZLE_GENERATOR,
    INPUT_ERROR,
    GAME_EXE_ERROR,
    CANT_GENERATE_ERRONEUOS_BOARD,
     NO_AUTOFILL_FOR_UNSOLVABLE_BOARD,
     NO_AUTOFILL_FOR_ERRONEUS_BOARD
} ERROR;

#endif
