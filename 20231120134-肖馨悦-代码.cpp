#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ITEMS 320000  // 定义最大物品数量
#define MAX_CAPACITY 1000000  // 定义最大背包容量

// 物品结构体定义
typedef struct {
    int id;  // 物品编号
    int weight;  // 物品重量
    double value;  // 物品价值
    double density;  // 物品价值密度（价值/重量），用于贪心算法
} Item;

// 最优解结构体
typedef struct {
    double maxValue;  // 当前找到的最大价值
    int totalWeight;  // 当前选中物品的总重量
    int selected[MAX_ITEMS];  // 记录选中的物品编号
    int selectedCount;  // 选中的物品数量
} Solution;

Solution bestSolution;  // 全局最优解变量

// 生成随机物品数据（重量改为整数，简化下标处理）
void generateItems(Item items[], int n, int capacity) {
    srand(time(NULL));  // 使用当前时间作为随机种子
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;  // 物品编号从1开始
        items[i].weight = (rand() % 100) + 1;  // 随机生成1到100之间的整数重量
        // 使用浮点数随机数生成方法，生成100.00到1000.00之间的随机价值
        items[i].value = ((double)rand() / RAND_MAX) * 900.0 + 100.0;
        items[i].density = items[i].value / items[i].weight;  // 计算价值密度
    }
}

// 贪心法比较函数：按价值密度从高到低排序
int compareItems(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    if (itemA->density < itemB->density) return 1;  // 如果A的密度小于B的密度，返回1
    if (itemA->density > itemB->density) return -1;  // 如果A的密度大于B的密度，返回-1
    return 0;  // 如果密度相等，返回0
}

// 计算回溯法上界
double bound(Item items[], int n, int level, int currentWeight, double currentValue, int capacity) {
    if (currentWeight > capacity) return 0;  // 如果当前重量超过背包容量，返回0

    double boundValue = currentValue;  // 当前价值
    int weight = currentWeight;  // 当前重量
    int i = level;

    // 按密度排序后计算上界
    while (i < n && weight + items[i].weight <= capacity) {
        weight += items[i].weight;  // 添加物品重量
        boundValue += items[i].value;  // 添加物品价值
        i++;
    }

    // 估计剩余容量的价值（0-1背包不能部分装入，此处仅作上界估计）
    if (i < n) {
        boundValue += (capacity - weight) * items[i].density;
    }

    return boundValue;
}

// 回溯法核心递归函数
void backtrack(Item items[], int n, int capacity, int level, int currentWeight,
               double currentValue, int selected[], int selectedCount) {
    // 到达叶节点
    if (level == n) {
        if (currentValue > bestSolution.maxValue) {  // 如果当前价值大于最优解价值
            bestSolution.maxValue = currentValue;  // 更新最优解价值
            bestSolution.totalWeight = currentWeight;  // 更新最优解总重量
            bestSolution.selectedCount = selectedCount;  // 更新选中物品数量
            for (int i = 0; i < selectedCount; i++) {
                bestSolution.selected[i] = selected[i];  // 更新选中物品编号
            }
        }
        return;
    }

    // 剪枝：如果上界不超过当前最优解，回溯
    if (bound(items, n, level, currentWeight, currentValue, capacity) <= bestSolution.maxValue) {
        return;
    }

    // 选择当前物品
    if (currentWeight + items[level].weight <= capacity) {
        selected[selectedCount] = level;  // 将当前物品加入选中列表
        backtrack(items, n, capacity, level + 1, currentWeight + items[level].weight,
                  currentValue + items[level].value, selected, selectedCount + 1);
    }

    // 不选择当前物品
    backtrack(items, n, capacity, level + 1, currentWeight, currentValue, selected, selectedCount);
}

