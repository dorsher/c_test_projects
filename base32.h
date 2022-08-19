/**
 * @file base32.h
 * @author Doron Sher
 * @brief header file, convert to base 32
 * @version 0.1
 * @date 2022-08-17
 */

#include "globals.h"
#include <stdio.h>

/**
 * Convert to base 32 as was requested in the "imaginary" assembly language
 *
 * @param value - decimal value
 * @param fp - file path to write to
 */
void write_as_base_32(cell_value_type value, FILE *fp);