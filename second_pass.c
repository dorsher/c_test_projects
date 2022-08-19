/**
 * @file second_pass.c
 * @author Doron Sher
 * @brief second pass of the source file after macro has deployed as well as the first pass, to convert source to base32 (machine code)
 * @version 0.1
 * @date 2022-08-15
 *
 */

#include "globals.h"
#include "passes.h"
#include "base32.h"
#include "validators.h"
#include "helpers.h"
#include "handlers.h"

#include <string.h>

/* Shifts for bitwise operations (setting word fields) */
#define LEFT_SHIFT_BY_TWO 2
#define LEFT_SHIFT_BY_FOUR 4
#define LEFT_SHIFT_BY_SIX 6

/* address ia a static var that is accessible in this function to increment by 1 the word count (both IC & DC)*/
static cell_value_type current_address = 100;

/* bitwise operator functions */


/**
 * @brief Set the field type object in word
 *
 * @param cell_array - array of words
 * @param src - source value in word
 * @param element - index of word in the cell array
 * @param shift - left shift by a given value
 */
void set_field_type(cell *cell_array, int src, int element, int shift)
{
    int mask;
    mask = src << shift;
    cell_array[element].value |= mask;
}


/**
 * @brief Set the opcode object in word
 *
 * @param cell_array - array of words
 * @param opcode - opcode (among the 16 possible options defined in the docs, such as mov, lea, prn etc)
 * @param element - index of word in the cell array
 * @param shift - left shift by a given value
 */
void set_opcode(cell *cell_array, int opcode, int element, int shift)
{
    cell_array[element].value = opcode << shift;
}


/**
 * @brief Set the encoding type object
 *
 * @param cell_array - array of words
 * @param element - index of word in the cell array
 * @param type - encoding type
 */
void set_encoding_type(cell *cell_array, int element, encoding_type type)
{
    int mask;
    /* mask is the encoding type, for example: relocate = 2 */
    mask = type;
    cell_array[element].value |= mask;
}


/**
 * @brief handle operand words for immediate, direct, struct and register
 *
 * @param type - type of operand
 * @param cell_array - array of words
 * @param index_element - index in cell_array
 * @param value - value to set in word
 * @param shift_only_for_register - register special use case only, left shift by 2 for dest and 6 for src
 * @param is_extern - flag checking is extern word
 * @return int - num of words
 */
int handle_operand_words(operand_type type, cell *cell_array, unsigned int index_element, int *value,
                         int shift_only_for_register, const char *extern_symbol, FILE *externs_file)
{
    cell c_operand_word = {0};
    cell c_operand_second_word = {0};

    if (extern_symbol != NULL)
    {
        fprintf(externs_file, "%s\t", extern_symbol);
        write_as_base_32(current_address + index_element, externs_file);
        fputs("\n", externs_file);
    }

    switch (type)
    {
    /* handle_operand_words for immediate_operand and direct_operand is the same*/
    case immediate_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        return 1;
    case direct_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        /* choose the correct encoding */
        set_encoding_type(cell_array, index_element, (extern_symbol != NULL) ? (encoding_extern) : (encoding_relocatable));
        return 1;
    case struct_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        /* only the first word is struct (for example: S.1 so only "s" is relevant */
        set_encoding_type(cell_array, index_element, (extern_symbol != NULL) ? (encoding_extern) : (encoding_relocatable));

        c_operand_second_word.value = value[1] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element + 1] = c_operand_second_word;
        return 2;
    case register_operand:
        set_field_type(cell_array, value[0], index_element, shift_only_for_register);
        return 1;
    case invalid_operand:
        return -1;
    }

    return -1;
}


/**
 * @brief handle the operand addressing method ("A.R.E") and edit the word with the correct values based on the src\dst type
 *
 * @param type - type of operand
 * @param operand - the operand itself
 * @param method - addressing method: can be 0, 1 or 2 (A.R.E)
 * @param value - operand's value (for example, for immediate it will be the number itself)
 * @param is_extern - is operand extern
 * @return int - return code (0: okay, -1: error identifying the operand)
 */
