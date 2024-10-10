//  CITS2002 Project 2 2024
//  Student1:   24169259   Leon Li
//  Platform:   Linux  (or Apple)
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int process_id; // 0 1 2 3
    int page_num; // 0 1 2 3
    int last_accessed;
} memory;


memory virtual_memory[32]; // virtual memory
memory ram[16]; // RAM
int page_num; // page number for the current process
int virtual_memory_index; // index of the current process from virtual memory
int page_table[4][4]; // page table for each process. page_table[i][j] means page number j in process i
int next_page[4]; // next_page[i] means the next page number of process i
int process_id; // current process id from in.txt file
int last_accessed; // When each page is brought into RAM, time step increased by 1.

/**
 * Initialise all data structures
 */
void initialise_memory()
{
    last_accessed = 0;
    for (int i = 0; i < 4; i++)
    {
        next_page[i] = 0;
    }
    for (int i = 0; i < 32; i += 2)
    {
        virtual_memory[i].process_id = i / 8;
        virtual_memory[i].page_num = (i / 2) % 4;
        virtual_memory[i].last_accessed = 0;
        virtual_memory[i + 1] = virtual_memory[i];
    }
    for (int i = 0; i < 16; i++)
    {
        ram[i].process_id = -1;
    }
    for (int pid = 0; pid < 4; pid++)
    {
        for (int page_num = 0; page_num < 4; page_num++)
        {
            page_table[pid][page_num] = 99;
        }
    }
}

/**
 * Find available index in RAM
 * @return:
 *      -1: nowhere available, which means RAM is full
 *      0-15: available index
 */
int find_ram_available_index()
{
    for (int i = 0; i < 16; i++)
    {
        if (ram[i].process_id == -1)
        {
            return i;
        }
    }
    return -1;
}

/**
 * find the index that can be replaced based on LRU algorithm
 * @param is_global 1: search globally, 2: search within the same process
 * @return -1 if not found, otherwise return index
 */
int find_lru_page_index_for_process(const int is_global)
{
    int lru_index = -1;
    int lru_time = __INT_MAX__;
    for (int i = 0; i < 16; i += 2)
    {
        if ((is_global || ram[i].process_id == process_id) && ram[i].last_accessed < lru_time)
        {
            lru_index = i;
            lru_time = ram[i].last_accessed;
        }
    }
    return lru_index;
}

/**
 * load page into RAM
 * @param index current index in RAM
 * @param virtual_memory_index current index in virtual memory
 */
void load_page_into_ram(const int index, const int virtual_memory_index) {
    ram[index] = virtual_memory[virtual_memory_index]; // load the page from virtual memory to RAM
    ram[index + 1] = virtual_memory[virtual_memory_index + 1]; // load the page from virtual memory to RAM

    ram[index].last_accessed = last_accessed; // update last_accessed
    ram[index + 1].last_accessed = last_accessed; // update last_accessed
}

/**
 * update page table
 * @param index current index in RAM
 * @param virtual_memory_index current index in virtual memory
 */
void update_page_table(const int index, const int virtual_memory_index)
{
    page_table[process_id][virtual_memory[virtual_memory_index].page_num] = index / 2; // update page table
}

/**
 * print the outcome into the output file
 * @param file_name the output file name
 */
void write_output(const char* file_name)
{
    FILE* file = fopen(file_name, "w");
    if (!file)
    {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == 3)
            {
                fprintf(file, "%d", page_table[i][j]);
                break;
            }
            fprintf(file, "%d, ", page_table[i][j]);
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < 16; i++)
    {
        if (ram[i].process_id != -1)
        {
            fprintf(file, "%d,%d,%d; ", ram[i].process_id, ram[i].page_num, ram[i].last_accessed);
        }
    }
    fclose(file);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input_file = fopen(argv[1], "r");
    if (!input_file)
    {
        perror("Please input a valid file");
        return EXIT_FAILURE;
    }

    initialise_memory(); // Initialise all data structures

    while (fscanf(input_file, "%d", &process_id) != EOF) // read every process id in each loop from in.txt
    {
        page_num = next_page[process_id];  // get the next page number of the current process
        virtual_memory_index = process_id * 8 + page_num * 2;  // get the current index from virtual memory based on process id and page number
        next_page[process_id] = (page_num + 1) % 4;  // update the next page number

        int index = find_ram_available_index(); // find the available index in RAM

        // if index is -1, RAM need to replace certain page
        if (index == -1)
        {
            index = find_lru_page_index_for_process(0); // search for the index replaceable in the same process based on LRU algorithm
            // if there is no same process in RAM
            if (index == -1)
            {
                index = find_lru_page_index_for_process(1); // search for the index replaceable globally
                page_table[ram[index].process_id][ram[index].page_num] = 99; // update the replaced page in page table
            }
            // if the same process exists in RAM
            else
            {
                page_table[process_id][ram[index].page_num] = 99;// update the replaced page in page table
            }
        }

        load_page_into_ram(index, virtual_memory_index); // load page into ram
        update_page_table(index, virtual_memory_index); // update page table

        ++last_accessed; // time step increased by 1 in each loop
    }

    fclose(input_file); // Close the input file

    write_output(argv[2]); // write to out.txt

    return EXIT_SUCCESS;
}
