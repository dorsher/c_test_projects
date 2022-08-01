/* shift + alt + F to indent your code
should have an array of structs (each object has the opcode, src, dst etc) later on you run in a loop... talk with the legend

first pass
validate like immediate 

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define NUMBER_OF_ITEMS_FOR_ONE_OPERAND 2
#define LEFT_SHIFT_SRC_TYPE_COUNT 4
#define LEFT_SHIFT_DST_TYPE_COUNT 2
#define LEFT_SHIFT_OPCODE_COUNT 6
#define LEFT_SHIFT_SRC_TYPE_COUNT_BY_DIRECT_REGISTER 2
#define LEFT_SHIFT_DST_TYPE_COUNT_BY_DIRECT_REGISTER 6
#define LEFT_SHIFT_BY_TWO 2
#define LEFT_SHIFT_BY_SIX 6
#define NUM_BITS_IN_CELL_WITHOUT_ARE 8

/* the cell struct contains the value var with 10 bits
to represent the "word" of the machine code instructions.
*/

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

typedef enum
{
    immediate_operand = 0,
    direct_operand = 1,
    struct_operand = 2,
    register_operand = 3,
    invalid_operand = 666

} operand_type;

typedef struct cell
{
    unsigned short value : 10;
} cell;

/* decalre function prototypes*/

/* two operand function prototypes*/
int do_operation(opcodes opcode, cell *cell_array, char *operand_source, char *operand_dest);

/* oprand validator function prototypes*/

int validate_register_operand(const char *operand);
int validate_immediate_operand(const char *operand);
int validate_two_words_in_cmd(char *split, char *first_operand);
int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand);
void print_elements_in_cell(cell *cell_array, int num_of_elements);

/* decalre bitwise operator functions */

void set_opcode(cell *cell_array, int opcode, int element, int shift);
void set_field_type(cell *cell_array, int src, int element, int shift);
void set_dst_type(cell *cell_array, int dst, int element, int shift);
/*
 set_opcode
 set_src_type
 set_dst_type
*/

operand_type get_operand_type(const char *operand)
{
    operand_type type = invalid_operand;
    int rc;
    rc = validate_register_operand(operand);
    if (rc == 0)
    {
        type = register_operand;
    }
    else
    {
        rc = validate_immediate_operand(operand);
        if (rc == 0)
        {
            type = immediate_operand;
        }
        else
        {
            // printf("Operand '%s' is not valid.\n", mov_operation);
            return 1;
        }
    }

    return type;
}

int handle_operand_addressing_method(operand_type type, const char *operand, int *method, int *value)
{
    switch (type)
    {
    case immediate_operand:
        *method = 0;
        *value = atoi(&operand[1]);
        break;
    case register_operand:
        *method = 3;
        *value = operand[1] - '0';
        break;
    default:
        return -1;
    }
    return 0;
}

int handle_operand_words(operand_type type, cell *cell_array, unsigned int index_element, int value, int shift_only_for_register)
{
    cell c_operand_word = {0};
    switch (type)
    {
    case immediate_operand:
        c_operand_word.value = value << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        return 1;
    case direct_operand:
        break;
    case struct_operand:
        break;
    case register_operand:
        set_field_type(cell_array, value, index_element, shift_only_for_register);
        return 1;
    case invalid_operand:
        return -1;
    }
}

typedef struct
{
    const char *name;
    opcodes opcode;
    /* operand_num - how many operands the command should handle*/
    int operand_num;
    /* TODO (TBD) - add operand_options (!@!!!!!!)*/
} operation_info;

operation_info operations[] = {
    {.name = "mov",
     .opcode = mov,
     .operand_num = 2},
    {.name = "cmp",
     .opcode = cmp,
     .operand_num = 2},
    {.name = "add",
     .opcode = add,
     .operand_num = 2},
    {.name = "sub",
     .opcode = sub,
     .operand_num = 2},
    {.name = "not",
     .opcode = not,
     .operand_num = 1},
    {.name = "clr",
     .opcode = clr,
     .operand_num = 1},
    {.name = "lea",
     .opcode = lea,
     .operand_num = 2},
    {.name = "inc",
     .opcode = inc,
     .operand_num = 1},
    {.name = "dec",
     .opcode = dec,
     .operand_num = 1},
    {.name = "jmp",
     .opcode = jmp,
     .operand_num = 1},
    {.name = "bne",
     .opcode = bne,
     .operand_num = 1},
    {.name = "get",
     .opcode = get,
     .operand_num = 1},
    {.name = "prn",
     .opcode = prn,
     .operand_num = 1},
    {.name = "jsr",
     .opcode = jsr,
     .operand_num = 1},
    {.name = "rts",
     .opcode = rts,
     .operand_num = 0},
    {.name = "hlt",
     .opcode = hlt,
     .operand_num = 0},
};

