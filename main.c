/* shift + alt + F to indent your code
should have an array of structs (each object has the opcode, src, dst etc) later on you run in a loop... talk with the legend

first pass - macro + creatiti
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
#define MAX_ELEMENTS 100
#define MAX_ELEMENTS_FOR_MACRO 100
#define OPERAND_SIZE 16
#define MAX_ELEMENTS_IN_CELL 16
#define RIGHT_SHIFT_BY_5 5

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

typedef enum
{
    encoding_absolute = 0,
    encoding_extern = 1,
    encoding_relocatable = 2
} encoding_type;

/* struct node to hold char of arrays to get from input file */
typedef struct Node
{
    int address;
    char tag[MAX_ELEMENTS];
    guiding_instructions type;
    int is_entry;
    struct Node *next;
} Node;

/* struct node to store macros */
typedef struct MacroNode
{
    char tag[MAX_ELEMENTS];
    char macro_content[MAX_ELEMENTS_FOR_MACRO];
    struct MacroNode *next;
} MacroNode;

Node *head = NULL;
MacroNode *headMacro = NULL;

/* Declaring functions to be used */

Node *insert_to_end(Node *head);
Node *new_node();
int length(Node *head);
void free_nodes_memory(Node *head);

/* Macro linked list functions */

MacroNode *macro_insert_to_end(MacroNode *headMacro);
MacroNode *macro_new_node();
int macro_length(MacroNode *headMacro);
void macro_free_nodes_memory(MacroNode *headMacro);

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

/* enum of all possible adressing methods of source and dest operands */
typedef enum
{
    immediate_operand = 0,
    direct_operand = 1,
    struct_operand = 2,
    register_operand = 3,
    invalid_operand = 666

} operand_type;


