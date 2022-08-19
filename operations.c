/**
 * @file operations.c
 * @author Doron Sher
 * @brief Get operation metadata info for each operation (16 opcodes)
 * @version 0.1
 * @date 2022-08-16
 */

#include "globals.h"

/* For each operation, store its name, opcode and number of args it gets */
const operation_info operations[] = {
    {"mov",
     mov,
     2},
    {"cmp",
     cmp,
     2},
    {"add",
     add,
     2},
    {"sub",
     sub,
     2},
    {"not",
     not,
     1},
    {"clr",
     clr,
     1},
    {"lea",
     lea,
     2},
    {"inc",
     inc,
     1},
    {"dec",
     dec,
     1},
    {"jmp",
     jmp,
     1},
    {"bne",
     bne,
     1},
    {"get",
     get,
     1},
    {"prn",
     prn,
     1},
    {"jsr",
     jsr,
     1},
    {"rts",
     rts,
     0},
    {"hlt",
     hlt,
     0},
};

/**
 * @brief get the operations metadata info
 * @return const operation_info* - array of struct type "operation_info" with each operation's metadata (name, code etc)
 */
const operation_info *get_operations()
{
    return operations;
}

/**
 * @brief get the operations count
 * @return size_t - # of operations
 */
size_t get_operations_count()
{
    return sizeof(operations) / sizeof(operation_info);
}