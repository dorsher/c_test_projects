#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* the cell struct contains the value var with 10 bits
to represent the "word" of the machine code instructions.
*/
typedef struct cell
{
    unsigned short value : 10;
} cell;

cell do_hlt();
cell do_rts();
cell do_not(char operand[]);

int main()
{
    char inst[] = "not r2";
    char res_buff[100] = {0};
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

    if ((strcmp(not_operation, split)) == 0)
    {
        char *split_one_operand  = strtok(split, " ");
        printf("operator %s is valid\n", not_operation);
        res_cell = do_not(operand_param);
        /* itoa - integer to ascii */
        printf("%s bits: %s", not_operation, itoa(res_cell.value, res_buff, 2));
        printf("\nsplit_one_operand = %s", split_one_operand);
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

cell do_not(char operand[])
{
    cell c_not;
    c_not.value = 15 << 6;
    return c_not;
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
