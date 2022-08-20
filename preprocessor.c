/**
 * @file preprocessor.c
 * @author Doron Sher
 * @brief Scan the source file for macros and deploy them in a dedicated ".am" (after macro) file.
 * The first and second pass phases will parse the ".am" output file.
 * @version 0.1
 * @date 2022-08-19
 *
 */
#include "globals.h"
#include "passes.h"
#include "helpers.h"

#include <string.h>

/**
 * @brief Scan the source file for macros and deploy them in a dedicated ".am" (after macro) file
 *
 * @param input_file - source assembly input file
 * @param file_prefix - the source file prefix (will be used to concat with the ".am" suffix)
 */
void pre_processor(FILE *input_file, const char *file_prefix)
{
    FILE *fp_write_to;
    /* requirement in project: line's len is up to 80 chars */
    char line[MAX_CHARS_IN_LINE];
    char *split = NULL;
    int is_found_macro = 0;

    char output_file_name[MAX_CHARS_FILE_NAME];

    Macro current;
    Macro *other;

    /* concat outout file nsame string with file's prefix and ".am" */
    sprintf(output_file_name, "%s%s", file_prefix, SUFFIX_AM);

    fp_write_to = fopen(output_file_name, "w");

    /* read line by line and write to the "am" file (after macro) */
    while (fgets(line, sizeof(line), input_file))
    {
        split = trim_string_whitespace(line);
        if (strcmp(split, WORD_ENDMACRO) == 0)
        {
            is_found_macro = 0;
            add_macro(&current);
            continue;
        }

        if (is_found_macro != 0)
        {
            strcat(current.macro_content, split);
            strcat(current.macro_content, "\n");
            continue;
        }

        if (strstr(split, WORD_STARTMACRO) == split)
        {
            split = strtok(line, " ");
            split = strtok(NULL, " ");

            /* make sure tag name is trimmed */
            split = trim_string_whitespace(split);

            /* Clear macro content buffer */
            current.macro_content[0] = '\0';

            /* set macro name */
            strcpy(current.tag, split);
            is_found_macro = 1;
            continue;
        }

        /* before writing to file check for macro */
        other = find_macro(split);
        if (other != NULL)
        {
            fprintf(fp_write_to, "%s", other->macro_content);
            continue;
        }
        fprintf(fp_write_to, "%s\n", line);
    }
    fclose(fp_write_to);
}
