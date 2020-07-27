#ifndef LIST_NODE_H
#define LIST_NODE_H
#include "typedefs.h"
#include "errors.h"
#include "command_data.h"

struct Node 
{
    Command_data *command_data;
    struct Node *next;
    struct Node *prev;
};


ERROR get_new_node(List_node **new_node, Command_data *command_data);

void free_node(List_node *node);

#endif