int main()
{
    int i;
    /* temp solution - process the instruction */
    char inst[] = "hlt";
    char res_buff[100] = {0};

    /* the first operand splitted by " " */
    char *split = strtok(inst, " ");

    /* operations with one operand */
    char rts_operation[] = "rts";
    char hlt_operation[] = "hlt";

    /* operations with one operands */
    char not_operation[] = "not";
    char clr_operation[] = "clr";
    char inc_operation[] = "inc";
    char dec_operation[] = "dec";
    // char jmp_operation[] = "jmp";
    // char bne_operation[] = "bne";
    char get_operation[] = "get";
    char prn_operation[] = "prn";
    // char jsr_operation[] = "jsr";

    /* operations with two operands */
    char mov_operation[] = "mov";
    char cmp_operation[] = "cmp";
    char add_operation[] = "add";
    char sub_operation[] = "sub";
    char lea_operation[] = "lea";

    /* temp solution - process call and operand */
    char operand_param[10];
    cell res_cell;

    /* temp solution - global vars to handle register operands */
    int num_of_elements;
    char first_operand[16];
    char second_operand[16];
    cell cell_array[5] = {0};
    int rc;
    int is_found = 0;

    for (i = 0; i < sizeof(operations) / sizeof(operations[0]); i++)
    {
        if (strcmp(split, operations[i].name) == 0)
        {
            is_found = 1;
            printf("operator %s is valid\n", split);
            switch (operations[i].operand_num)
            {
            case 0:
                num_of_elements = do_operation(operations[i].opcode, cell_array, NULL, NULL);
                break;
            case 1:
                /* verify there are two words in command */
                rc = validate_two_words_in_cmd(split, first_operand);
                if (rc != 0)
                {
                    printf("- operator %s is not valid.\n", operations[i].name);
                    return -1;
                }

                num_of_elements = do_operation(operations[i].opcode, cell_array, first_operand, NULL);
                break;
            case 2:
                rc = validate_three_words_in_cmd(split, first_operand, second_operand);
                if (rc != 0)
                {
                    printf("- operator %s is not valid.\n", mov_operation);
                    return 1;
                }
                num_of_elements = do_operation(operations[i].opcode, cell_array, first_operand, second_operand);
                break;
            default:
                printf("Invalid operand num %d", operations[i].operand_num);
                return -1;
            }
        }
    }

    print_elements_in_cell(cell_array, num_of_elements);

    /* operations with ze
    /* operations that are not listed in the opcodes table - fail with error */
    if (!is_found)
    {
        printf("operator %s is not recognized wrong - ERROR!!!\n", inst);
    }
    return 0;
}

/* operations to call */
/* type_source and type_operation are set to invalid_operand if there is no soure/dest operand (e.g. rts, hlt) */
int do_operation(opcodes opcode, cell *cell_array, char *operand_source, char *operand_dest)
{
    operand_type type_source = invalid_operand, type_dest = invalid_operand;
    int value_num_source;
    int value_num_dest;
    int src = 0, dst = 0, element;
    int rc;
    int num_words;

    cell c_source_word = {0};
    cell c_dest_word = {0};

    /* handle src operand */
    if (operand_source != NULL)
    {
        type_source = get_operand_type(operand_source);
        if (type_source == invalid_operand)
        {
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_source, operand_source, &src, &value_num_source);
        if (rc != 0)
        {
            printf("Failed handling the source addressing method\n");
            return -1;
        }
    }

    /* handle dest operand */
    if (operand_dest != NULL)
    {
        type_dest = get_operand_type(operand_dest);
        if (type_dest == invalid_operand)
        {
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_dest, operand_dest, &dst, &value_num_dest);
        if (rc != 0)
        {
            printf("Failed handling the dest addressing method\n");
            return -1;
        }
    }

    /* zero cell is based on the instructions */
    element = 0;
    set_opcode(cell_array, opcode, element, LEFT_SHIFT_OPCODE_COUNT);
    set_field_type(cell_array, src, element, LEFT_SHIFT_SRC_TYPE_COUNT);
    set_dst_type(cell_array, dst, element, LEFT_SHIFT_DST_TYPE_COUNT);
    /* increment element by 1 since the instruciton's word was written  */
    element = 1;

    /* handle special cases  */

    /* handle regsiter and register operands */
    if ((type_source == register_operand) && (type_dest == register_operand))
    {
        set_field_type(cell_array, value_num_source, element, LEFT_SHIFT_BY_SIX);
        set_dst_type(cell_array, value_num_dest, element, LEFT_SHIFT_BY_TWO);
        return 2;
    }

    if (operand_source != NULL)
    { /* handle source operand - the handle_operand_words funciton contains another switch case per each case*/
        num_words = handle_operand_words(type_source, cell_array, element, value_num_source, LEFT_SHIFT_BY_SIX);

        if (num_words == -1)
        {
            printf("Failed getting source operand words.\n");
            return -1;
        }

        element += num_words;
    }

    /* handle dest operand  */
    if (operand_dest != NULL)
    {
        num_words = handle_operand_words(type_dest, cell_array, element, value_num_dest, LEFT_SHIFT_BY_TWO);

        if (num_words == -1)
        {
            printf("Failed getting dest operand words.\n");
            return -1;
        }

        element += num_words;
    }

    /* return total number of words in cell array*/
    return element;
}

