/**
 * @file macro_symbol.c
 * @author Doron Sher
 * @brief Hanlde macro and symbol tags in linked lists
 * @version 0.1
 * @date 2022-08-19
 *
 */
#include "globals.h"
#include <string.h>

/**
 * @brief add a new symbol to the symbol linked list
 *
 * @param symbol - the symbol node to be added
 * @return Symbol* - the added node
 */
Symbol *add_symbol(const Symbol *symbol)
{
    Node *node = list_insert_to_end(&symbols, symbol, sizeof(Symbol));
    if (node == NULL)
        return NULL;

    return (Symbol *)node->value;
}

/**
 * @brief create a new node in the symbols linked list
 * @return Symbol* - the created node
 */
Symbol *add_empty_symbol()
{
    Symbol symbol = {0};
    return add_symbol(&symbol);
}

/**
 * @brief Add Macro node to the macro's linked list
 *
 * @param macro - Macro to be added
 * @return int - 0: success, -1: failure
 */
int add_macro(const Macro *macro)
{
    if (list_insert_to_end(&macros, macro, sizeof(Macro)) == NULL)
        return -1;

    return 0;
}

/**
 * @brief Find the symbol tag in question in the symbol linked list
 *
 * @param name - the symbol's tag name to look for
 * @return Symbol* - the Symbol that was found (if not, NULL is returned)
 */
Symbol *find_symbol(const char *name)
{
    Node *current = symbols.head;
    Symbol *symbol = NULL;

    while (current != NULL)
    {
        symbol = current->value;
        if (strcmp(symbol->tag, name) == 0)
            return symbol;
        current = current->next;
    }

    return NULL;
}

/**
 * @brief Find the macro tag in question in the macro linked list
 *
 * @param name - the macro's tag name to look for
 * @return Macro* - the Macro that was found (if not, NULL is returned)
 */
Macro *find_macro(const char *name)
{
    Node *current = macros.head;
    Macro *macro = NULL;

    while (current != NULL)
    {
        macro = current->value;
        if (strcmp(macro->tag, name) == 0)
            return macro;
        current = current->next;
    }

    return NULL;
}