int handle_operand_addressing_method(operand_type type, char *operand, int *method, int *value, const char **is_extern, FILE *externs_file)
{
    char *split = NULL;
    Symbol *symbol = NULL;

    switch (type)
    {
    case immediate_operand:
        *method = 0;
        *value = atoi(&operand[1]);
        break;
    case struct_operand:
        *method = 2;
        split = strtok(operand, ".");
        symbol = find_symbol(split);
        if (symbol->type == extern_guide)
        {
            *is_extern = symbol->tag;
        }
        value[0] = symbol->address;
        split = strtok(NULL, ".");
        value[1] = atoi(split);
        break;
    case register_operand:
        *method = 3;
        *value = operand[1] - '0';
        break;
    case direct_operand:
        *method = 1;
        /* value is the address taken from the linked list build during the first bypass*/
        symbol = find_symbol(operand);
        *value = symbol->address;
        if (symbol->type == extern_guide)
        {
            *is_extern = symbol->tag;
        }
        break;
    default:
        return -1;
    }
    return 0;
}


/**
 * @brief the main function that's used in the second pass to get any operand (src or dest) and handle it, all in one function.
 *
 * @param opcode - operation code (from 0 to 15).
 * @param cell_array - array of words
 * @param operand_source - src operand
 * @param operand_dest - dst operand
 * @return int - return total number of words in cell array
 */
int do_operation(opcodes opcode, cell *cell_array, FILE *externs_file, char *operand_source, char *operand_dest)
{
    /* type_source and type_operation are set to invalid_operand if there is no soure/dest operand (e.g. rts, hlt) */

	operand_type type_source = invalid_operand, type_dest = invalid_operand;
    int value_num_source[2];
    int value_num_dest[2];
    int src = 0, dst = 0, element;
    int rc;
    int num_words;

    const char *src_extern = NULL, *dst_extern = NULL;

    /* handle src operand */
    if (operand_source != NULL)
    {
        /* Validate operand type and label */
        type_source = get_operand_type(operand_source, 1);
        if (type_source == invalid_operand)
        {
            print_log_line(log_error);
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_source, operand_source, &src, value_num_source, &src_extern, externs_file);
        if (rc != 0)
        {
            print_log_line(log_error);
            printf("Failed handling the source addressing method\n");
            return -1;
        }
    }

    /* handle dest operand */
    if (operand_dest != NULL)
    {
        /* Validate operand type and label */
        type_dest = get_operand_type(operand_dest, 1);
        if (type_dest == invalid_operand)
        {
            print_log_line(log_error);
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_dest, operand_dest, &dst, value_num_dest, &dst_extern, externs_file);
        if (rc != 0)
        {
            print_log_line(log_error);
            printf("Failed handling the dest addressing method\n");
            return -1;
        }
    }

    /* Check that src/dst operand types are valid for this operation */

    /* lea operation cannot have register or immediate src operand types */
    if (opcode == lea && (type_source == immediate_operand || type_source == register_operand))
    {
        print_log_line(log_error);
        printf("lea operation must have direct or struct source operand\n");
        return -1;
    }

    /* Only cmp, prn can have immediate as dst operand type */
    if (type_dest == immediate_operand && opcode != cmp && opcode != prn)
    {
        print_log_line(log_error);
        printf("Only 'cmp' and 'prn' operations can have immediate destination operand\n");
        return -1;
    }

    /* zero cell is based on the instructions */
    element = 0;
    set_opcode(cell_array, opcode, element, LEFT_SHIFT_BY_SIX);
    set_field_type(cell_array, src, element, LEFT_SHIFT_BY_FOUR);
    set_field_type(cell_array, dst, element, LEFT_SHIFT_BY_TWO);
    /* increment element by 1 since the instruciton's word was written  */
    element = 1;

    /* handle special cases  */

    /* handle regsiter and register operands */
    if ((type_source == register_operand) && (type_dest == register_operand))
    {
        set_field_type(cell_array, value_num_source[0], element, LEFT_SHIFT_BY_SIX);
        set_field_type(cell_array, value_num_dest[0], element, LEFT_SHIFT_BY_TWO);
        return 2;
    }

    if (operand_source != NULL)
    { /* handle source operand - the handle_operand_words funciton contains another switch case per each case*/
        num_words = handle_operand_words(type_source, cell_array, element, value_num_source, LEFT_SHIFT_BY_SIX,
                                         src_extern, externs_file);

        if (num_words == -1)
        {
            print_log_line(log_error);
            printf("Failed getting source operand words.\n");
            return -1;
        }

        element += num_words;
    }

    /* handle dest operand  */
    if (operand_dest != NULL)
    {
        num_words = handle_operand_words(type_dest, cell_array, element, value_num_dest, LEFT_SHIFT_BY_TWO,
                                         dst_extern, externs_file);

        if (num_words == -1)
        {
            print_log_line(log_error);
            printf("Failed getting dest operand words.\n");
            return -1;
        }

        element += num_words;
    }

    /* return total number of words in cell array*/
    return element;
}


