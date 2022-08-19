/**
 * @file handlers.h
 * @author Doron Sher
 * @brief handlers are called to handle guide lines is source file
 * @version 0.1
 * @date 2022-08-19
 *
 *
 */
#ifndef _HANDLERS_H_
#define _HANDLERS_H_

#include "globals.h"
#include <stdio.h>

/**
 * Get the object guide type
 *
 * @param name - the guide string to parse
 * @return type of guiding instrucion (guiding_instructions enum)
 */
guiding_instructions get_guide_type(char *name);


/**
 * @brief handle guide line (guide can be: entry, extern, string, data and struct)
 *
 * @param guide - the guide line (as read from the source file)
 * @param output_file - object file (output_file == NULL means we're in the first pass, so symbol table isn't built yet)
 * @return int - guide line length in words
 */
int handle_guide_line(char *guide, FILE *output_file);

#endif /* _HANDLERS_H_ */