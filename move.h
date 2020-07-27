#ifndef MOVE_H
#define MOVE_H
#include "commands.h"
#include "errors.h" 
#include "typedefs.h"

/*For SET - param1 = coord x
            param2 = coord y
            param3 = the value that was there before the set oper*/

/*for AUTOFILL - need to preserve a copy of the old board, or a list of coordinates to erase in undo*/
struct move_parameter
{
    BOOL exist;
    int value;
    ERROR param_state;
};

struct Move
{
    Command COMMAND;
    ERROR state;
    Move_parameter param1;
    Move_parameter param2;
    Move_parameter param3;
    double extraParam; 
    char *path; 
};

void clear_move(Move *move); /*when we reuse the same move, resets all params to a diffault value*/
#endif