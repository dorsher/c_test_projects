/**
 * @file validators.c
 * @author Doron Sher
 * @brief validate functions to be used for the first + second passes
 * @version 0.1
 * @date 2022-08-17
 */
#include "validators.h"
#include "helpers.h"

#include <stdio.h>
#include <string.h>

/**
 * Validate whether the operand is immediate or not
 *
 * @param operand  - the operand to validate whether it's immediate or not
 * @return int - return code error
 */
int validate_immediate_operand(const char *operand)
{
    int rc;
    /* get the first char of operand and verify it's '#' as required */
    if (operand[0] != '#')
    {
        /* operand does not start with '#' as required */
        return -1;
    }
    rc = is_string_numeric(&operand[1]);
    if (rc != 0)
    {
        /* operand does not contain a numeric number following the '#' char */
        return -1;
    }
    return 0;
}

/**
 * Validate whether the operand is register or not
 *
 * @param operand  - the operand to validate whether it's register or not
 * @return int - return code error
 */
int validate_register_operand(const char *operand)
{
    int register_num = operand[1] - '0';
    /* 0 false, 1 true */

    /* get the regiser operand and verify it's correct */
    if (strlen(operand) != 2)
    {
        /* A register with 1 numeric identifer (i.e. 'r1') between 0 to 7 is required */
        return -1;
    }
    if (operand[0] != 'r')
    {
        /* Register symbol does not exist */
        return -1;
    }
    if ((register_num > 7) || (register_num < 0))
    {
        /* Register number is not valid */
        return -1;
    }

    return 0;
}

/**
 * Validate whether the operand is direct or not by checking the symbol linked list
 *
 * @param operand - the operand to validate whether it's direct or not
 * @return int - return code error
 */
int validate_direct_operand(const char *operand)
{
    /* If operand was found in linked list, return 0; Otherwise, -1 */
    return (find_symbol(operand) == NULL) ? (-1) : (0);
}

/**
 * Validate whether the operand is a struct or not
 *
 * @param operand  - the operand to validate whether it's struct or not
 * @param validate_label - if 0, don't check if the left side of the '.' delimiter
 *                         is a valid label
 * @return int - return code error
 */
int validate_struct_operand(char *operand, int validate_label)
{

    int len_line;
    int struct_member_count_number = 0;

    char *split = NULL;
    len_line = strlen(operand);
    split = strtok(operand, ".");

    /* there is a "."" in line? */
    if (strlen(split) == len_line)
    {
        return -1;
    }

    if (validate_label != 0)
    {
        /* validate left to the . delimieter */
        if (find_symbol(operand) == NULL)
        {
            return -1;
        }
    }

    /* validate right to the . delimieter */
    split = strtok(NULL, ".");
    if (is_string_numeric(split) != 0)
    {
        return -1;
    }

    struct_member_count_number = atoi(split);
    if (struct_member_count_number != 1 && struct_member_count_number != 2)
    {
        print_log_line(log_error);
        printf("struct operand must have 1 or 2 after '.'\n");
        return -1;
    }

    return 0;
}

/**
 * Validate the operand is consisted from two words
 *
 * @param operand  - the operand to validate whether it's consisted from two words
 * @return int - return code error
 */
int validate_two_words_in_cmd(char *split, char *first_operand)
{
    int counter_splitted_words = 0;
    while (split != NULL)
    {
        counter_splitted_words += 1;
        split = strtok(NULL, " ");
        if ((split == NULL) && (counter_splitted_words < 2))
        {
            print_log_line(log_error);
            printf("Expected 2 words in command, got %d\n", counter_splitted_words);

            return -1;
        }
        else if (counter_splitted_words == 1)
        {
            split = trim_string_whitespace(split);
            strcpy(first_operand, split);
        }
        else if (counter_splitted_words > 2)
        {
            /* more than 2 words were provided */
            print_log_line(log_error);
            printf("Too many operands were provided\n");
            return -1;
        }
    }

    return 0;
}

/**
 * Validate the operand is consisted from three words
 *
 * @param operand  - the operand to validate whether it's consisted from three words
 * @return int - return code error
 */
int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand)
{
    int counter_splitted_words = 0;
    while (split != NULL)
    {
        split = strtok(NULL, ",");
        counter_splitted_words += 1;
        /* verify the first and second word (cmd + operand) exist as split should not be null */
        if ((split == NULL) && (counter_splitted_words < 3))
        {
            print_log_line(log_error);
            printf("Expected 3 words in command, got %d\n", counter_splitted_words);

            return -1;
        }
        else if (counter_splitted_words == 1)
        {
            split = trim_string_whitespace(split);
            strcpy(first_operand, split);
        }
        else if (counter_splitted_words == 2)
        {
            split = trim_string_whitespace(split);
            strcpy(second_operand, split);
        }
        else if (counter_splitted_words > 3)
        {
            /* more than 3 words were provided */
            print_log_line(log_error);
            printf("Too many operands were provided\n");
            return -1;
        }
    }

    return 0;
}

/**
 * Get the operand type object that can be one of: immediate, direct, struct, register or invalid.
 *
 * @param operand - operand to get its type
 * @param validate_label 1: to validate the label for struct and direct. 0: otherwise.
 * @return operand type
 */
operand_type get_operand_type(const char *operand, int validate_label)
{
    int rc;
    char tmp_operand[OPERAND_SIZE];

    rc = validate_register_operand(operand);
    if (rc == 0)
    {
        return register_operand;
    }
    rc = validate_immediate_operand(operand);
    if (rc == 0)
    {
        return immediate_operand;
    }

    strcpy(tmp_operand, operand);
    rc = validate_struct_operand(tmp_operand, validate_label);
    if (rc == 0)
    {
        return struct_operand;
    }

    /* Since direct operands are just labels, don't check if validate_label is false */
    if (validate_label != 0)
    {
        rc = validate_direct_operand(operand);
        if (rc == 0)
        {
            return direct_operand;
        }
    }
    else
    {
        return direct_operand;
    }

    return invalid_operand;
}
