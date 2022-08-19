/**
 * @file main.c
 * @author Doron Sher
 * @version 0.1
 * @date 2022-08-16
 *
 * Call the main function (assembler) in order to run the next passes:
 * 1. Pre processor - look for macro files and deploy them (if found).
 * 2. First pass - insert symbols into the symbol linked list, and write ic and dc to the object file
 * 3. Second pass - convert the source to base32 (machine code)
 *
 * The assembler will print the object, entry & extern (if exists) files in base32 as required by the imaginary assembly language in project.
 */

#include "globals.h"
#include "list.h"
#include "helpers.h"
#include "passes.h"
#include "base32.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief write entries found in the source file into a dedicated entry output file
 *
 * @param file_prefix - the source file prefix ( will be concat with the ".entry" suffix)
 */
void write_entry_file(const char *file_prefix)
{
    char output_ent_file_name[1024];
    FILE *ent_file = NULL;
    Node *current_node = NULL;
    Symbol *symbol = NULL;

    /* build entry file name */
    sprintf(output_ent_file_name, "%s%s", file_prefix, SUFFIX_ENTRY);

    current_node = symbols.head;
    while (current_node != NULL)
    {
        symbol = current_node->value;

        if (symbol->is_entry == 1)
        {
            /* The first time we see an entry symbol, open the file.
             * This way, if there are no entries, no entry file is created
             */
            if (ent_file == NULL)
            {
                ent_file = fopen(output_ent_file_name, "w");
            }

            fprintf(ent_file, "%s\t", symbol->tag);
            write_as_base_32(symbol->address, ent_file);
            fputs("\n", ent_file);
        }

        current_node = current_node->next;
    }

    if (ent_file != NULL)
        fclose(ent_file);
}

/**
 * Read the input file as an argument (can be more than one arg) and process each one.
 * Next passes are executed on each input file: pre processor, first & second pass.
 * In the end, the assembler will print the object, entry & extern (if exists) in base32 as required by the imaginary assembly language in project.
 *
 * @param argc - # of commands.
 * @param argv - source input files.
 * @return int - success (0) or failure (otherwise)
 */
int main(int argc, char **argv)
{
    char file_prefix[1024] = {0};
    char output_obj_file_name[1024];
    char output_ext_file_name[1024];
    char *split = NULL;
    FILE *fp = NULL;
    int err;
    int i;

    /* verify at least one input file was passsed as an argument */

    current_line[0] = '\0';
    if (argc <= AT_LEAST_ONE_ARG)
    {
        print_log_line(log_critical);
        printf("At least one arg (source file) is required. Exiting.\n");
        return -1;
    }

    /* run in a loop. For every arg there are 3 passes that are called: pre processor, first pass, and second pass */
    for (i = 1; i < argc; i++)
    {
        /* verify file exists in file system */
        FILE *input_file;
        if ((input_file = fopen(argv[i], "r")) == NULL)
        {
            print_log_line(log_error);
            printf("Cannot find file '%s' in file system. Skipping.\n", argv[i]);
            continue;
        }

        /* init the symbols and macros linked lists */
        list_init(&symbols);
        list_init(&macros);

        current_line[0] = '\0';
        printf("--- Start processing input file #%d: %s\n", i, argv[i]);
        split = strtok(argv[i], ".");
        strcpy(file_prefix, split);

        /* call the first phase - the pre processor to look for macros */
        pre_processor(input_file, file_prefix);

        fclose(input_file);

        /* build object output file name */
        sprintf(output_obj_file_name, "%s%s", file_prefix, SUFFIX_OB);

        fp = fopen(output_obj_file_name, "w");

        /* call the first and second phases:
            first pass - identify symbols
            second pass - convert commands to base32
         */
        err = first_pass(fp, file_prefix);
        /* rc != 0 means an error was encountered in the first pass*/
        if (err != 0)
        {
            print_log_line(log_error);
            printf("First pass failed. Cleaning up and skipping file\n");
            fclose(fp);
            remove(output_obj_file_name);

            /* Continue to process next file(s) in args */
            continue;
        }

        err = second_pass(fp, file_prefix);
        fclose(fp);

        if (err != 0)
        {
            print_log_line(log_error);
            printf("Second pass failed. Cleaning up and skipping file\n");
            remove(output_obj_file_name);
            /* Continue to process next file(s) in args */
            continue;
        }

        printf("--- Finished processing input file #%d: %s\n", i, argv[i]);

        /* Write entry file */
        write_entry_file(file_prefix);

        /* in case the extern file is empty in end of running, remove it from file system */
        sprintf(output_ext_file_name, "%s%s", file_prefix, SUFFIX_EXTERN);
        fp = fopen(output_ext_file_name, "r");
        if (fp != NULL)
        {
            int file_size_in_file_system;
            /* seek the file to the end and use ftell to get the file's size, if 0, it means it's empty */
            fseek(fp, 0, SEEK_END);
            file_size_in_file_system = ftell(fp);
            if (file_size_in_file_system == 0)
                remove(output_ext_file_name);
        }

        /* avoid mem leak by clearing the linked list and its nodes */
        list_clear_memory(&symbols);
        list_clear_memory(&macros);
    }

    return 0;
}
