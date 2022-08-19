/**
 * @file list.c
 * @author Doron Sher
 * @brief Handle linked lists in project, used for macro and symbols identification
 * @version 0.1
 * @date 2022-08-19
 *
 */

#include <stdlib.h>
#include <memory.h>

#include "list.h"

/**
 * @brief Adding a new node to the linked list
 *
 * @param value - value to be copied into the node
 * @param value_size - value size (memcmp will be used to copy the passed data, and for that the value size is needed)
 * @return Node* - new node that was just added
 */
Node *new_node(const void *value, size_t value_size)
{
    Node *new_node = NULL;
    void *data = NULL;

    new_node = malloc(sizeof(Node));
    if (new_node == NULL)
        return NULL;

    data = malloc(value_size);
    if (data == NULL)
    {
        free(new_node);
        return NULL;
    }

    new_node->next = NULL;
    memcpy(data, value, value_size);
    new_node->value = data;

    return new_node;
}

/**
 * init the linked list by setting head to NULL
 * @param list - the linked list to init
 */
void list_init(List *list)
{
    list->head = NULL;
}

/**
 * return linked list's length
 *
 * @param list - the linked list to return its length
 * @return unsigned int - length
 */
unsigned int list_length(const List *list)
{
    return list->length;
}


/**
 * @brief  insert a new node to the end of list (if it's the first element, then it becomes the list's head)
 *
 * @param list - the linked list to insert
 * @param value - value to be copied into the node
 * @param value_size - value size (see more info in "new_node" function's docs)
 * @return Node* - return the created node
 */
Node *list_insert_to_end(List *list, const void *value, size_t value_size)
{
    Node *tmp_element = NULL;

    if (list->head == NULL)
    {
        list->head = new_node(value, value_size);
        if (list->head == NULL)
            return NULL;
        tmp_element = list->head;
    }
    else
    {
        tmp_element = list->head;
        while (tmp_element->next != NULL)
            tmp_element = tmp_element->next;
        tmp_element->next = new_node(value, value_size);
        if (tmp_element->next == NULL)
            return NULL;
        /* Return the newly-created (next) node */
        tmp_element = tmp_element->next;
    }

    list->length++;
    return tmp_element;
}


/**
 * @brief clear the linked list memory by clearing its nodes from tail to head
 * @param list - the linked list to clear from memory
 */
void list_clear_memory(List *list)
{
    Node *current = list->head;
    Node *next = NULL;

    while (current != NULL)
    {
        next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}
