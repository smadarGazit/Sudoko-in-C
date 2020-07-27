#include "list_node.h"
#include "command_data.h"

#include <stdlib.h>

ERROR get_new_node(List_node **new_node, Command_data *command_data)
{
    List_node *node = malloc(sizeof(List_node));
    if (!node)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    node->next = NULL;
    node->prev = NULL;
    node->command_data = command_data;
    *new_node = node;
    return SUCCESS;
}

void free_node(List_node *node)
{
    free_command_data(node->command_data);
    free(node);
}