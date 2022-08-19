/**
 * @file helpers.h
 * @author Doron Sher
 * @brief help validate string related syntax is correct
 * @version 0.1
 * @date 2022-08-16
 *
 */

/* log level enum */
typedef enum
{
    log_info,
    log_warning,
    log_error,
    log_critical
} log_level;

/**
 * verify the string contains double quotes
 *
 * @param name  - pointer to a string to
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
int check_string_in_double_quotes(char *name);

/**
 * string is checked to verify it's numric or not
 *
 * @param str - a pointer to the string to check whether its numeric
 * @return int - return code ( 0 - okay, otherwise - failure)
 */
int is_string_numeric(const char *str);

/**
 * every string is trimmed (start and end), and a pointer is returned to the "trimmed" string
 *
 * @param name  - pointer to a string to
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
char *trim_string_whitespace(char *input);

/**
 * @brief Print line number and content, before an error message
 *
 * @param level log level to print
 */
void print_log_line(log_level level);