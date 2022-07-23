/* shift + alt + F to indent your code */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMBER_OF_ITEMS_FOR_ONE_OPERAND 2
#define LEFT_SHIFT_SRC_TYPE_COUNT 4
#define LEFT_SHIFT_DST_TYPE_COUNT 2
#define LEFT_SHIFT_OPCODE_COUNT 6
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

typedef struct cell
{
    unsigned short value : 10;
} cell;

/* decalre function prototypes*/

/* zero operand function prototypes*/
cell do_hlt();
cell do_rts();

/* one operand function prototypes*/
int do_not(cell *array, char *operand);
int do_clr(cell *array, char *operand);
int do_inc(cell *array, char *operand);
int do_dec(cell *array, char *operand);
int do_get(cell *array, char *operand);
int do_prn(cell *array, char *operand);

/* two operand function prototypes*/
int do_mov(cell *array, char *operand);
// int do_cmp(cell *array, char *operand);
// int do_add(cell *array, char *operand);
// int do_sub(cell *array, char *operand);
// int do_lea(cell *array, char *operand);

/* oprand validator function prototypes*/

int validate_register_operand(char *operand);
int validate_two_words_in_cmd(char *split, char *first_operand);
int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand);
void print_elements_in_cell(cell *cell_array, int num_of_elements);

/* decalre bitwise operator functions */

void set_opcode(cell *cell_array, int opcode, int element);
void set_src_type(cell *cell_array, int src, int element);
void set_dst_type(cell *cell_array, int dst, int element);
/*
 set_opcode
 set_src_type
 set_dst_type
*/

