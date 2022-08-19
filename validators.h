/**
 * @file validators.h
 * @author Doron Sher
 * @brief Validate functions for the source file passes
 * @version 0.1
 * @date 2022-08-17
 *
 */
#ifndef _VALIDATORS_H_
#define _VALIDATORS_H_

#include "globals.h"


/**
 * Get the operand type object that can be one of: immediate, direct, struct, register or invalid.
 *
 * @param operand - operand to get its type
 * @param validate_label - 1 to validate the label for struct and direct. 0 - otherwise.
 * @return operand type
 */
operand_type get_operand_type(const char *operand, int validate_label);

/**
 * Validate the operand is consisted from two words
 *
 * @param operand  - the operand to validate whether it's consisted from two words
 * @return int - return code error
 */
int validate_two_words_in_cmd(char *split, char *first_operand);


/**
 * Validate the operand is consisted from three words
 *
 * @param operand  - the operand to validate whether it's consisted from three words
 * @return int - return code error
 */
int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand);

#endif /* _VALIDATORS_H_ */