const char base32_array[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";

void convert_to_base_32(cell *cell_array){
    /* mask right = 0000011111 */
    int mask_right = 31;
    int left_index_to_print, right_index_to_print;

    /* left 5 bits of cell */
    left_index_to_print = cell_array->value >> RIGHT_SHIFT_BY_5;
    printf("left_index_to_print = %d, base32 = %c \n", left_index_to_print,  base32_array[left_index_to_print]);

    /* right 5 bits of cell */
    right_index_to_print = mask_right & cell_array->value;
    printf("right_index_to_print = %d, base32 = %c \n", right_index_to_print,  base32_array[right_index_to_print]);

}



/**
 * verify the string contains double quotes
 *
 * @param name  - pointer to a string to
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
int check_string_in_double_quotes(char *name)
{
    int len;
    len = strlen(name);

    if (len == 0)
    {
        printf("The string's lenght empty (0). ERROR.\n");
        return -1;
    }

    if ((name[0] == '"') && name[(len - 1)] == '"')
    {
        return 0;
    }
    printf("The string has no double quotes. ERROR.\n");
    return -1;
}

/**
 * string is checked to verify it's numric or not
 *
 * @param str - a pointer to the string to check whether its numeric
 * @return int - return code ( 0 - okay, otherwise - failure)
 */
int is_string_numeric(const char *str)
{
    int i = 0;
    /* check whether string is not empty */
    if (str[0] == '\0')
    {
        return 1;
    }

    if (str[0] == '+' || str[0] == '-')
    {
        i++;
    }
    for (i; str[i] != '\0'; i++)
    {
        /* rc = 0 of isdigit func means the digit is not a number */
        if ((isdigit(str[i])) == 0)
            return 2;
    }
    return 0;
}

int handle_string_guide(char *name, int do_print_to_output_file)
{
    int len;
    int i;
    int rc;
    char res_buff[MAX_ELEMENTS];

    rc = check_string_in_double_quotes(name);
    if (rc != 0)
    {
        printf("Invalid argument.\n");
        return -1;
    }

    len = strlen(name);
    if (do_print_to_output_file != 0)
    {
        for (i = 1; i < len - 1; i++)
        {
            printf("name: %c, binary: %s\n", name[i], itoa(name[i], res_buff, 2));
        }

        /* Add string null-terminator */
        printf("name: %1c, binary: %s\n", 0, itoa(0, res_buff, 2));
    }

    return len - 1; /* add 1 for null terminator, substract 2 for double quotes */
}

/**
 * every string is trimmed (start and end), and a pointer is returned to the "trimmed" string
 *
 * @param name  - pointer to a string to
 * @return int  - return code ( 0 - okay, otherwise - failure)
 */
char *trim_string_whitespace(char *input)
{
    char *end_input;

    /* get rid of white spaces in start of string */
    while (isspace((unsigned char)*input))
    {
        input++;
    }

    /* if no white spaces, return the string */
    if (*input == '\0')
    {
        return input;
    }

    /* get rid of white spaces in end of string */
    end_input = input + strlen(input) - 1;
    while (end_input > input && isspace((unsigned char)*end_input))
    {
        end_input--;
    }

    /* place '\0' in end of string AFTER remove trailing spaces*/
    end_input[1] = '\0';

    return input;
}

int handle_data_guide(char *name, int do_print_to_output_file)
{
    int size = 0;
    int val_split_decimal;
    char *split = NULL;
    char res_buff[MAX_ELEMENTS];
    split = strtok(name, ",");
    while (split != NULL)
    {
        split = trim_string_whitespace(split);
        if (is_string_numeric(split) != 0)
        {
            return -1;
        }

        val_split_decimal = atoi(split);
        printf("name: %s, decimal: %d, binary: %s\n", split, val_split_decimal, itoa(val_split_decimal, res_buff, 2));
        split = strtok(NULL, ",");
        size++;
    }

    return size;
}

int handle_struct_guide(char *name, int do_print_to_output_file)
{
    int rc;
    char *split = NULL;
    int val_split_decimal;
    char res_buff[MAX_ELEMENTS];

    split = strtok(name, ",");
    split = trim_string_whitespace(split);

    if (is_string_numeric(split) != 0)
    {
        printf("Failed reading the first element (%s) in the struct guide (should be decimal).", split);
        return -1;
    }

    val_split_decimal = atoi(split);

    if (do_print_to_output_file)
    {
        printf("name: %d, binary: %s\n", val_split_decimal, itoa(val_split_decimal, res_buff, 2));
    }

    split = strtok(NULL, "\n");
    split = trim_string_whitespace(split);

    rc = handle_string_guide(split, do_print_to_output_file);
    if (rc == -1)
    {
        printf("Failed reading the second element (%s) in the struct guide (should be a string).", split);
        return -1;
    }

    /* struct always has one word which is the int in the first (left) member */
    return rc + 1;
}

/* identify the guiding type */

/**
 * Get the guide type object
 *
 * @param name - the guide string to parse
 * @return guiding_instructions
 */
guiding_instructions get_guide_type(char *name)
{
    char *split = NULL;
    split = strtok(&name[1], " ");
    split = trim_string_whitespace(split);
    // char * name_guide = strstr(name, entry_guide);
    if (strcmp(split, "entry") == 0)
        return entry_guide;
    if (strcmp(split, "extern") == 0)
        return extern_guide;
    if (strcmp(split, "string") == 0)
        return string_guide;
    if (strcmp(split, "data") == 0)
        return data_guide;
    if (strcmp(split, "struct") == 0)
        return struct_guide;
    return invalid_guide;
}

/* the cell struct contains the value var with 10 bits
to represent the "word" of the machine code instructions.
*/
typedef struct cell
{
    unsigned short value : 10;
} cell;

/* search from word in the linked list */
Node *find_symbol(Node *head, const char *name)
{
    while (head != NULL)
    {
        if (strcmp(head->tag, name) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

/* search for tag in the macro linked list */

char *find_macro_tag(MacroNode *head, const char *name)
{
    while (head != NULL)
    {
        if (strcmp(head->tag, name) == 0)
            return head->macro_content;
        head = head->next;
    }
    return NULL;
}

/* decalre function prototypes*/

/* two operand function prototypes*/
int do_operation(opcodes opcode, cell *cell_array, char *operand_source, char *operand_dest);

/* oprand validator function prototypes*/

int validate_register_operand(const char *operand);
int validate_immediate_operand(const char *operand);
int validate_direct_operand(const char *operand);
int validate_struct_operand(char *operand, int validate_label);
int validate_two_words_in_cmd(char *split, char *first_operand);
int validate_three_words_in_cmd(char *split, char *first_operand, char *second_operand);
void print_elements_in_cell(cell *cell_array, int num_of_elements);

/* decalre bitwise operator functions */

void set_opcode(cell *cell_array, int opcode, int element, int shift);
void set_field_type(cell *cell_array, int src, int element, int shift);
void set_dst_type(cell *cell_array, int dst, int element, int shift);
void set_encoding_type(cell *cell_array, int element, encoding_type type);
/*
 set_opcode
 set_src_type
 set_dst_type
*/

operand_type get_operand_type(const char *operand, int validate_label)
{
    int rc;
    char tmp_operand[OPERAND_SIZE];

    rc = validate_register_operand(operand);
    if (rc == 0)
    {
        return register_operand;
    }
    rc = validate_immediate_operand(operand);
    if (rc == 0)
    {
        return immediate_operand;
    }

    strcpy(tmp_operand, operand);
    rc = validate_struct_operand(tmp_operand, validate_label);
    if (rc == 0)
    {
        return struct_operand;
    }

    if (validate_label != 0)
    {
        rc = validate_direct_operand(operand);
        if (rc == 0)
        {
            return direct_operand;
        }
    }
    else
    {
        return direct_operand;
    }

    return invalid_operand;
}

/* building the inst word (first element in array of words) */
int handle_operand_addressing_method(operand_type type, char *operand, int *method, int *value, int *is_extern)
{
    char *split = NULL;
    Node *symbol = NULL;

    switch (type)
    {
    case immediate_operand:
        *method = 0;
        *value = atoi(&operand[1]);
        break;
    case struct_operand:
        *method = 2;
        split = strtok(operand, ".");
        symbol = find_symbol(head, split);
        if (symbol->type == extern_guide) {
            *is_extern = 1;
        }
        value[0] = symbol->address;
        split = strtok(NULL, ".");
        value[1] = atoi(split);
        break;
    case register_operand:
        *method = 3;
        *value = operand[1] - '0';
        break;
    case direct_operand:
        *method = 1;
        /* value is the address taken from the linked list build during the first bypass*/
        symbol = find_symbol(head, operand);
        *value = symbol->address;
        if (symbol->type == extern_guide)
        {
            *is_extern = 1;
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int handle_operand_words(operand_type type, cell *cell_array, unsigned int index_element, int *value,
                         int shift_only_for_register, int is_extern)
{
    cell c_operand_word = {0};
    cell c_operand_second_word = {0};
    encoding_type encoding;

    switch (type)
    {
    /* handle_operand_words for immediate_operand and direct_operand is the same*/
    case immediate_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        return 1;
    case direct_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
        /* choose the correct encoding */
        set_encoding_type(cell_array, index_element, (is_extern == 1) ? (encoding_extern) : (encoding_relocatable));
        return 1;
    case struct_operand:
        c_operand_word.value = value[0] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element] = c_operand_word;
         /* only the first word is struct (for example: S.1 so only "s" is relevant */
        set_encoding_type(cell_array, index_element, (is_extern == 1) ? (encoding_extern) : (encoding_relocatable));

        c_operand_second_word.value = value[1] << LEFT_SHIFT_BY_TWO;
        cell_array[index_element + 1] = c_operand_second_word;
        return 2;
        break;
    case register_operand:
        set_field_type(cell_array, value[0], index_element, shift_only_for_register);
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

void pre_processor()
{
    /* read line by line and write to other file*/
    FILE *fp_read_from;
    FILE *fp_write_to;
    char line[80];
    char *split = NULL;
    int is_found_macro = 0;
    const char *content;

    /* init first node */
    MacroNode *new_node = NULL;
    MacroNode *current = NULL;

    /* check if file can be opened */
    fp_read_from = fopen("lists_c.txt", "r");
    if (fp_read_from == NULL)
    {
        printf("Could not open the fp_read_from file. Exiting.\n");
        exit(1);
    }

    fp_write_to = fopen("lists_c_COPY_AM.txt", "w");
    while (fgets(line, sizeof(line), fp_read_from))
    {
        split = trim_string_whitespace(line);
        if (strcmp(split, "endmacro") == 0)
        {
            is_found_macro = 0;
            continue;
        }

        if (is_found_macro != 0)
        {
            strcat(current->macro_content, split);
            strcat(current->macro_content, "\n");
            continue;
        }

        if (strstr(split, "macro ") == split)
        {
            split = strtok(line, " ");
            split = strtok(NULL, " ");

            /* make sure tag name is trimmed */
            split = trim_string_whitespace(split);

            /* create a new node in the linked list */
            new_node = macro_insert_to_end(headMacro);
            if (headMacro == NULL)
            {
                headMacro = new_node;
            }
            current = new_node;

            /* add the macro tag to the node */
            strcpy(current->tag, split);
            is_found_macro = 1;
            continue;
        }
        /* before writing to file check for macro */
        content = find_macro_tag(headMacro, split);
        if (content != NULL)
        {
            fprintf(fp_write_to, "%s", content);
            continue;
        }
        fprintf(fp_write_to, "%s\n", line);
    }
    fclose(fp_read_from);
    fclose(fp_write_to);
}

int handle_guide_line(char *guide, int do_print_to_output_file)
{
    Node *symbol = NULL;
    int rc;
    char *split = NULL;
    guiding_instructions guide_rc;

    guide_rc = get_guide_type(guide);

    split = strtok(NULL, "\n");
    split = trim_string_whitespace(split);

    switch (guide_rc)
    {
    case entry_guide:
        /* look for the entry in symbols linked list
        if found - set is_entry to true
        else - throw an error and move on
        */
        symbol = find_symbol(head, split);
        if (symbol != NULL)
        {
            symbol->is_entry = 1;
        }
        else
        {
            printf("ERROR: The entry (%s) cannot be found.", split);
        }
        /* return 0 "words" as nothing should be printed */
        return 0;
    case extern_guide:
        symbol = find_symbol(head, split);
        if (symbol != NULL)
        {
            printf("ERROR: The extern (%s) was found in symbols table.", split);
        }
        else
        {
            symbol = insert_to_end(head);
            if (head == NULL)
            {
                head = symbol;
            }
            symbol->address = 0;
            symbol->type = extern_guide;
            strcpy(symbol->tag, split);
        }
        /* return 0 "words" as nothing should be printed */
        return 0;
    case string_guide:
        printf("split: %s is a string\n", split);
        rc = handle_string_guide(split, do_print_to_output_file);
        if (rc == -1)
        {
            printf("string guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    case data_guide:
        printf("split: %s is a data\n", split);
        rc = handle_data_guide(split, do_print_to_output_file);
        if (rc == -1)
        {
            printf("data guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    case struct_guide:
        printf("split: %s is a struct\n", split);
        rc = handle_struct_guide(split, do_print_to_output_file);
        if (rc == -1)
        {
            printf("struct guiding instruction (%s) is wrong.\n", split);
            return -1;
        }
        break;
    default:
        printf("invalid guiding operand: %s", split);
        return -1;
    }

    return rc;
}

int get_guide_line_length_in_words(char *guide)
{
    return handle_guide_line(guide, 0);
}

int get_operand_length_in_words(operand_type operand)
{
    switch (operand)
    {
    case register_operand:
    case immediate_operand:
    case direct_operand:
        return 1;
    case struct_operand:
        return 2;
    default:
        return 0;
    }
}

int get_operation_line_length_in_words(char *operands)
{
    char *split = NULL;
    int size = 1;
    operand_type src_type = invalid_operand, dst_type = invalid_operand;

    split = strtok(operands, ",");
    if (split != NULL)
    {
        split = trim_string_whitespace(split);
        src_type = get_operand_type(split, 0);
        size += get_operand_length_in_words(src_type);
    }

    split = strtok(NULL, "\n");
    if (split != NULL)
    {
        split = trim_string_whitespace(split);
        dst_type = get_operand_type(split, 0);
        size += get_operand_length_in_words(dst_type);
    }

    if (src_type == register_operand && dst_type == register_operand)
    {
        return 2;
    }

    return size;
}

int get_line_length_in_words(char *line)
{
    int size = 0, i;
    char *split = NULL;
    int is_operation = 0;

    if (line[0] == '.')
    {
        return get_guide_line_length_in_words(line);
    }

    split = strtok(line, " ");
    split = trim_string_whitespace(split);

    for (i = 0; i < sizeof(operations) / sizeof(operations[0]); i++)
    {
        if (strcmp(split, operations[i].name) == 0)
        {
            is_operation = 1;
            break;
        }
    }

    if (is_operation == 0)
    {
        return -1;
    }

    split = strtok(NULL, "\n");
    return get_operation_line_length_in_words(split);
}

void first_pass()
{
    int ic = 0, dc = 0, line_length;
    FILE *fp;
    char *split = NULL;
    char file_name_fp[] = "lists_c.txt";
    char line[80];
    int index = 0;
    int len_linked_list = 0;
    size_t len_line;
    int had_label;

    /* init first node */
    Node *new_node = NULL;
    Node *current = NULL;

    printf("\n----- Start running.\n");

    fp = fopen(file_name_fp, "r");
    /* check if file can be opened */
    if (fp == NULL)
    {
        printf("Could not open the input file. Exiting.\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), fp))
    {
        had_label = 0;
        split = trim_string_whitespace(line);
        if (split[0] == ';' || split[0] == '\0')
        {
            continue;
        }

        len_line = strlen(split);
        split = strtok(split, ":");
        if (strlen(split) != len_line)
        {
            printf("tag is: %s at ic= %d, dc = %d\n", split, ic, dc);
            /* create a new node in the linked list */
            new_node = insert_to_end(head);
            if (head == NULL)
            {
                head = new_node;
            }

            current = new_node;
            /* add the tag to the node */
            strcpy(current->tag, split);
            had_label = 1;

            split = strtok(NULL, "\n");
            split = trim_string_whitespace(split);
        }

        line_length = get_line_length_in_words(split);
        if (line_length == -1)
        {
            printf("ERROR - Invalid line length in %s\n", line);
            continue;
        }

        if (split[0] == '.')
        {
            /* guide line */
            if (had_label)
            {
                current->address = dc;
                current->type = get_guide_type(split);
            }
            printf("Adding len %d to dc=%d\n", line_length, dc);
            dc += line_length;
        }
        else
        {
            /* operation line */
            if (had_label)
            {
                current->address = ic;
                current->type = operation_guide;
            }
            ic += line_length;
        }
    }
    fclose(fp);

    len_linked_list = length(head);
    printf("\n\n----- There are %d elements in the linked list.\n", len_linked_list);
    current = head;

    /* print chars in a friendly user manner*/
    printf("----- Printing the nodes in a user friendly manner:\n");
    while (current != NULL)
    {
        printf("tag is: %s, address: %d\n", current->tag, current->address);
        printf("\n");
        current = current->next;
    }
    /* when done free the memory of all elements in struct by running in a loop from last to first*/
}

int second_pass()
{

    int i;
    int len_line;
    char *split = NULL;
    char line[80];

    FILE *fp = fopen("lists_c.txt", "r");
    /* check if file can be opened */
    if (fp == NULL)
    {
        printf("Could not open the input file. Exiting.\n");
        exit(1);
    }

    /* temp solution - process call and operand */
    char operand_param[10];
    cell res_cell;

    /* temp solution - global vars to handle register operands */
    int num_of_elements;
    char first_operand[OPERAND_SIZE];
    char second_operand[OPERAND_SIZE];
    cell cell_array[MAX_ELEMENTS_IN_CELL];
    int rc;
    int is_found = 0;

    while (fgets(line, sizeof(line), fp))
    {

        /* make sure the line is trimmed before start processing it */
        split = trim_string_whitespace(line);

        /* read each line and parse it */

        /* ignore lines that starts with ';' */

        if (line[0] == ';')
        {
            printf("Line (%s) starts with ';', ignoring.\n", line);
            continue;
        }

        /* check for lables by looking for the ":" delimiter */
        len_line = strlen(line);
        split = strtok(line, ":");
        /* if strlen(split) == len_line then it means there is no ":" in the cmd*/
        if (strlen(split) != len_line)
        {
            /* there is ":" in line*/
            split = strtok(NULL, ":");
            /* line is trimmed and now split is the pointer to the cmd*/
            split = trim_string_whitespace(split);
        }

        /* check for starting with "." */
        if (split[0] == '.')
        {
            /* verify type of guiding line (can be extern, entry, data, string or struct) */
            if (handle_guide_line(split, 1) == -1)
            {
                printf("ERROR - invalid guide line %s\n", split);
            }

            continue;
        }

        /* The memset() function sets the (TBD) TODO */
        memset(cell_array, 0, sizeof(cell_array));
        /* split is the cmd delimitered by " " */
        split = strtok(split, " ");

        for (i = 0; i < sizeof(operations) / sizeof(operations[0]); i++)
        {
            if (strcmp(split, operations[i].name) == 0)
            {
                is_found = 1;
                printf("operator %s is valid\n", split);
                /* operand_num - how many operands should be handled */
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
                        printf("- operator %s is not valid.\n", operations[i].name);
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

        /* operations that are not listed in the opcodes table - fail with error */
        if (!is_found)
        {
            printf("operator %s is not recognized wrong - ERROR!!!\n", split);
        }
    }

    fclose(fp);

    return 0;
}

/* operations to call */
/* type_source and type_operation are set to invalid_operand if there is no soure/dest operand (e.g. rts, hlt) */
int do_operation(opcodes opcode, cell *cell_array, char *operand_source, char *operand_dest)
{
    operand_type type_source = invalid_operand, type_dest = invalid_operand;
    int value_num_source[2];
    int value_num_dest[2];
    int src = 0, dst = 0, element;
    int rc;
    int num_words;
    int src_is_extern = 0, dst_is_extern = 0;

    cell c_source_word = {0};
    cell c_dest_word = {0};

    /* handle src operand */
    if (operand_source != NULL)
    {
        type_source = get_operand_type(operand_source, 1);
        if (type_source == invalid_operand)
        {
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_source, operand_source, &src, value_num_source, &src_is_extern);
        if (rc != 0)
        {
            printf("Failed handling the source addressing method\n");
            return -1;
        }
    }

    /* handle dest operand */
    if (operand_dest != NULL)
    {
        type_dest = get_operand_type(operand_dest, 1);
        if (type_dest == invalid_operand)
        {
            printf("Failed handling the source type\n");
            return -1;
        }

        rc = handle_operand_addressing_method(type_dest, operand_dest, &dst, value_num_dest, &dst_is_extern);
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
        set_field_type(cell_array, value_num_source[0], element, LEFT_SHIFT_BY_SIX);
        set_dst_type(cell_array, value_num_dest[0], element, LEFT_SHIFT_BY_TWO);
        return 2;
    }

    if (operand_source != NULL)
    { /* handle source operand - the handle_operand_words funciton contains another switch case per each case*/
        num_words = handle_operand_words(type_source, cell_array, element, value_num_source, LEFT_SHIFT_BY_SIX, src_is_extern);

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
        num_words = handle_operand_words(type_dest, cell_array, element, value_num_dest, LEFT_SHIFT_BY_TWO, dst_is_extern);

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

int validate_direct_operand(const char *operand)
{
    if (find_symbol(head, operand) != NULL)
    {
        /* operand was found in linked list, return 0 */
        return 0;
    }
    printf("Error: operand (%s) was not found in linked list\n", operand);
    return 1;
}

int validate_struct_operand(char *operand, int validate_label)
{

    int len_line;
    char *split = NULL;
    len_line = strlen(operand);
    split = strtok(operand, ".");

    /* there is A . in line? */
    if (strlen(split) == len_line)
    {
        printf("Error: operand (%s) ia not a struct or data\n", operand);
        return 1;
    }

    if (validate_label != 0)
    {
        /* validate left to the . delimieter */
        if (find_symbol(head, operand) == NULL)
        {
            printf("Error: left to '.' (%s) ia not a direct operand\n", split);
            return 1;
        }
    }

    /* validate right to the . delimieter */
    split = strtok(NULL, ".");
    if (is_string_numeric(split) != 0)
    {
        printf("Error: right to '.' (%s) ia not an immediate operand\n", split);
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
            split = trim_string_whitespace(split);
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
            split = trim_string_whitespace(split);
            /* copy 2 chars + 1 ('\0' - end of string) */
            strcpy(first_operand, split);
        }
        else if (counter_splitted_words == 2)
        {
            split = trim_string_whitespace(split);
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

void set_encoding_type(cell *cell_array, int element, encoding_type type)
{
    int mask;
    /* mask is the encoding type, for example: relocate = 2 */
    mask = type;
    cell_array[element].value |= mask;
}

void set_opcode(cell *cell_array, int opcode, int element, int shift)
{
    cell_array[element].value = opcode << shift;
}

/* new node in linked list */

Node *new_node()
{
    Node *tmp = NULL;
    tmp = (Node *)malloc(sizeof(Node));
    if (tmp != NULL)
    {
        tmp->address = -1;
        tmp->next = NULL;
        tmp->type = invalid_guide;
        tmp->is_entry = 0;
    }
    return tmp;
}

/* insert to end of linked list a new node if needed */

Node *insert_to_end(Node *head)
{
    Node *new_node_to_return = NULL;
    if (head == NULL)
        return new_node();
    else
    {
        Node *tmp_element = head;
        while (tmp_element->next != NULL)
            tmp_element = tmp_element->next;
        new_node_to_return = tmp_element->next = new_node();
    }
    return new_node_to_return;
}

/* get len of linked list  */

int length(Node *head)
{
    if (head == NULL)
        return 0;
    return length(head->next) + 1;
}

/* free the memory of all nodes */

void free_nodes_memory(Node *head)
{
    Node *current = NULL;
    while (head != NULL)
    {
        current = head;
        head = head->next;
        free(current);
    }
}

/* macro - linked list functions */

/* new node in linked list */

MacroNode *macro_new_node()
{
    MacroNode *tmp = NULL;
    tmp = (MacroNode *)malloc(sizeof(MacroNode));
    if (tmp != NULL)
    {
        /* set the first element as null so that the array will not be filled with garbage during strcat later on */
        tmp->macro_content[0] = '\0';
        tmp->next = NULL;
    }
    return tmp;
}

/* insert to end of linked list a new node if needed */

MacroNode *macro_insert_to_end(MacroNode *headMacro)
{
    MacroNode *new_node_to_return = NULL;
    if (headMacro == NULL)
        return macro_new_node();
    else
    {
        MacroNode *tmp_element = headMacro;
        while (tmp_element->next != NULL)
            tmp_element = tmp_element->next;
        new_node_to_return = tmp_element->next = macro_new_node();
    }
    return new_node_to_return;
}

/* get len of linked list  */

int macro_length(MacroNode *head)
{
    if (head == NULL)
        return 0;
    return macro_length(head->next) + 1;
}

/* free the memory of all nodes */

void macro_free_nodes_memory(MacroNode *head)
{
    MacroNode *current = NULL;
    while (head != NULL)
    {
        current = head;
        head = head->next;
        free(current);
    }
}

int main()
{
    // pre_processor();
    first_pass();
    second_pass();

    // free_nodes_memory(head);
    Node *current = head;
    while (current)
    {
        printf("Symbol %s: type %d, address %d, entry? %d\n", current->tag, current->type, current->address, current->is_entry);
        current = current->next;
    }
    return 0;
}
