/**
 * @file list.h
 * @author Doron Sher
 * @brief Handle linked lists in project, used for macro and symbols identification
 * @version 0.1
 * @date 2022-08-19
 *
 */
#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>

/* Node element in list */
typedef struct Node
{
    void *value;
    struct Node *next;
} Node;

/* List can be used for macro \ symbol  */
typedef struct
{
    Node *head;
    unsigned int length;
} List;

/**
 * init the linked list by setting head to NULL
 * @param list - the linked list to init
 */
void list_init(List *list);

/**
 * return linked list's length
 *
 * @param list - the linked list to return its length
 * @return unsigned int - length
 */
unsigned int list_length(const List *list);

/**
 * @brief  insert a new node to the end of list (if it's the first element, then it becomes the list's head)
 *
 * @param list - the linked list to insert
 * @param value - value to be copied into the node
 * @param value_size - value size (see more info in "new_node" function's docs)
 * @return Node* - return the created node
 */
Node *list_insert_to_end(List *list, const void *value, size_t value_size);

/**
 * @brief clear the linked list memory by clearing its nodes from tail to head
 * @param list - the linked list to clear from memory
 */
void list_clear_memory(List *list);

#endif /* LIST_H_ */