/**
 * @brief write to the object file as base32
 *
 * @param value - decimal value
 * @param obj_file - path to object file to write to
 */
void write_element_with_address(cell_value_type value, FILE *obj_file)
{
    write_as_base_32(current_address, obj_file);
    fputs("\t", obj_file);
    write_as_base_32(value, obj_file);
    fputs("\n", obj_file);
    current_address++;
}


/**
 * @brief write to the object file as base32 by iterating every word in the cell array of words
 * it calls the write_element_with_address function
 *
 * @param cell_array - cell array of words
 * @param num_of_elements - num of elements words in the cell array
 * @param obj_file - path to object file to write to
 */
void write_elements_in_cell(cell *cell_array, int num_of_elements, FILE *obj_file)
{
    int i;
    for (i = 0; i < num_of_elements; i++)
    {
        write_element_with_address(cell_array[i].value, obj_file);
    }
}


/**
 * @brief reads the source file after macros have deployed and following the first pass has finished, to convert the source to base32.
 *
 * @param obj_file - the .ob file to write the converted base32 commands to
 * @param file_prefix - the prefix of the source file (used to concat with the .ob format, or to read from the .am file)
 * @return int - if return code != 0 - it means the 2nd pass failed
 */
int second_pass(FILE *obj_file, const char *file_prefix)
{
    int i;
    int len_line;
    char *split = NULL;
    char line[80];

    FILE *tmp_data_file = NULL;
    FILE *externs_file = NULL;
    FILE *fp = NULL;

    /* build file names for temp, am and extern files
    note, the tmp file is used to concat the data and instructions base32 output in the correct order. When program ends - it's deleted. */

    const operation_info *operations = get_operations();

    char input_am_file_name[1024];
    char input_tmp_data_file_name[1024];
    char input_extern_file_name[1024];

    int num_of_elements;
    char first_operand[OPERAND_SIZE];
    char second_operand[OPERAND_SIZE];
    cell cell_array[MAX_ELEMENTS_IN_CELL];
    int rc;
    int is_found = 0;
    /* err is set to 0, and set to -1 on error, and we don't write the obj file */
    int err = 0;
    long externs_file_size;

    sprintf(input_am_file_name, "%s%s", file_prefix, SUFFIX_AM);
    sprintf(input_tmp_data_file_name, "%s%s", file_prefix, SUFFIX_TMP);
    sprintf(input_extern_file_name, "%s%s", file_prefix, SUFFIX_EXTERN);

    tmp_data_file = fopen(input_tmp_data_file_name, "w");
    externs_file = fopen(input_extern_file_name, "w");
    fp = fopen(input_am_file_name, "r");

    /* read each line and parse it */
    while (fgets(line, sizeof(line), fp))
    {
        strcpy(current_line, line);
        /* make sure the line is trimmed before start processing it */
        split = trim_string_whitespace(line);

        /* Ignore empty lines */
        if (split[0] == '\0')
        {
            continue;
        }

        /* ignore lines that starts with ';' */
        if (line[0] == ';')
        {
            print_log_line(log_info);
            printf("Line (%s) starts with ';', ignoring.\n", line);
            continue;
        }

        /* check for lables by looking for the ":" delimiter */
        len_line = strlen(line);
        split = strtok(line, ":");
        /* if strlen(split) == len_line then it means there is no ":" in the cmd*/
        if (strlen(split) != len_line)
        {
            /* there is ":" in line*/
            split = strtok(NULL, ":");
            if (split == NULL)
            {
                print_log_line(log_error);
                printf("Empty label line is not allowed\n");
                continue;
            }
            /* line is trimmed and now split is the pointer to the cmd*/
            split = trim_string_whitespace(split);
        }

        /* check for starting with "." */
        if (split[0] == '.')
        {
            /* verify type of guiding line (can be extern, entry, data, string or struct) */
            if (handle_guide_line(split, tmp_data_file) == -1)
            {
                print_log_line(log_error);
                printf("invalid guide line %s\n", split);
            }

            continue;
        }

        /* The memset() function clears the array (sets everything to 0) to avoid garbage data */
        memset(cell_array, 0, sizeof(cell_array));
        /* split is the cmd delimitered by " " */
        split = strtok(split, " ");

        split = trim_string_whitespace(split);
        for (i = 0; i < get_operations_count(); i++)
        {
            num_of_elements = -1;
            if (strcmp(split, operations[i].name) == 0)
            {
                is_found = 1;
                /* operand_num - how many operands should be handled */
                switch (operations[i].operand_num)
                {
                case 0:
                    num_of_elements = do_operation(operations[i].opcode, cell_array, externs_file, NULL, NULL);
                    break;
                case 1:
                    /* verify there are two words in command */
                    rc = validate_two_words_in_cmd(split, first_operand);
                    if (rc != 0)
                    {
                        print_log_line(log_error);
                        printf("operator %s is not valid.\n", operations[i].name);
                        break;
                    }

                    num_of_elements = do_operation(operations[i].opcode, cell_array, externs_file, NULL, first_operand);
                    break;
                case 2:
                    rc = validate_three_words_in_cmd(split, first_operand, second_operand);
                    if (rc != 0)
                    {
                        print_log_line(log_error);
                        printf("operator %s is not valid.\n", operations[i].name);
                        break;
                    }
                    num_of_elements = do_operation(operations[i].opcode, cell_array, externs_file, first_operand, second_operand);
                    break;
                default:
                    print_log_line(log_error);
                    printf("Invalid operand num %d", operations[i].operand_num);
                    break;
                }

                break;
            }
        }

        /* num_of_elements will be negative if do_operation failed (and returned -1) */
        if (num_of_elements > 0)
        {
            write_elements_in_cell(cell_array, num_of_elements, obj_file);
        }
        else
        {
            err = -1;
        }

        /* operations that are not listed in the opcodes table - fail with error */
        if (!is_found)
        {
            print_log_line(log_error);
            printf("operator %s is not recognized\n", split);
            err = -1;
        }
    }

    fclose(fp);
    fclose(tmp_data_file);
    fclose(externs_file);

    /* err != 0 means there was an error somewhere in the pass */
    if (err != 0)
    {
        if (remove(input_extern_file_name) != 0)
        {
            print_log_line(log_warning);
            printf("failed to remove externs file");
        }
    }

    /* If externs file is empty, remove it */
    externs_file = fopen(input_extern_file_name, "r");
    if (NULL != externs_file)
    {
        fseek(fp, 0, SEEK_END);
        externs_file_size = ftell(fp);
        fclose(externs_file);

        if (externs_file_size == 0)
        {
            if (remove(input_extern_file_name) != 0)
            {
                printf("failed to remove empty externs file");
            }
        }
    }

    /* Now that the code is written, add the data (with addresses) from the temp file */
    tmp_data_file = fopen(input_tmp_data_file_name, "r");
    while (fgets(line, sizeof(line), tmp_data_file))
    {
        write_element_with_address(atoi(line), obj_file);
    }

    fclose(tmp_data_file);

    if (remove(input_tmp_data_file_name) != 0)
    {
        print_log_line(log_warning);
        printf("failed to remove temporary data file\n");
    }

    return err;
}
