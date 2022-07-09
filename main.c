#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 10

/* the cell struct contains the value var with 10 bits
to represent the "word" of the machine code instructions.
*/
typedef struct cell
{
    unsigned short value : 10;
} cell;

cell do_hlt();
cell do_rts();
int do_not(char *operand, cell *array);

int main()
{
    char inst[] = "not r0";
    char res_buff[100] = {0};
    char first_operand[2];
    char *split = strtok(inst, " ");

    /* operations with one operand */
    char rts_operation[] = "rts";
    char hlt_operation[] = "hlt";

    /* operations with two operands */
    char not_operation[] = "not";
    char clr_operation[] = "clr";
    char inc_operation[] = "inc";
    char get_operation[] = "get";
    char prn_operation[] = "prn";
    char operand_param[10];
    cell res_cell;
    cell cell_array [5];
    int num_of_elements; 
    
    int counter_splitted_words = 0;
    if ((strcmp(not_operation, split)) == 0)
    {
        int is_valid = 1;
        printf("read operator: %s\n", not_operation);
        while (split != NULL)
        {
            counter_splitted_words += 1;
            split = strtok(NULL, " ");
            if (counter_splitted_words == 1)
            {
                printf("first_operand is %s\n", split);
                if (strlen(split) > 2)
                {
                    is_valid = 0;
                }
                else{
                    strncpy(first_operand, split, 2);
                }
            }
        }
        if ((counter_splitted_words > 2) || (is_valid == 0))
        {
            printf("operator %s is not valid.\n", not_operation);
        }
        else
        {
            num_of_elements = do_not(first_operand, cell_array);
            printf("There are %d elements in the array of cells.\n", num_of_elements);
            for(int i =0; i<num_of_elements; i++){
                printf("Element #%d bits: %s.\n", i, itoa(cell_array[i].value, res_buff, 2));
            }
        }
    }

    else if ((strcmp(rts_operation, split)) == 0)
    {
        printf("operator %s is valid\n", rts_operation);
        res_cell = do_rts();
        /* itoa - integer to ascii */
        printf("%s bits: %s", rts_operation, itoa(res_cell.value, res_buff, 2));
    }

    else if ((strcmp(hlt_operation, split)) == 0)
    {

        printf("operator %s is valid\n", hlt_operation);
        res_cell = do_hlt();
        /* itoa - integer to ascii */
        printf("%s bits: %s", hlt_operation, itoa(res_cell.value, res_buff, 2));
    }

    else if ((strcmp(hlt_operation, split)) == 0)
    {

        printf("operator %s is valid\n", hlt_operation);
        res_cell = do_hlt();
        /* itoa - integer to ascii */
        printf("rts bits: %s", itoa(res_cell.value, res_buff, 2));
    }

    else
    {
        printf("operator %s is not recognized wrong - ERROR!!!\n", inst);
    }
    return 0;
}

/* functions to be used: */

int do_not(char operand[], cell *cell_array)
{
    cell c_not_first_word;
    cell c_not_second_word;
    int register_num = operand[1] - '0';
    printf("register number is %d\n", register_num);
    /* 0 false, 1 true */
    int is_valid = 1;
    int num_of_elements_in_array;
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
    }
    else
    {
        printf("operand is legal.\n");
        c_not_first_word.value = 15 << 6;
        c_not_second_word.value = 15 << 6;
        cell_array[0] = c_not_first_word;
        cell_array[1] = c_not_second_word;
        num_of_elements_in_array = sizeof(cell_array) / sizeof(cell);
        return num_of_elements_in_array;
    }
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