// 蛮力法求解0-1背包问题
void bruteForce(Item items[], int n, int capacity) {
    int bestCombination[1000] = {0};  // 存储最优组合
    double maxValue = 0;  // 当前最大价值
    int totalWeight = 0;  // 当前总重量
    int count = 0;  // 选中物品数量

    // 枚举所有2^n种组合（仅适用于n较小的情况）
    for (int i = 0; i < (1 << n); i++) {
        double currentValue = 0;  // 当前组合价值
        int currentWeight = 0;  // 当前组合重量
        int selected[1000] = {0};  // 当前选中物品编号
        int selectedCount = 0;  // 当前选中物品数量

        for (int j = 0; j < n; j++) {
            if (i & (1 << j)) {  // 检查第j位是否为1
                currentValue += items[j].value;  // 累加价值
                currentWeight += items[j].weight;  // 累加重量
                selected[selectedCount++] = j;  // 记录选中物品编号
            }
        }

        // 更新最优解
        if (currentWeight <= capacity && currentValue > maxValue) {
            maxValue = currentValue;  // 更新最大价值
            totalWeight = currentWeight;  // 更新总重量
            count = selectedCount;  // 更新选中物品数量
            for (int k = 0; k < count; k++) {
                bestCombination[k] = selected[k];  // 更新最优组合
            }
        }
    }

    // 输出结果
    printf("蛮力法求解0-1背包问题:\n");
    printf("总价值: %.2f\n", maxValue);
    printf("总重量: %d\n", totalWeight);
    printf("选择的物品:\n");
    for (int i = 0; i < count; i++) {
        int idx = bestCombination[i];
        printf("物品%d: 重量=%d, 价值=%.2f\n", items[idx].id, items[idx].weight, items[idx].value);
    }
}

// 动态规划法求解0-1背包问题
void dynamicProgramming(Item items[], int n, int capacity) {
    double dp[MAX_CAPACITY + 1] = {0};  // 动态规划数组：dp[j]表示容量j时的最大价值
    int selected[MAX_ITEMS] = {0};  // 记录选中的物品
    int selectedCount = 0;  // 选中物品数量

    // 填充动态规划数组
    for (int i = 0; i < n; i++) {
        // j必须是整数，反向遍历容量
        for (int j = capacity; j >= items[i].weight; j--) {
            if (dp[j - items[i].weight] + items[i].value > dp[j]) {
                dp[j] = dp[j - items[i].weight] + items[i].value;  // 更新dp值
            }
        }
    }

    double maxValue = dp[capacity];  // 最大价值
    int totalWeight = 0;  // 总重量

    // 回溯找出选中的物品
    int j = capacity;
    for (int i = n - 1; i >= 0; i--) {
        if (j >= items[i].weight && dp[j] == dp[j - items[i].weight] + items[i].value) {
            selected[selectedCount++] = i;  // 记录选中物品编号
            j -= items[i].weight;  // 减去当前物品重量
            totalWeight += items[i].weight;  // 累加总重量
        }
    }

    // 输出结果
    printf("动态规划法求解0-1背包问题:\n");
    printf("总价值: %.2f\n", maxValue);
    printf("总重量: %d\n", totalWeight);
    printf("选择的物品:\n");
    for (int i = selectedCount - 1; i >= 0; i--) {  // 倒序输出
        int idx = selected[i];
        printf("物品%d: 重量=%d, 价值=%.2f\n", items[idx].id, items[idx].weight, items[idx].value);
    }
}

// 贪心法求解0-1背包问题
void greedy(Item items[], int n, int capacity) {
    Item tempItems[MAX_ITEMS];
    for (int i = 0; i < n; i++) {
        tempItems[i] = items[i];  // 复制物品数组
    }

    // 按价值密度排序
    qsort(tempItems, n, sizeof(Item), compareItems);

    double totalValue = 0;  // 总价值
    int totalWeight = 0;  // 总重量

    // 依次选择价值密度高的物品
    for (int i = 0; i < n; i++) {
        if (totalWeight + tempItems[i].weight <= capacity) {
            totalValue += tempItems[i].value;  // 累加价值
            totalWeight += tempItems[i].weight;  // 累加重量
        }
    }

    // 输出结果
    printf("贪心法求解0-1背包问题:\n");
    printf("总价值: %.2f\n", totalValue);
    printf("总重量: %d\n", totalWeight);
    printf("选择的物品（按价值密度排序）:\n");
    int currentWeight = 0;
    for (int i = 0; i < n && currentWeight + tempItems[i].weight <= capacity; i++) {
        currentWeight += tempItems[i].weight;
        printf("物品%d: 重量=%d, 价值=%.2f, 密度=%.2f\n",
               tempItems[i].id, tempItems[i].weight, tempItems[i].value, tempItems[i].density);
    }
}

