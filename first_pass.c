/**
 * @file first_pass.c
 * @author Doron Sher
 * @brief  The first pass uses the following:
 * Iterate the source file after macros have already deployed
 * Insert symbols into the symbol linked list
 * write ic and dc to the object file
 * @version 0.1
 * @date 2022-08-19
 *
 */

#include "globals.h"
#include "handlers.h"
#include "passes.h"
#include "helpers.h"
#include "validators.h"
#include "base32.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Get the guide line length in words
 *
 * @param guide - guide line
 * @return int - length
 */
int get_guide_line_length_in_words(char *guide)
{
    return handle_guide_line(guide, NULL);
}

/**
 * @brief Get the operand length in words
 *
 * @param operand - the operand type
 * @return int - operand length in words (can be 0, 1 or 2)
 */
int get_operand_length_in_words(operand_type operand)
{
    switch (operand)
    {
    case register_operand:
    case immediate_operand:
    case direct_operand:
        return 1;
    case struct_operand:
        return 2;
    default:
        return 0;
    }
}

/**
 * @brief Get the operation line length in words
 *
 * @param operands - operands in line
 * @return int - operands line length in words
 */
int get_operation_line_length_in_words(char *operands)
{
    char *split_src = NULL, *split_dst = NULL;
    int size = 1;
    operand_type src_type = invalid_operand, dst_type = invalid_operand;

    split_src = strtok(operands, ",");
    split_dst = strtok(NULL, "\n");

    if (split_src != NULL)
    {
        split_src = trim_string_whitespace(split_src);
        /* Don't validate labels - in the first pass we only want the length */
        src_type = get_operand_type(split_src, 0);
        if (src_type == invalid_operand)
            return -1;

        size += get_operand_length_in_words(src_type);
    }

    if (split_dst != NULL)
    {
        split_dst = trim_string_whitespace(split_dst);
        /* Don't validate labels - in the first pass we only want the length */
        dst_type = get_operand_type(split_dst, 0);
        if (dst_type == invalid_operand)
            return -1;

        size += get_operand_length_in_words(dst_type);
    }

    /* Special case of register src AND dst - two words only */
    if (src_type == register_operand && dst_type == register_operand)
    {
        return 2;
    }

    return size;
}

/**
 * @brief Get the line length in words
 *
 * @param line - the line to be parsed (in ".am" file)
 * @return int - operation line length in words
 */
int get_line_length_in_words(char *line)
{
    int i;
    char *split = NULL;
    int is_operation = 0;

    if (line[0] == '.')
    {
        return get_guide_line_length_in_words(line);
    }

    split = strtok(line, " ");
    split = trim_string_whitespace(split);

    for (i = 0; i < get_operations_count(); i++)
    {
        if (strcmp(split, get_operations()[i].name) == 0)
        {
            is_operation = 1;
            break;
        }
    }

    if (is_operation == 0)
    {
        return -1;
    }

    split = strtok(NULL, "\n");
    return get_operation_line_length_in_words(split);
}

/**
 * @brief Validate the symbol tag name is valid (for example: first letter of symbol name must be alphabetic)
 *
 * @param symbol_name_string  - the symbol string to be validated
 * @return int - return code (0: success, otherwise: failure)
 */
int validate_symbol_tag_name(const char *symbol_name_string)
{
    const operation_info *operations = get_operations();
    size_t operations_count = get_operations_count();
    size_t len = strlen(symbol_name_string);
    size_t i;

    /* Empty label is not allowed */
    if (len == 0)
        return -1;

    /* First letter of symbol name must be alphabetic */
    if (isalpha(symbol_name_string[0]) == 0)
        return -1;

    /* Rest of the label name must be alphanumeric */
    for (i = 1; i < len; i++)
    {
        if (isalnum(symbol_name_string[i]) == 0)
            return -1;
    }

    /* Register name ('r' + digit between 0 and 7, inclusive) is forbidden */
    if (len == 2 && symbol_name_string[0] == 'r' && (symbol_name_string[1] >= '0' && symbol_name_string[1] <= '7'))
    {
        return -1;
    }

    /* Operation names are forbidden */
    for (i = 0; i < operations_count; i++)
    {
        if (strcmp(operations[i].name, symbol_name_string) == 0)
        {
            return -1;
        }
    }

    return 0;
}


/**
 * @brief Insert symbols into the symbol linked list, and write ic and dc to the object file
 *
 * @param obj_file - the object file
 * @param file_prefix - the source file prefix
 * @return int - if return code != 0 - it means the 1st pass failed
 */
int first_pass(FILE *obj_file, const char *file_prefix)
{
    int ic = 100, dc = 0, line_length;
    FILE *fp;
    char *split = NULL;
    char line[80];
    size_t len_line;
    int had_label;
    int err = 0;

    Symbol *new_symbol = NULL;
    Symbol *current = NULL;
    Node *current_node = NULL;

    char input_am_file_name[1024];

    sprintf(input_am_file_name, "%s%s", file_prefix, SUFFIX_AM);
    fp = fopen(input_am_file_name, "r");
    /* check if file can be opened */
    if (fp == NULL)
    {
        print_log_line(log_error);
        printf("Could not open the input file. Exiting.\n");
        return -1;
    }

    while (fgets(line, sizeof(line), fp))
    {
        strcpy(current_line, line);
        had_label = 0;
        split = trim_string_whitespace(line);

        if (split[0] == ';' || split[0] == '\0')
        {
            continue;
        }

        len_line = strlen(split);
        split = strtok(split, ":");
        if (strlen(split) != len_line)
        {
            /* check if symbol is valid (only alphanumeric, no keywords) */
            if (validate_symbol_tag_name(split) != 0)
            {
                print_log_line(log_error);
                printf("Invalid symbol name %s detected - skipping\n", split);
                err = -1;
                continue;
            }
            /* create a new node in the linked list */
            new_symbol = add_empty_symbol();

            current = new_symbol;
            /* add the tag to the node */
            strcpy(current->tag, split);
            had_label = 1;

            split = strtok(NULL, "\n");
            if (split == NULL)
            {
                printf("Label line without content is not allowed\n");
                err = -1;
                continue;
            }

            split = trim_string_whitespace(split);
        }

        line_length = get_line_length_in_words(split);
        if (line_length == -1)
        {
            print_log_line(log_error);
            printf("Invalid line length in %s\n", line);
            err = -1;
            continue;
        }

        if (split[0] == '.')
        {
            /* guide line */
            if (had_label)
            {
                current->address = dc;
                current->type = get_guide_type(split);
            }
            dc += line_length;
        }
        else
        {
            /* operation line */
            if (had_label)
            {
                current->address = ic;
                current->type = operation_guide;
            }
            ic += line_length;
        }
    }
    fclose(fp);

    current_node = symbols.head;

    while (current_node != NULL)
    {
        current = current_node->value;
        /* Add IC to data lines to put them in the correct index in memory */
        if (current->type == struct_guide || current->type == string_guide ||
            current->type == data_guide)
        {
            current->address += ic;
        }

        current_node = current_node->next;
    }

    /* write ic and dc in head of file*/

    write_as_base_32(ic - 100, obj_file);
    fputs(" ", obj_file);
    write_as_base_32(dc, obj_file);
    fputs("\n", obj_file);

    return err;
}
