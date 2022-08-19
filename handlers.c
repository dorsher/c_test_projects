#include "globals.h"
#include "helpers.h"

#include <stdio.h>
#include <string.h>

#define MAX_GUIDE_LENGTH 100

/**
 * @brief parsing the string guide line command: write values to a temp file (later will be converted to base32 in the obj file) and get string guide line length in words
 *
 * @param name - command to parse (in the form of string guide line)
 * @param output_file - data temporary file - save value in DECIMAL form (used to sperate data from instructions, where at a later point they will be concat separately into one object file)
 * @return int - # of string elements
 */
int handle_string_guide(char *name, FILE *output_file)
{
    int len;
    int i;
    int rc;

    rc = check_string_in_double_quotes(name);
    if (rc != 0)
    {
        print_log_line(log_error);
        printf("Invalid string guide argument.\n");
        return -1;
    }

    len = strlen(name);
    if (output_file != NULL)
    {
        for (i = 1; i < len - 1; i++)
        {
            /* output_file here is the data temporary file - save value in DECIMAL form */
            fprintf(output_file, "%d\n", name[i]);
        }

        /* Add string null-terminator */
        fprintf(output_file, "%d\n", 0);
    }

    return len - 1; /* add 1 for null terminator, substract 2 for double quotes ("-1" is the final substruction outcome) */
}

/**
 * @brief parsing the data guide line command: write values to a temp file (later will be converted to base32 in the obj file) and get data guide line length in words
 *
 * @param name - command to parse (in the form of data guide line)
 * @param output_file  - data temporary file - save value in DECIMAL form (used to sperate data from instructions, where at a later point they will be concat separately into one object file)
 * @return int - # of data elements
 */
int handle_data_guide(char *name, FILE *output_file)
{
    int size = 0;
    cell_value_type val_split_decimal;
    char *split = NULL;
    split = strtok(name, ",");
    while (split != NULL)
    {
        split = trim_string_whitespace(split);
        if (is_string_numeric(split) != 0)
        {
            return -1;
        }

        val_split_decimal = atoi(split);

        if (output_file != NULL)
        {
            /* output_file here is the data temporary file - save value in DECIMAL form */
            fprintf(output_file, "%d\n", val_split_decimal);
        }

        split = strtok(NULL, ",");
        size++;
    }

    return size;
}


/**
 * @brief parsing the struct guide line command: write values to a temp file (later will be converted to base32 in the obj file) and get struct guide line length in words
 *
 * @param name - command to parse (in the form of struct guide line)
 * @param output_file - data temporary file - save value in DECIMAL form (used to sperate data from instructions, where at a later point they will be concat separately into one object file)
 * @return int - # of struct elements
 */
int handle_struct_guide(char *name, FILE *output_file)
{
    int rc;
    char *split = NULL;
    cell_value_type val_split_decimal;

    split = strtok(name, ",");
    split = trim_string_whitespace(split);

    if (is_string_numeric(split) != 0)
    {
        printf("Failed reading the first element (%s) in the struct guide (should be decimal).", split);
        return -1;
    }

    val_split_decimal = atoi(split);

    if (output_file != NULL)
    {
        /* output_file here is the data temporary file - save value in DECIMAL form */
        fprintf(output_file, "%d\n", val_split_decimal);
    }

    split = strtok(NULL, "\n");

    if (split == NULL)
    {
        print_log_line(log_error);
        printf("struct guide must have string after '.'\n");
        return -1;
    }

    split = trim_string_whitespace(split);

    rc = handle_string_guide(split, output_file);
    if (rc == -1)
    {
        print_log_line(log_error);
        printf("Failed reading the second element (%s) in the struct guide (should be a string).\n", split);
        return -1;
    }

    /* struct always has one word which is the int in the first (left) member */
    return rc + 1;
}

/**
 * Get the object guide type
 *
 * @param name - the guide string to parse
 * @return type of guiding instrucion (guiding_instructions enum)
 */
guiding_instructions get_guide_type(char *name)
{
    char *split = NULL;
    split = strtok(&name[1], " ");
    split = trim_string_whitespace(split);

    if (strcmp(split, "entry") == 0)
        return entry_guide;
    if (strcmp(split, "extern") == 0)
        return extern_guide;
    if (strcmp(split, "string") == 0)
        return string_guide;
    if (strcmp(split, "data") == 0)
        return data_guide;
    if (strcmp(split, "struct") == 0)
        return struct_guide;
    return invalid_guide;
}

/**
 * @brief handle guide line (guide can be: entry, extern, string, data and struct)
 *
 * @param guide - the guide line (as read from the source file)
 * @param output_file - object file (output_file == NULL means we're in the first pass, so symbol table isn't built yet)
 * @return int - guide line length in words
 */
int handle_guide_line(char *guide, FILE *output_file)
{
    Symbol *symbol = NULL;
    Symbol new_symbol;

    int rc;
    char *split = NULL;
    guiding_instructions guide_rc;

    guide_rc = get_guide_type(guide);

    split = strtok(NULL, "\n");

    if (split == NULL)
    {
        print_log_line(log_error);
        printf("Guide line without content is not allowed\n");
        return -1;
    }

    split = trim_string_whitespace(split);

    switch (guide_rc)
    {
    case entry_guide:
        /* output_file == NULL means we're in the first pass, so symbol table isn't built yet - skip this check */
        if (output_file == NULL)
            return 0;
        /* look for the entry in symbols linked list
        if found - set is_entry to true
        else - throw an error and move on
        */
        symbol = find_symbol(split);
        if (symbol != NULL)
        {
            symbol->is_entry = 1;
        }
        else
        {
            print_log_line(log_warning);
            printf("The entry (%s) cannot be found.\n", split);
        }
        /* return 0 "words" as nothing should be printed for entry guide*/
        return 0;
    case extern_guide:
        symbol = find_symbol(split);

        /* If symbol is found and is NOT an extern, it's an error */
        if (symbol != NULL && symbol->is_extern == 0)
        {
            print_log_line(log_warning);
            printf("The extern (%s) was found in symbols table.\n", split);
        }
        else if (symbol == NULL)
        {
            /* symbol not yet in table - add it */
            new_symbol.address = 0;
            new_symbol.type = extern_guide;
            new_symbol.is_extern = 1;
            strcpy(new_symbol.tag, split);
            add_symbol(&new_symbol);
        }

        /* return 0 "words" as nothing should be printed */
        return 0;
    case string_guide:
        rc = handle_string_guide(split, output_file);
        if (rc == -1)
        {
            print_log_line(log_error);
            printf("string guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    case data_guide:
        rc = handle_data_guide(split, output_file);
        if (rc == -1)
        {
            print_log_line(log_error);
            printf("data guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    case struct_guide:
        rc = handle_struct_guide(split, output_file);
        if (rc == -1)
        {
            print_log_line(log_error);
            printf("struct guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    default:
    print_log_line(log_error);
        printf("invalid guiding operand: %s", split);
        return -1;
    }

    return rc;
}
