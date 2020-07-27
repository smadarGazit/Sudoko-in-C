#ifndef LIST_H
#define LIST_H

#include "list_node.h"

struct List
{
    List_node *head;
    List_node *tail;
    int length;
};

ERROR get_list(List **moves_list);


void add_node_to_tail(List *moves_list, List_node *new_node);

/*TODO - do it need to free nodes after point of insertion?*/
/*adds a new node after a current specfied node (what is next and beyond curr_node
 is not relevent anymore*/
 /*if there is always an empty node in the start, it holds also for adding to head*/
void add_node_after_cur(List* moves_list, List_node* cur_node, List_node* new_node);


void link_nodes(List_node *left,List_node *right);

/*destroy the list from position_to_free_from to the tail..
 * if given list->head will destroy the whole list*/
int destroy_list_from_cur(List_node *position_to_free_from);

void destroy_list(List *list);

void free_list(List *list);
    
#endif