#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int process_id; // 0 1 2 3
    int page_num;   // 0 1 2 3
    int last_accessed;
} memory;

memory virtual_memory[32]; // 虚拟内存
memory ram[16];            // RAM

void initialize_memory()
{
    // 初始化虚拟内存，假设每个页面的 process_id 和 page_num 分别初始化为对应的值
    for (int i = 0; i < 32; i += 2)
    {
        virtual_memory[i].process_id = i / 8;
        virtual_memory[i].page_num = (i / 2) % 4;
        virtual_memory[i].last_accessed = 0;

        virtual_memory[i + 1] = virtual_memory[i]; // 每个页面占两个位置
    }
    // 初始化 RAM，设为空状态，或者可以用特定值表示空闲
    for (int i = 0; i < 16; i++)
    {
        ram[i].process_id = -1; // -1 表示空闲
    }
}


/*
 * return -1 nowhere available
 * return 0-15, available index
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

int next_page[4] = {0}; // 记录每个进程当前要加载的页面

int main()
{
    FILE *input_file = fopen("in.txt", "r");
    initialize_memory();

    // current process id
    int process_id;
    int last_accessed = 0;
    memory *virtual_memory_p;
    while (fscanf(input_file, "%d", &process_id) != EOF)
    {
        // 获取当前进程的页面号
        int page_num = next_page[process_id];  // 获取下一个需要加载的页
        next_page[process_id] = (page_num + 1) % 4; // 更新下一个页面号（在 0 到 3 之间循环）

        int index = find_ram_available_index();
        // 计算虚拟内存中的页面索引
        int virtual_memory_index = process_id * 8 + page_num * 2; // 计算虚拟内存位置

        // 使用指针指向虚拟内存中的正确位置
        virtual_memory_p = &virtual_memory[virtual_memory_index];

        // if ram is not full
        if (index != -1)
        {
            // 将虚拟内存页面加载到 RAM
            ram[index] = *virtual_memory_p;
            ram[index + 1] = virtual_memory_p[1]; // 加载两个位置

            printf("Process %d's page %d loaded into RAM at index %d\n", process_id, page_num, index);
        }
        // if ram is full
        else
        {
            // 处理 RAM 满的情况（例如 LRU 替换算法）
        }
        last_accessed++;
    }
    // Close the input file
    fclose(input_file);

    return 0;
}

