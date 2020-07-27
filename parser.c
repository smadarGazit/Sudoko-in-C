#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "move.h"
#include "parser.h"
#include "errors.h"


/*recives the string we got from the user.
matchs the first word to a command and the following words to parameters*/
ERROR parse_user_input(char *user_input, Move *user_move){
    int param_num = NULL;
    char* token;

    clear_move(user_move);

    token = strtok(user_input, " \t\r\n");
    user_move->COMMAND = str_to_command(token);
    if (user_move->COMMAND == UNKNOWN){
        user_move->path = token;
        return UNRECOGNIZED_COMMAND; 
    }

    param_num = get_param_num(user_move->COMMAND);  

    if (param_num==0){
        return zero_param(user_move);
    }

    if (param_num==-1){ /*the order is edit and it's value is optional*/
        return optional_param(user_move);
    }
    
    if (param_num==1){ 
        return one_param(user_move);
    }

    return get_params(user_move); 
}



ERROR string_to_int(char *token, Move_parameter *move_parameter){
    int res, sign;
    char *ptr_cur_char;
    res = 0;
    sign=1;
    move_parameter->exist = TRUE;
    if (*token == '-'){
        token++;
        sign = -1;
        if (*token == '\0'){
            move_parameter->param_state = NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED;
            return INDEVIDUAL_PARAM_ERROR;
        }
    }
    for(ptr_cur_char = token; (*ptr_cur_char != '\0'); ++ptr_cur_char) {
        if(!isdigit(*ptr_cur_char)){
            move_parameter->param_state = NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED;
            return INDEVIDUAL_PARAM_ERROR;
        }
        res = res * 10 + *ptr_cur_char - '0';
    }

    move_parameter->value = sign * res;
    return SUCCESS;
}

double string_to_double(char *token, Move *move){
    double rez = 0, fact = 1;
    int point_seen=0;
    int d;

    if (*token == '-'){
        token++;
        fact = -1;
        if (*token == '\0'){
            move->state = NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED;
            return 0.0;
        }
    }

    for (point_seen = 0; *token!='\0'; token++){
        if (*token == '.'){
            point_seen = 1; 
            continue;
        }
        if (!isdigit(*token)){
            move->state = NON_NUMERIC_PARAM_WHEN_WHEN_NEEDED;
            return 0.0;
        }
        d = *token - '0';
        if (d >= 0 && d <= 9){
            if (point_seen){
                fact /= 10.0f;
            } 
            rez = rez * 10.0f + (double)d;
        }
    }
    return rez * fact;
   }
/* relevant for "edit" only */
/* checks if the number of params entered is 0 or 1 (if not, returns an error). If 1 param is found,
   enters it to the path field in the move recived as input */ 
ERROR optional_param(Move *move){ 
    char *param1;

    param1 = strtok(NULL, " \t\r\n");
    if (!param1){
        move->state = SUCCESS;
        return SUCCESS;
    }
    move->path = param1;
    if (strtok(NULL, " \t\r\n")){
        move->state = TOO_MANY_PARAMS;
        return TOO_MANY_PARAMS;
    }
    return SUCCESS;        
}

/* relevant for commands that don't require input at all */
/* checks if the number of params entered is 0 (if not, returns an error)*/
ERROR zero_param(Move *move){
    char *str;
    str = strtok(NULL, " \t\r\n");
    if (!str){
        move->state = SUCCESS;
        return SUCCESS;
    }
    move->state = TOO_MANY_PARAMS;
    return TOO_MANY_PARAMS; 
}

/* relevant for "solve", "mark_errors", "guess", "save" */
/* checks if the number of params entered is 1 (if not, returns an error). If 1 param is found,
   enters it to the move recived as input */ 
ERROR one_param(Move *move){
    char *str, *str2;

    str = strtok(NULL, " \t\r\n");
    if (!str){ 
        move->state = TOO_FEW_PARAMS; 
        return TOO_FEW_PARAMS;
    }

    str2 = strtok(NULL, " \t\r\n");
      
    if (str2){
        move->state = TOO_MANY_PARAMS;
        return TOO_MANY_PARAMS;
    }

    if (move->COMMAND == SOLVE || move->COMMAND == SAVE){
        move->path = str;
    }

    if (move->COMMAND == MARK_ERRORS){
        string_to_int(str,  &move->param3);
    }

    if (move->COMMAND == GUESS){
        move->extraParam = string_to_double(str, move); 
    }

    return SUCCESS;
}


ERROR get_params(Move *move){ 
    char *str, *str2, *str3, *str4;
    ERROR error;
    
    int param_count =get_param_num(move->COMMAND);
    
    str = strtok(NULL, " \t\r\n");
    str2 = strtok(NULL, " \t\r\n");
    str3 = strtok(NULL, " \t\r\n");
    str4 = strtok(NULL, " \t\r\n");

    if (!str || !str2 || (!str3 && param_count == 3)){
        move->state = TOO_FEW_PARAMS;
        return TOO_FEW_PARAMS;
    }

    if (str4 || (str3 && param_count == 2)){
        move->state = TOO_MANY_PARAMS;
        return TOO_MANY_PARAMS;
    }

    if (param_count >= 1)
    {
       error = string_to_int(str,  &move->param1);
       if (error!=SUCCESS){
           move->state = error;
       }
    }

    if (param_count >= 2)
    {
        error = string_to_int(str2,  &move->param2);
        if (error!=SUCCESS){
           move->state = error;
        }        
    }
    if (param_count >= 3)
    {
        error = string_to_int(str3,  &move->param3);
        if (error!=SUCCESS){
           move->state = error;
        } 
    }

    return SUCCESS;
}
