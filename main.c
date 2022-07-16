/* shift + alt + F to indent your code */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMBER_OF_ITEMS_FOR_ONE_OPERAND 2
#define LEFT_SHIFT_SRC_TYPE_COUNT 4
#define LEFT_SHIFT_DST_TYPE_COUNT 2
#define  LEFT_SHIFT_OPCODE_COUNT 6
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
cell do_hlt();
cell do_rts();
int do_not(cell *array, char *operand);
int validate_register_operand(char *operand);
int validate_two_words_in_cmd(char *split, char *first_operand);
int print_elements_in_cell(cell *cell_array, int num_of_elements);

/* decalre bitwise operator functions */

int set_opcode(cell *cell_array, int opcode, int element);
int set_src_type(cell *cell_array, int src, int element);
int set_dst_type(cell *cell_array, int dst, int element);
/*
 set_opcode
 set_src_type
 set_dst_type
*/

int main()
{

    /* temp solution - process the instruction */
    char inst[] = "not r3";
    char res_buff[100] = {0};
    
    /* the first operand splitted by " " */
    char *split = strtok(inst, " ");

    /* operations with one operand */
    char rts_operation[] = "rts";
    char hlt_operation[] = "hlt";

    /* operations with two operands */
    char not_operation[] = "not";
    char clr_operation[] = "clr";
    char inc_operation[] = "inc";
    char dec_operation[] = "dec";
    char jmp_operation[] = "jmp";
    char bne_operation[] = "bne";
    char get_operation[] = "get";
    char prn_operation[] = "prn";
    char jsr_operation[] = "jsr";

    /* operations with three operands */
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
    cell cell_array[5];

    /* not operation */
    if ((strcmp(not_operation, split)) == 0)
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

    /* rts operation */
    else if ((strcmp(rts_operation, split)) == 0)
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

    /* operations that are not listed in the opcodes table - fail with error */
    else
    {
        printf("operator %s is not recognized wrong - ERROR!!!\n", inst);
    }
    return 0;
}

/* operations to call */

int do_not(cell *cell_array, char *operand)
{
    int register_num = operand[1] - '0';
    int src, dst, element;
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
        0000   - 00 -  register_num - 00

    */
    src = 0;
    dst = 3;
    element = 0;
    set_opcode(cell_array, not, element);
    set_src_type(cell_array, src, element);
    set_dst_type(cell_array, dst, element);
    return NUMBER_OF_ITEMS_FOR_ONE_OPERAND;
}

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

int print_elements_in_cell(cell *cell_array, int num_of_elements)
{
    int i;
    char res_buff[100] = {0};
    printf("Printing %d elements in the array of cells.\n", num_of_elements);
    for (i = 0; i < num_of_elements; i++)
    {
        printf("Element #%d bits: %s.\n", i, itoa(cell_array[i].value, res_buff, 2));
    }
    return 0;
}

/* bitwise operator functions */

int set_src_type(cell *cell_array, int src, int element)
{
    int mask;
    mask = src << LEFT_SHIFT_SRC_TYPE_COUNT;
    cell_array[element].value = src | mask;
    return 0;
}

int set_dst_type(cell *cell_array, int dst, int element)
{
    int mask;
    mask = dst << LEFT_SHIFT_DST_TYPE_COUNT;
    cell_array[element].value = dst | mask;
    return 0;
}

int set_opcode(cell *cell_array, int opcode, int element)
{
    cell c_not_first_word = {0};
    c_not_first_word.value = opcode << LEFT_SHIFT_OPCODE_COUNT;
    cell_array[element] = c_not_first_word;
}
