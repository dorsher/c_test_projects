/**
 * @file base32.c
 * @author Doron Sher
 * @brief convert from decimal to base32
 * @version 0.1
 * @date 2022-08-17
 */
#include "base32.h"

const char base32_array[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";

/**
 * Convert to base 32 as was requested in the "imaginary" assembly language
 *
 * @param value - decimal value
 * @param fp - file path to write to
 */
void write_as_base_32(cell_value_type value, FILE *fp)
{
    int mask_right = 31;
    int left_index_to_print, right_index_to_print;

    /* left 5 bits of cell */
    left_index_to_print = (value >> RIGHT_SHIFT_BY_5) & mask_right;

    /* right 5 bits of cell
    mask right = 0000011111
    with & of:   cell_array->value
    only the right 5 bits are handled
    */
    right_index_to_print = mask_right & value;
    fprintf(fp, "%c%c", base32_array[left_index_to_print], base32_array[right_index_to_print]);
}
