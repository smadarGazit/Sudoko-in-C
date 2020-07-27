#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "bool.h"
#include "cell.h"
#include "board.h"
#include "errors.h"
#include <stdio.h>

ERROR export_board(char* path, Board *B);
ERROR import_board(char* path, Board *B, char *info);


#endif