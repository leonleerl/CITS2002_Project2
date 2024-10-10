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
int page_table[4][4]; // page table for each process. page_table[i][j] means page number j in process i
int next_page[4]; // next_page[i] means the next page number of process i
int process_id; // current process id from in.txt file
int last_accessed; // When each page is brought into RAM, time step increased by 1.
memory* virtual_memory_p; //

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
 * Find available index of RAM
 * return:
 *   -1: nowhere available
 *   0-15: available index
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

/*
 * 找到 ram 中某个进程的 LRU 页面
 * return -1 if not found, otherwise return index
 */
int find_lru_page_index_for_process(int is_global)
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

void write_output(const char* output_file_name)
{
    FILE* output_file = fopen(output_file_name, "w");
    if (!output_file)
    {
        perror("Error opening output file");
        exit(1);
    }

    for (int pid = 0; pid < 4; pid++)
    {
        for (int page_num = 0; page_num < 4; page_num++)
        {
            if (page_num == 3)
            {
                fprintf(output_file, "%d", page_table[pid][page_num]);
                break;
            }
            fprintf(output_file, "%d, ", page_table[pid][page_num]);
        }
        fprintf(output_file, "\n");
    }

    for (int i = 0; i < 16; i++)
    {
        if (ram[i].process_id != -1)
        {
            fprintf(output_file, "%d,%d,%d; ", ram[i].process_id, ram[i].page_num, ram[i].last_accessed);
        }
    }

    fclose(output_file);
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
        perror("无法打开输入文件");
        return EXIT_FAILURE;
    }

    initialise_memory();
    while (fscanf(input_file, "%d", &process_id) != EOF)
    {
        // 获取当前进程的页面号
        int page_num = next_page[process_id];  // 获取下一个需要加载的页
        next_page[process_id] = (page_num + 1) % 4;  // 更新下一个页面号（在 0 到 3 之间循环）

        // 计算虚拟内存中的页面索引
        int virtual_memory_index = process_id * 8 + page_num * 2;  // 计算虚拟内存位置
        virtual_memory_p = &virtual_memory[virtual_memory_index];  // 使用指针指向虚拟内存中的正确位置

        // 查找 RAM 中的可用索引
        int index = find_ram_available_index();

        // 准备执行页面替换（如果需要）
        if (index == -1)
        {
            // 查找最少使用页面的索引
            index = find_lru_page_index_for_process(0);
            // 如果没有相同进程的页面，执行全局替换
            if (index == -1)
            {
                index = find_lru_page_index_for_process(1);
                // 更新被替换页面的页表
                page_table[ram[index].process_id][ram[index].page_num] = 99;  // 标记为在磁盘中
            }
            else
            {
                // 同一进程在 RAM 中，标记其页面为在磁盘中
                page_table[process_id][ram[index].page_num] = 99;
            }
        }

        // 将虚拟内存页面加载到 RAM
        ram[index] = *virtual_memory_p++;
        ram[index + 1] = *virtual_memory_p;  // 加载两个位置

        // 更新 last_accessed
        ram[index].last_accessed = last_accessed;
        ram[index + 1].last_accessed = last_accessed;

        // 更新页表
        page_table[process_id][page_num] = index / 2;

        ++last_accessed;
    }

    // Close the input file
    fclose(input_file);

    // 写入输出文件
    write_output(argv[2]);

    return EXIT_SUCCESS;
}