/* validate functions */
int is_string_numeric(const char *str)
{
    /* check whether string is not empty */
    if (str[0] == '\0')
    {
        return 1;
    }
    for (int i = 0; str[i] != '\0'; i++)
    {
        /* rc = 0 of isdigit func means the digit is not a number */
        if ((isdigit(str[i])) == 0)
            return 2;
    }
    return 0;
}

int validate_immediate_operand(const char *operand)
{
    int rc;
    printf("validate_immediate_operand: first_char is %c\n", operand[0]);
    /* get the first char of operand and verify it's '#' as required */
    if (operand[0] != '#')
    {
        printf("operand does not start with '#' as required: '%c'\n", operand[0]);
        return 1;
    }
    rc = is_string_numeric(&operand[1]);
    if (rc != 0)
    {
        printf("operand does not contain a numeric number following the '#' char: '%p'\n", &operand[1]);
        return 2;
    }
    return 0;
}

int validate_register_operand(const char *operand)
{
    int register_num = operand[1] - '0';
    printf("register operand is %s\n", operand);
    printf("register number is %d\n", register_num);
    /* 0 false, 1 true */
    int is_valid = 1;
    int num_of_elements_in_array;
    /* get the regiser operand and verify it's correct */
    if (strlen(operand) != 2)
    {
        /* return 1 if the first operand contains more than 2 chars */
        printf("Error parsing operand: %s - a register with 1 numeric identifer (i.e. 'r1') between 0 to 7 is required\n", operand);
        is_valid = 0;
    }
    else if (operand[0] != 'r')
    {
        printf("register symbol does not exist: %c\n", operand[0]);
        is_valid = 0;
    }
    else if ((register_num > 7) || (register_num < 0))
    {
        printf("register number is not valid: %c\n", operand[1]);
        is_valid = 0;
    }
    if (is_valid == 0)
    {
        printf("operand is not legal.\n");
        return 1;
    }
    return 0;
}

int validate_two_words_in_cmd(char *split, char *first_operand)
{
    int is_valid = 1;
    int counter_splitted_words = 0;
    printf("read operator: '%s'\n", split);
    while (split != NULL)
    {
        counter_splitted_words += 1;
        split = strtok(NULL, " ");
        if (counter_splitted_words == 1)
        {
            strcpy(first_operand, split);
        }
    }

    if (counter_splitted_words > 2)
    {
        /* return 2 if more than 2 words were provided */
        printf("Too many operands were provided ");
        return 2;
    }
    return 0;
}

int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand)
{
    int is_valid = 1;
    int counter_splitted_words = 0;
    printf("read operator: '%s'\n", split);
    while (split != NULL)
    {
        split = strtok(NULL, ",");
        counter_splitted_words += 1;
        /* verify the first and second word (cmd + operand) exist as split should not be null */
        if ((split == NULL) && (counter_splitted_words < 3))
        {
            /* no operand was provided */
            if (counter_splitted_words == 1)
            {
                printf("Command requires operands, where none was provided ");
            }
            /* only one was provided */
            else
            {
                printf("Command requires two operands ");
            }
            return 1;
        }
        else if (counter_splitted_words == 1)
        {

            /* copy 2 chars + 1 ('\0' - end of string) */
            strcpy(first_operand, split);
        }
        else if (counter_splitted_words == 2)
        {
            /* copy 2 chars + 1 ('\0' - end of string) */
            strcpy(second_operand, split);
        }
        else if (counter_splitted_words > 3)
        {
            /* return 4 if more than 3 words were provided */
            printf("Too many operands were provided ");
            return 4;
        }
    }
    return 0;
}

void print_elements_in_cell(cell *cell_array, int num_of_elements)
{
    int i;
    char res_buff[100] = {0};
    printf("Printing %d elements in the array of cells.\n", num_of_elements);
    for (i = 0; i < num_of_elements; i++)
    {
        printf("Element #%d bits: %s.\n", i, itoa(cell_array[i].value, res_buff, 2));
    }
}

/* bitwise operator functions */

void set_field_type(cell *cell_array, int src, int element, int shift)
{
    int mask;
    mask = src << shift;
    cell_array[element].value |= mask;
}

void set_dst_type(cell *cell_array, int dst, int element, int shift)
{
    int mask;
    mask = dst << shift;
    cell_array[element].value |= mask;
}

void set_opcode(cell *cell_array, int opcode, int element, int shift)
{
    cell_array[element].value = opcode << shift;
}
