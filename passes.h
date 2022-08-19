/**
 * @file passes.h
 * @author Doron Sher
 * @brief header contains the 3 main passes functions the assembler calls to: pre_processor, first_pass and second_pass.
 * @version 0.1
 * @date 2022-08-16
 */

#ifndef _SECOND_PASS_H_
#define _SECOND_PASS_H_

#include <stdio.h>

/**
 * @brief Scan the source file for macros and deploy them in a dedicated ".am" (after macro) file
 *
 * @param input_file - source assembly input file
 * @param file_prefix - the source file prefix (will be used to concat with the ".am" suffix)
 */
void pre_processor(FILE *input_file, const char *file_prefix);

/**
 * @brief Insert symbols into the symbol linked list, and write ic and dc to the object file
 *
 * @param obj_file - the object file
 * @param file_prefix - the source file prefix
 * @return int - if return code != 0 - it means the 1st pass failed
 */
int first_pass(FILE *obj_file, const char *file_prefix);

/**
 * @brief reads the source file after macros have deployed and following the first pass has finished, to convert the source to base32.
 *
 * @param obj_file - the .ob file to write the converted base32 commands to
 * @param file_prefix - the prefix of the source file (used to concat with the .ob format, or to read from the .am file)
 * @return int - if return code != 0 - it means the 2nd pass failed
 */
int second_pass(FILE *obj_file, const char *file_prefix);

#endif /* _SECOND_PASS_H_ */