int main()
{

    /* temp solution - process the instruction */
    char inst[] = "mov r7 r3";
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
    char first_operand[2];
    char second_operand[2];
    cell cell_array[5];

    /* operations with zero operands */

    /* rts operation */
    if ((strcmp(rts_operation, split)) == 0)
    {
        printf("operator %s is valid\n", rts_operation);
        res_cell = do_rts();
        /* itoa - integer to ascii */
        printf("%s bits: %s", rts_operation, itoa(res_cell.value, res_buff, 2));
    }

    /* hlt operation */
    else if ((strcmp(hlt_operation, split)) == 0)
    {

        printf("operator %s is valid\n", hlt_operation);
        res_cell = do_hlt();
        /* itoa - integer to ascii */
        printf("%s bits: %s", hlt_operation, itoa(res_cell.value, res_buff, 2));
    }

    /* operations with one operands */

    /* not operation */
    else if ((strcmp(not_operation, split)) == 0)
    {

        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", not_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", not_operation);
            }
            else
            {
                num_of_elements = do_not(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* clr operation */
    else if ((strcmp(clr_operation, split)) == 0)
    {
        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", clr_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", clr_operation);
            }
            else
            {
                num_of_elements = do_clr(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* inc operation */
    else if ((strcmp(inc_operation, split)) == 0)
    {
        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", inc_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", inc_operation);
            }
            else
            {
                num_of_elements = do_inc(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* dec operation */
    else if ((strcmp(dec_operation, split)) == 0)
    {
        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", dec_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", dec_operation);
            }
            else
            {
                num_of_elements = do_dec(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* get operation */
    else if ((strcmp(get_operation, split)) == 0)
    {
        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", get_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", get_operation);
            }
            else
            {
                num_of_elements = do_get(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* prn operation */
    else if ((strcmp(prn_operation, split)) == 0)
    {
        int rc;
        int i;
        /* verify there are two words in command */
        rc = validate_two_words_in_cmd(split, first_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", prn_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            rc = validate_register_operand(first_operand);
            if (rc != 0)
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", prn_operation);
            }
            else
            {
                num_of_elements = do_prn(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* operations with two operands */

    /* mov operation */
    else if ((strcmp(mov_operation, split)) == 0)
    {
        int rc;
        int rc_first_operand;
        int rc_second_operand;
        int i;
        /* verify there are three words in command */
        rc = validate_three_words_in_cmd(split, first_operand, second_operand);
        if (rc != 0)
        {
            printf("- operator %s is not valid.\n", mov_operation);
        }
        /* verify register is valid and then print the words (bits) */
        else
        {
            printf("first operand: %s\n", first_operand);
            printf("second operand: %s\n", second_operand);
            rc_first_operand = validate_register_operand(first_operand);
            rc_second_operand = validate_register_operand(second_operand);
            if ((rc_first_operand != 0) || (rc_second_operand != 0))
            {
                printf("Operand '%s' is not legal. Register is not valid.\n", mov_operation);
            }
            else
            {
                num_of_elements = do_mov(cell_array, first_operand);
                print_elements_in_cell(cell_array, num_of_elements);
            }
        }
    }

    /* operations that are not listed in the opcodes table - fail with error */
    else
    {
        printf("operator %s is not recognized wrong - ERROR!!!\n", inst);
    }
    return 0;
}

/* operations to call */

/* operations with zero operand */

cell do_hlt()
{
    cell c_hlt;
    c_hlt.value = 15 << 6;
    return c_hlt;
}

cell do_rts()
{
    cell c_rts;
    c_rts.value = 14 << 6;
    return c_rts;
}

/* operations with one operand */

int do_not(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_not_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        0100   -  00 -  11   - 00
    the 'not' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, not, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_not_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_not_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

int do_clr(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_clr_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        0110   -  00 -  11   - 00
    the 'clr' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, clr, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_clr_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_clr_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

int do_inc(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_inc_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        0111   -  00 -  11   - 00
    the 'inc' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, inc, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_inc_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_inc_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

int do_dec(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_dec_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        1000   -  00 -  11   - 00
    the 'dec' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, dec, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_dec_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_dec_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

int do_get(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_get_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        1011   -  00 -  11   - 00
    the 'get' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, get, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_get_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_get_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

int do_prn(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
    cell c_prn_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        1100   -  00 -  11   - 00
    the 'prn' value is taken from the opecodes enum */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, prn, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode - src - dst - ARE
        0000   - 00 -  register_num - 00 */
    element = 1;
    c_prn_first_word.value = register_num << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_prn_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

/* operations with two operands */

int do_mov(cell *cell_array, char *operand)
{
    int register_num_src_01 = operand[1] - '0';
    int register_num_dst_02 = operand[2] - '0';
    int src, dst, element;
    cell c_mov_first_word = {0};
    /* first cell is based on the instructions
        opcode - src - dst - ARE
        0000   -  00 -  11   - 00
    the 'mov' value is taken from the opecodes enum */
    src = 3;
    dst = 3;
    element = 0;
    set_opcode(cell_array, mov, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    /* second cell is the register
        opcode          src                     dst          ARE
        0000    register_num_src_01    register_num_dst_02    00 */
    element = 1;
    c_mov_first_word.value = register_num_src_01 << LEFT_SHIFT_SRC_TYPE_COUNT;
    c_mov_first_word.value = register_num_dst_02 << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element] = c_mov_first_word;
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

/* validate functions */

int validate_register_operand(char *operand)
{
    int register_num = operand[1] - '0';
    printf("register operand is %s\n", operand);
    printf("register number is %d\n", register_num);
    /* 0 false, 1 true */
    int is_valid = 1;
    int num_of_elements_in_array;
    /* get the regiser operand and verify it's correct */
    if (operand[0] != 'r')
    {
        printf("register symbol does not exist: %c\n", operand[0]);
        is_valid = 0;
    }
    if ((register_num > 7) || (register_num < 0))
    {
        printf("register number is not valid: %c", operand[1]);
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
            if (strlen(split) > 2)
            {
                /* return 1 if the first operand contains more than 2 chars */
                printf("A register with 1 numeric identifer (i.e. 'r1') is required ");
                return 1;
            }
            else
            {
                /* copy 2 chars + 1 ('\0' - end of string) */
                strncpy(first_operand, split, 3);
            }
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
        counter_splitted_words += 1;
        split = strtok(NULL, " ");
        printf("first_operand is: '%s', counter = %d\n", first_operand, counter_splitted_words);
        printf("second_operand is: '%s', counter = %d\n", second_operand, counter_splitted_words);
        if (counter_splitted_words == 1)
        {
            if (strlen(split) > 3)
            {
                /* return 1 if the first operand contains more than 2 chars */
                printf("A register with 1 numeric identifer (i.e. 'r1') is required ");
                return 1;
            }
            else
            {
                /* copy 2 chars + 1 ('\0' - end of string) */
                strncpy(first_operand, split, 3);
                printf("first_operand copied: '%s'\n", first_operand);
            }
        }
        if (counter_splitted_words == 2)
        {
            if (strlen(split) > 3)
            {
                /* return 1 if the first operand contains more than 2 chars */
                printf("A register with 1 numeric identifer (i.e. 'r1') is required ");
                return 1;
            }
            else
            {
                /* copy 2 chars + 1 ('\0' - end of string) */
                strncpy(second_operand, split, 3);
                printf("second_operand copied: '%s'\n", second_operand);
            }
        }
    }
    printf("outside the while loop, first_operand is: '%s'\n", first_operand);
    printf("outside the while loop, second_operand is: '%s'\n", second_operand);
    if (counter_splitted_words > 3)
    {
        /* return 3 if more than 3 words were provided */
        printf("Too many operands were provided ");
        return 3;
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

void set_src_type(cell *cell_array, int src, int element)
{
    int mask;
    mask = src << LEFT_SHIFT_SRC_TYPE_COUNT;
    cell_array[element].value |= mask;
}

void set_dst_type(cell *cell_array, int dst, int element)
{
    int mask;
    mask = dst << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element].value |= mask;
}

void set_opcode(cell *cell_array, int opcode, int element)
{
    cell_array[element].value = opcode << LEFT_SHIFT_OPCODE_COUNT;
}
