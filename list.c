#include "list.h"
#include "list_node.h"
#include "errors.h"

#include <stdlib.h>

ERROR get_list(List **moves_list)
{
    List *list = (List*)malloc(sizeof(List));
    if(!list)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    list->head = NULL;
    list->tail = NULL; 
    list->length = 0; 
    *moves_list = list;
    return SUCCESS;
}

void add_node_to_tail(List *moves_list, List_node *new_node)
{
    /*assuming empty_list iff tail anf head == NULL*/
    if(moves_list->length == 0)
    {
        moves_list->head = new_node;
        moves_list->tail = new_node;
    }
    else
    {
        link_nodes(moves_list->tail, new_node);
        new_node->next = NULL; /*I think it is not needed...*/
        moves_list->tail = new_node;
    }
    ++moves_list->length;
}

/*TODO - do it need to free nodes after point of insertion?*/
/*adds a new node after a current specfied node (what is next and beyond curr_node
 is not relevent anymore*/
 /*if there is always an empty node in the start, it holds also for adding to head*/
void add_node_after_cur(List* moves_list, List_node* cur_node, List_node* new_node)
{
    int deleted_nodes = 0;
    /*case 1: it is the first node in the list*/
    if(moves_list->length == 0)
    {
        add_node_to_tail(moves_list, new_node);
        return;
    }
    /*if asked to add to first,,,*/
    if(cur_node == NULL && moves_list->length > 0)
    {
        destroy_list(moves_list);
        moves_list->tail = new_node;
        moves_list->head = new_node;
        moves_list->length = 1;
        return;
    }

    if(cur_node != moves_list->tail)
    {
        deleted_nodes = destroy_list_from_cur(cur_node->next);
    }

    /*case 2: more than one node in the list*/
    link_nodes(cur_node, new_node);
    moves_list->tail = new_node;
    moves_list->length = moves_list->length - deleted_nodes + 1;
}


void link_nodes(List_node *left, List_node *right)
{
    left->next = right;
    right->prev = left;
}

/*destroy the list from position_to_free_from to the tail..
 * if given list->head will destroy the whole list
 * @ret the number of cells deleted*/
int destroy_list_from_cur(List_node *position_to_free_from)
{
    int deleted_cnt = 0;
    List_node *cur_node, *node_to_delete;

    if(position_to_free_from == NULL)
    {
       return 0; 
    }

    cur_node = position_to_free_from;
    while(cur_node != NULL)
    {
        node_to_delete = cur_node;
        cur_node = cur_node->next;
        free_node(node_to_delete);
        ++deleted_cnt;
    }
    return deleted_cnt;
}

void destroy_list(List *list)
{
    destroy_list_from_cur(list->head);
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
}

void free_list(List *list)
{
    destroy_list_from_cur(list->head);
    free(list);
}
    