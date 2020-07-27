#include <stdio.h>
#include <stdlib.h>
#include "move.h"
#include "error.h"
#include "commands.h"

void clear_move(Move *move){
    move->COMMAND = UNKNOWN;
    move->param1.exist = FALSE;
    move->param1.param_state = SUCCESS;
    move->param2.exist = FALSE;
    move->param2.param_state = SUCCESS;
    move->param3.exist = FALSE;
    move->param3.param_state = SUCCESS;
    move->extraParam = -2.0;
    move->path = "";
    move->state = SUCCESS;
}
