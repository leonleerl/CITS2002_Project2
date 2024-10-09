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

/*
 * 找到 RAM 中某个进程的 LRU 页面
 * return -1 if not found, otherwise return index
 */
int find_lru_page_for_process(int process_id)
{
    int lru_index = -1;
    int lru_time = __INT_MAX__;
    for (int i = 0; i < 16; i += 2)
    {
        if (ram[i].process_id == process_id && ram[i].last_accessed < lru_time)
        {
            lru_index = i;
            lru_time = ram[i].last_accessed;
        }
    }
    return lru_index;
}

/*
 * 找到 RAM 中的全局 LRU 页面
 * return index of the LRU page
 */
int find_global_lru_page()
{
    int lru_index = -1;
    int lru_time = __INT_MAX__;
    for (int i = 0; i < 16; i += 2)
    {
        if (ram[i].last_accessed < lru_time)
        {
            lru_index = i;
            lru_time = ram[i].last_accessed;
        }
    }
    return lru_index;
}

int main()
{
    FILE *input_file = fopen("in.txt", "r");
    if (!input_file) {
        perror("无法打开输入文件");
        return 1;
    }

    initialize_memory();

    // current process id
    int process_id;
    int last_accessed = 0;
    memory *virtual_memory_p;
    while (fscanf(input_file, "%d", &process_id) != EOF)
    {
        printf("本轮进入进程：%d\n", process_id);
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

            // 更新 last_accessed
            ram[index].last_accessed = ++last_accessed;
            ram[index + 1].last_accessed = last_accessed;

            // printf("Process %d's page %d loaded into RAM at index %d\n", process_id, page_num, index);
        }
        // if ram is full
        else
        {
            // 查找是否有同一进程的页面在 RAM 中
            int lru_index = find_lru_page_for_process(process_id);
            if (lru_index != -1)
            {
                // 有同一进程的页面，替换 LRU 页面
                ram[lru_index] = *virtual_memory_p;
                ram[lru_index + 1] = virtual_memory_p[1]; // 加载两个位置

                // 更新 last_accessed
                ram[lru_index].last_accessed = ++last_accessed;
                ram[lru_index + 1].last_accessed = last_accessed;

                printf("Process %d's page %d replaced its LRU page in RAM at index %d\n", process_id, page_num, lru_index);
            }
            else
            {
                // 没有同一进程的页面，进行全局 LRU 替换
                lru_index = find_global_lru_page();

                ram[lru_index] = *virtual_memory_p;
                ram[lru_index + 1] = virtual_memory_p[1]; // 加载两个位置

                // 更新 last_accessed
                ram[lru_index].last_accessed = ++last_accessed;
                ram[lru_index + 1].last_accessed = last_accessed;

                printf("Process %d's page %d replaced global LRU page in RAM at index %d\n", process_id, page_num, lru_index);
            }
        }
        // 打印 RAM 的所有内容
        printf("RAM 内容:\n");
        for (int i = 0; i < 16; i++)
        {
            if (ram[i].process_id != -1)
            {
                printf("Index %d: Process %d, Page %d, Last Accessed %d\n", i, ram[i].process_id, ram[i].page_num, ram[i].last_accessed);
            }
            else
            {
                printf("Index %d: 空闲\n", i);
            }
        }
        printf("=================================\n");
    }
    // Close the input file
    fclose(input_file);



    // // 打印虚拟内存的所有内容
    // printf("\n虚拟内存内容:\n");
    // for (int i = 0; i < 32; i++)
    // {
    //     printf("Index %d: Process %d, Page %d, Last Accessed %d\n", i, virtual_memory[i].process_id, virtual_memory[i].page_num, virtual_memory[i].last_accessed);
    // }

    return 0;
}