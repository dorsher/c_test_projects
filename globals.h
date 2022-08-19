/**
 * @file globals.h
 * @author Doron Sher
 * @brief All the global structures, enums and values used throughout the program.
 * @version 0.1
 * @date 2022-08-14
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "list.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_BITS_IN_CELL_WITHOUT_ARE 8
#define MAX_ELEMENTS 30
#define MAX_ELEMENTS_FOR_MACRO 100
#define OPERAND_SIZE 16
#define MAX_ELEMENTS_IN_CELL 16
#define RIGHT_SHIFT_BY_5 5
#define AT_LEAST_ONE_ARG 1
#define SUFFIX_AM ".am"
#define SUFFIX_TMP ".tmp"
#define SUFFIX_EXTERN ".ext"
#define WORD_ENDMACRO "endmacro"
#define WORD_STARTMACRO "macro "
#define SUFFIX_OB ".ob"
#define SUFFIX_ENTRY ".ent"

/* enum of all possible symbol types */
typedef enum
{
    symbol_data,
    symbol_string,
    symbol_struct,
    symbol_extern,
    symbol_entry,
    symbol_operation
} symbol_type;


/* enum of all possible guiding instruction types */
typedef enum
{
    entry_guide = 0,
    extern_guide = 1,
    string_guide = 2,
    data_guide = 3,
    struct_guide = 4,
    operation_guide = 5,

    invalid_guide = 666
} guiding_instructions;


/* enum of all possible guiding encoding types */
typedef enum
{
    encoding_absolute = 0,
    encoding_extern = 1,
    encoding_relocatable = 2
} encoding_type;


/* enum of all possible operation codes */
typedef enum
{
    mov = 0,
    cmp = 1,
    add = 2,
    sub = 3,
    not = 4,
    clr = 5,
    lea = 6,
    inc = 7,
    dec = 8,
    jmp = 9,
    bne = 10,
    get = 11,
    prn = 12,
    jsr = 13,
    rts = 14,
    hlt = 15
} opcodes;


/* enum of all possible operand types */
typedef enum
{
    immediate_operand = 0,
    direct_operand = 1,
    struct_operand = 2,
    register_operand = 3,
    invalid_operand = 666

} operand_type;


/* struct symbol - address, symbol's tag, guiding type and indicate whether symbol is an entry or not  */
typedef struct
{
    int address;
    char tag[MAX_ELEMENTS];
    guiding_instructions type;
    int is_entry;
    int is_extern;
} Symbol;


/* struct macro - macro's tag name and its content */
typedef struct
{
    char tag[MAX_ELEMENTS];
    char macro_content[MAX_ELEMENTS_FOR_MACRO];
} Macro;


/* struct operation info - know per each operation how many operands it should handle */
typedef struct
{
    const char *name;
    opcodes opcode;
    /* operand_num - how many operands the command should handle*/
    int operand_num;
} operation_info;

/* symbols and macros linked lists*/
List symbols;
List macros;

/* Reference the current line to be printed in case of errors to let the user know which line has the error */
char current_line[80];

/* "shortcut" for cell_value_type = unsigned short type */
typedef unsigned int cell_value_type;

/* the cell struct contains the value var with 10 bits to represent the "word" of the machine code.*/
typedef struct cell
{
    cell_value_type value : 10;
} cell;


/* linked lists functions */
Symbol *add_symbol(const Symbol *symbol);
Symbol *add_empty_symbol();
Symbol *find_symbol(const char *name);
int add_macro(const Macro *macro);
size_t get_operations_count();
Macro *find_macro(const char *name);
const operation_info *get_operations();

#endif /* GLOBALS_H_ */