// 回溯法求解0-1背包问题
void backtracking(Item items[], int n, int capacity) {
    bestSolution.maxValue = 0;  // 初始化最优解价值
    bestSolution.totalWeight = 0;  // 初始化最优解总重量
    bestSolution.selectedCount = 0;  // 初始化选中物品数量

    Item tempItems[MAX_ITEMS];
    for (int i = 0; i < n; i++) {
        tempItems[i] = items[i];  // 复制物品数组
    }
    qsort(tempItems, n, sizeof(Item), compareItems);  // 按价值密度排序

    int selected[MAX_ITEMS] = {0};  // 记录选中物品编号
    backtrack(tempItems, n, capacity, 0, 0, 0, selected, 0);  // 调用回溯函数

    // 输出结果
    printf("回溯法求解0-1背包问题:\n");
    printf("总价值: %.2f\n", bestSolution.maxValue);
    printf("总重量: %d\n", bestSolution.totalWeight);
    printf("选择的物品:\n");
    for (int i = 0; i < bestSolution.selectedCount; i++) {
        int idx = bestSolution.selected[i];
        printf("物品%d: 重量=%d, 价值=%.2f\n",
               tempItems[idx].id, tempItems[idx].weight, tempItems[idx].value);
    }
}

// 将物品信息写入CSV文件
void writeItemsToCSV(Item items[], int n, const char *filename) {
    FILE *file = fopen(filename, "w");  // 打开文件
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }

    // 写入表头
    fprintf(file, "物品编号,物品重量,物品价值\n");

    // 写入物品信息
    for (int i = 0; i < n; i++) {
        fprintf(file, "%d,%d,%.2f\n", items[i].id, items[i].weight, items[i].value);
    }

    fclose(file);  // 关闭文件
    printf("物品信息已写入文件 %s\n", filename);
}

int main() {
    int n, capacity;
    printf("请输入物品数量n: ");
    scanf("%d", &n);
    printf("请输入背包容量capacity: ");
    scanf("%d", &capacity);

    if (n > MAX_ITEMS) {
        printf("物品数量超过最大限制(%d)!\n", MAX_ITEMS);
        return 1;
    }

    Item items[MAX_ITEMS];
    generateItems(items, n, capacity);  // 生成随机物品数据

    // 将物品信息写入CSV文件
    writeItemsToCSV(items, n, "20231120134-肖馨悦-数据.csv");

    clock_t start, end;
    double cpu_time_used1, cpu_time_used2, cpu_time_used3, cpu_time_used4;

    // 执行蛮力法
    start = clock();
    bruteForce(items, n, capacity);
    end = clock();
    cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("蛮力法执行时间: %.2f ms\n\n", cpu_time_used1);

    // 执行动态规划法
    start = clock();
    dynamicProgramming(items, n, capacity);
    end = clock();
    cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("动态规划法执行时间: %.2f ms\n\n", cpu_time_used2);

    // 执行贪心法
    start = clock();
    greedy(items, n, capacity);
    end = clock();
    cpu_time_used3 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("贪心法执行时间: %.2f ms\n\n", cpu_time_used3);

    // 执行回溯法
    start = clock();
    backtracking(items, n, capacity);
    end = clock();
    cpu_time_used4 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("回溯法执行时间: %.2f ms\n\n\n\n", cpu_time_used4);

    printf("------------执行时间汇总-------------\n");
    printf("蛮力法执行时间: %.2f ms\n\n", cpu_time_used1);
    printf("动态规划法执行时间: %.2f ms\n\n", cpu_time_used2);
    printf("贪心法执行时间: %.2f ms\n\n", cpu_time_used3);
    printf("回溯法执行时间: %.2f ms\n\n", cpu_time_used4);

    return 0;
}
