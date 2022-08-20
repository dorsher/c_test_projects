/**
 * @file helpers.c
 * @author Doron Sher
 * @brief help validate string related syntax is correct
 * @version 0.1
 * @date 2022-08-16
 *
 */
#include "helpers.h"
#include "globals.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

/**
 * verify the string contains double quotes
 *
 * @param name  - pointer to a string to
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
int check_string_in_double_quotes(char *name)
{
    int len;
    len = strlen(name);

    if (len == 0)
    {
        print_log_line(log_error);
        printf("The string's length empty (0)\n");
        return -1;
    }

    if ((name[0] == '"') && name[(len - 1)] == '"')
    {
        return 0;
    }

    print_log_line(log_error);
    printf("The string has no double quotes\n");
    return -1;
}

/**
 * string is checked to verify it's numric or not
 *
 * @param str - a pointer to the string to check whether it is numeric
 * @return int - return code ( 0 - okay, otherwise - failure)
 */
int is_string_numeric(const char *str)
{
    int i = 0;
    /* check whether string is not empty */
    if (str[0] == '\0')
    {
        return 1;
    }

    if (str[0] == '+' || str[0] == '-')
    {
        i++;
    }
    for (; str[i] != '\0'; i++)
    {
        /* rc = 0 of isdigit func means the digit is not a number */
        if ((isdigit(str[i])) == 0)
            return 2;
    }
    return 0;
}

/**
 * every string is trimmed (start and end), and a pointer is returned to the "trimmed" string
 *
 * @param name  - pointer to a string to trim
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
char *trim_string_whitespace(char *input)
{
    char *end_input;

    /* get rid of white spaces in start of string */
    while (isspace((unsigned char)*input))
    {
        input++;
    }

    /* if no white spaces, return the string */
    if (*input == '\0')
    {
        return input;
    }

    /* get rid of white spaces in end of string */
    end_input = input + strlen(input) - 1;
    while (end_input > input && isspace((unsigned char)*end_input))
    {
        end_input--;
    }

    /* place '\0' in end of string AFTER remove trailing spaces*/
    end_input[1] = '\0';

    return input;
}

/**
 * @brief logger (incldue log level in prints)
 * @param level - log level (can be info, warning, error or critical)
 */
void print_log_line(log_level level)
{
    /* Remove the '\n' at the end of the line */
    size_t line_len = strlen(current_line);
	if(line_len > 0 && (current_line[line_len -1]) == '\n')
		current_line[line_len -1] = '\0';
    switch (level)
    {
    case log_info:
        printf("Info (line [%s]): ", current_line);
        break;
    case log_warning:
        printf("Warning (line [%s]): ", current_line);
        break;
    case log_error:
        printf("Error (line [%s]): ", current_line);
        break;
    case log_critical:
        printf("Critical (line [%s]): ", current_line);
        break;
    default:
        break;
    }
}
