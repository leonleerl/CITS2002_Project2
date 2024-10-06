#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int process_id; // 0 1 2 3
    int page_num;   // 0 1 2 3
    int last_accessed;
} memory, process;

memory page_table[16];
process p1;
process p2;
process p3;
process p4;

int is_page_table_full(memory *page_table)
{
    // Since the page_table is always of size 16, it is always full if all entries are initialized
    return sizeof(page_table) / sizeof(page_table[0]) == 16 ? 1 : 0;
}

// 参数1:in.txt 参数2:out.txt
int main(int argc, char *argv[])
{
    initialise_process();
    // Check if the correct number of arguments is provided
    if (argc != 3)
    {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        perror("Error opening input file");
        return 1;
    }

    // current process id
    int process_id;
    int last_accessed = 0;
    while (fscanf(input_file, "%d", &process_id) != EOF)
    {
    }

    // Close the input file
    fclose(input_file);

    return 0;
}

void initialise_process()
{
    p1.process_id = 1;
    p1.page_num = 99;
    p2.process_id = 2;
    p2.page_num = 99;
    p3.process_id = 99;
    p3.page_num = 3;
    p4.process_id = 4;
    p4.page_num = 99;
}