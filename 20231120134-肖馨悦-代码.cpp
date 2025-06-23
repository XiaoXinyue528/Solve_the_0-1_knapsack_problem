#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ITEMS 320000  // ���������Ʒ����
#define MAX_CAPACITY 1000000  // ������󱳰�����

// ��Ʒ�ṹ�嶨��
typedef struct {
    int id;  // ��Ʒ���
    int weight;  // ��Ʒ����
    double value;  // ��Ʒ��ֵ
    double density;  // ��Ʒ��ֵ�ܶȣ���ֵ/������������̰���㷨
} Item;

// ���Ž�ṹ��
typedef struct {
    double maxValue;  // ��ǰ�ҵ�������ֵ
    int totalWeight;  // ��ǰѡ����Ʒ��������
    int selected[MAX_ITEMS];  // ��¼ѡ�е���Ʒ���
    int selectedCount;  // ѡ�е���Ʒ����
} Solution;

Solution bestSolution;  // ȫ�����Ž����

// ���������Ʒ���ݣ�������Ϊ���������±괦��
void generateItems(Item items[], int n, int capacity) {
    srand(time(NULL));  // ʹ�õ�ǰʱ����Ϊ�������
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;  // ��Ʒ��Ŵ�1��ʼ
        items[i].weight = (rand() % 100) + 1;  // �������1��100֮�����������
        // ʹ�ø�������������ɷ���������100.00��1000.00֮��������ֵ
        items[i].value = ((double)rand() / RAND_MAX) * 900.0 + 100.0;
        items[i].density = items[i].value / items[i].weight;  // �����ֵ�ܶ�
    }
}

// ̰�ķ��ȽϺ���������ֵ�ܶȴӸߵ�������
int compareItems(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    if (itemA->density < itemB->density) return 1;  // ���A���ܶ�С��B���ܶȣ�����1
    if (itemA->density > itemB->density) return -1;  // ���A���ܶȴ���B���ܶȣ�����-1
    return 0;  // ����ܶ���ȣ�����0
}

// ������ݷ��Ͻ�
double bound(Item items[], int n, int level, int currentWeight, double currentValue, int capacity) {
    if (currentWeight > capacity) return 0;  // �����ǰ����������������������0

    double boundValue = currentValue;  // ��ǰ��ֵ
    int weight = currentWeight;  // ��ǰ����
    int i = level;

    // ���ܶ����������Ͻ�
    while (i < n && weight + items[i].weight <= capacity) {
        weight += items[i].weight;  // �����Ʒ����
        boundValue += items[i].value;  // �����Ʒ��ֵ
        i++;
    }

    // ����ʣ�������ļ�ֵ��0-1�������ܲ���װ�룬�˴������Ͻ���ƣ�
    if (i < n) {
        boundValue += (capacity - weight) * items[i].density;
    }

    return boundValue;
}

// ���ݷ����ĵݹ麯��
void backtrack(Item items[], int n, int capacity, int level, int currentWeight,
               double currentValue, int selected[], int selectedCount) {
    // ����Ҷ�ڵ�
    if (level == n) {
        if (currentValue > bestSolution.maxValue) {  // �����ǰ��ֵ�������Ž��ֵ
            bestSolution.maxValue = currentValue;  // �������Ž��ֵ
            bestSolution.totalWeight = currentWeight;  // �������Ž�������
            bestSolution.selectedCount = selectedCount;  // ����ѡ����Ʒ����
            for (int i = 0; i < selectedCount; i++) {
                bestSolution.selected[i] = selected[i];  // ����ѡ����Ʒ���
            }
        }
        return;
    }

    // ��֦������Ͻ粻������ǰ���Ž⣬����
    if (bound(items, n, level, currentWeight, currentValue, capacity) <= bestSolution.maxValue) {
        return;
    }

    // ѡ��ǰ��Ʒ
    if (currentWeight + items[level].weight <= capacity) {
        selected[selectedCount] = level;  // ����ǰ��Ʒ����ѡ���б�
        backtrack(items, n, capacity, level + 1, currentWeight + items[level].weight,
                  currentValue + items[level].value, selected, selectedCount + 1);
    }

    // ��ѡ��ǰ��Ʒ
    backtrack(items, n, capacity, level + 1, currentWeight, currentValue, selected, selectedCount);
}

// ���������0-1��������
void bruteForce(Item items[], int n, int capacity) {
    int bestCombination[1000] = {0};  // �洢�������
    double maxValue = 0;  // ��ǰ����ֵ
    int totalWeight = 0;  // ��ǰ������
    int count = 0;  // ѡ����Ʒ����

    // ö������2^n����ϣ���������n��С�������
    for (int i = 0; i < (1 << n); i++) {
        double currentValue = 0;  // ��ǰ��ϼ�ֵ
        int currentWeight = 0;  // ��ǰ�������
        int selected[1000] = {0};  // ��ǰѡ����Ʒ���
        int selectedCount = 0;  // ��ǰѡ����Ʒ����

        for (int j = 0; j < n; j++) {
            if (i & (1 << j)) {  // ����jλ�Ƿ�Ϊ1
                currentValue += items[j].value;  // �ۼӼ�ֵ
                currentWeight += items[j].weight;  // �ۼ�����
                selected[selectedCount++] = j;  // ��¼ѡ����Ʒ���
            }
        }

        // �������Ž�
        if (currentWeight <= capacity && currentValue > maxValue) {
            maxValue = currentValue;  // ��������ֵ
            totalWeight = currentWeight;  // ����������
            count = selectedCount;  // ����ѡ����Ʒ����
            for (int k = 0; k < count; k++) {
                bestCombination[k] = selected[k];  // �����������
            }
        }
    }

    // ������
    printf("���������0-1��������:\n");
    printf("�ܼ�ֵ: %.2f\n", maxValue);
    printf("������: %d\n", totalWeight);
    printf("ѡ�����Ʒ:\n");
    for (int i = 0; i < count; i++) {
        int idx = bestCombination[i];
        printf("��Ʒ%d: ����=%d, ��ֵ=%.2f\n", items[idx].id, items[idx].weight, items[idx].value);
    }
}

// ��̬�滮�����0-1��������
void dynamicProgramming(Item items[], int n, int capacity) {
    double dp[MAX_CAPACITY + 1] = {0};  // ��̬�滮���飺dp[j]��ʾ����jʱ������ֵ
    int selected[MAX_ITEMS] = {0};  // ��¼ѡ�е���Ʒ
    int selectedCount = 0;  // ѡ����Ʒ����

    // ��䶯̬�滮����
    for (int i = 0; i < n; i++) {
        // j�����������������������
        for (int j = capacity; j >= items[i].weight; j--) {
            if (dp[j - items[i].weight] + items[i].value > dp[j]) {
                dp[j] = dp[j - items[i].weight] + items[i].value;  // ����dpֵ
            }
        }
    }

    double maxValue = dp[capacity];  // ����ֵ
    int totalWeight = 0;  // ������

    // �����ҳ�ѡ�е���Ʒ
    int j = capacity;
    for (int i = n - 1; i >= 0; i--) {
        if (j >= items[i].weight && dp[j] == dp[j - items[i].weight] + items[i].value) {
            selected[selectedCount++] = i;  // ��¼ѡ����Ʒ���
            j -= items[i].weight;  // ��ȥ��ǰ��Ʒ����
            totalWeight += items[i].weight;  // �ۼ�������
        }
    }

    // ������
    printf("��̬�滮�����0-1��������:\n");
    printf("�ܼ�ֵ: %.2f\n", maxValue);
    printf("������: %d\n", totalWeight);
    printf("ѡ�����Ʒ:\n");
    for (int i = selectedCount - 1; i >= 0; i--) {  // �������
        int idx = selected[i];
        printf("��Ʒ%d: ����=%d, ��ֵ=%.2f\n", items[idx].id, items[idx].weight, items[idx].value);
    }
}

// ̰�ķ����0-1��������
void greedy(Item items[], int n, int capacity) {
    Item tempItems[MAX_ITEMS];
    for (int i = 0; i < n; i++) {
        tempItems[i] = items[i];  // ������Ʒ����
    }

    // ����ֵ�ܶ�����
    qsort(tempItems, n, sizeof(Item), compareItems);

    double totalValue = 0;  // �ܼ�ֵ
    int totalWeight = 0;  // ������

    // ����ѡ���ֵ�ܶȸߵ���Ʒ
    for (int i = 0; i < n; i++) {
        if (totalWeight + tempItems[i].weight <= capacity) {
            totalValue += tempItems[i].value;  // �ۼӼ�ֵ
            totalWeight += tempItems[i].weight;  // �ۼ�����
        }
    }

    // ������
    printf("̰�ķ����0-1��������:\n");
    printf("�ܼ�ֵ: %.2f\n", totalValue);
    printf("������: %d\n", totalWeight);
    printf("ѡ�����Ʒ������ֵ�ܶ�����:\n");
    int currentWeight = 0;
    for (int i = 0; i < n && currentWeight + tempItems[i].weight <= capacity; i++) {
        currentWeight += tempItems[i].weight;
        printf("��Ʒ%d: ����=%d, ��ֵ=%.2f, �ܶ�=%.2f\n",
               tempItems[i].id, tempItems[i].weight, tempItems[i].value, tempItems[i].density);
    }
}

// ���ݷ����0-1��������
void backtracking(Item items[], int n, int capacity) {
    bestSolution.maxValue = 0;  // ��ʼ�����Ž��ֵ
    bestSolution.totalWeight = 0;  // ��ʼ�����Ž�������
    bestSolution.selectedCount = 0;  // ��ʼ��ѡ����Ʒ����

    Item tempItems[MAX_ITEMS];
    for (int i = 0; i < n; i++) {
        tempItems[i] = items[i];  // ������Ʒ����
    }
    qsort(tempItems, n, sizeof(Item), compareItems);  // ����ֵ�ܶ�����

    int selected[MAX_ITEMS] = {0};  // ��¼ѡ����Ʒ���
    backtrack(tempItems, n, capacity, 0, 0, 0, selected, 0);  // ���û��ݺ���

    // ������
    printf("���ݷ����0-1��������:\n");
    printf("�ܼ�ֵ: %.2f\n", bestSolution.maxValue);
    printf("������: %d\n", bestSolution.totalWeight);
    printf("ѡ�����Ʒ:\n");
    for (int i = 0; i < bestSolution.selectedCount; i++) {
        int idx = bestSolution.selected[i];
        printf("��Ʒ%d: ����=%d, ��ֵ=%.2f\n",
               tempItems[idx].id, tempItems[idx].weight, tempItems[idx].value);
    }
}

// ����Ʒ��Ϣд��CSV�ļ�
void writeItemsToCSV(Item items[], int n, const char *filename) {
    FILE *file = fopen(filename, "w");  // ���ļ�
    if (file == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }

    // д���ͷ
    fprintf(file, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");

    // д����Ʒ��Ϣ
    for (int i = 0; i < n; i++) {
        fprintf(file, "%d,%d,%.2f\n", items[i].id, items[i].weight, items[i].value);
    }

    fclose(file);  // �ر��ļ�
    printf("��Ʒ��Ϣ��д���ļ� %s\n", filename);
}

int main() {
    int n, capacity;
    printf("��������Ʒ����n: ");
    scanf("%d", &n);
    printf("�����뱳������capacity: ");
    scanf("%d", &capacity);

    if (n > MAX_ITEMS) {
        printf("��Ʒ���������������(%d)!\n", MAX_ITEMS);
        return 1;
    }

    Item items[MAX_ITEMS];
    generateItems(items, n, capacity);  // ���������Ʒ����

    // ����Ʒ��Ϣд��CSV�ļ�
    writeItemsToCSV(items, n, "20231120134-Фܰ��-����.csv");

    clock_t start, end;
    double cpu_time_used1, cpu_time_used2, cpu_time_used3, cpu_time_used4;

    // ִ��������
    start = clock();
    bruteForce(items, n, capacity);
    end = clock();
    cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("������ִ��ʱ��: %.2f ms\n\n", cpu_time_used1);

    // ִ�ж�̬�滮��
    start = clock();
    dynamicProgramming(items, n, capacity);
    end = clock();
    cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("��̬�滮��ִ��ʱ��: %.2f ms\n\n", cpu_time_used2);

    // ִ��̰�ķ�
    start = clock();
    greedy(items, n, capacity);
    end = clock();
    cpu_time_used3 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("̰�ķ�ִ��ʱ��: %.2f ms\n\n", cpu_time_used3);

    // ִ�л��ݷ�
    start = clock();
    backtracking(items, n, capacity);
    end = clock();
    cpu_time_used4 = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("���ݷ�ִ��ʱ��: %.2f ms\n\n\n\n", cpu_time_used4);

    printf("------------ִ��ʱ�����-------------\n");
    printf("������ִ��ʱ��: %.2f ms\n\n", cpu_time_used1);
    printf("��̬�滮��ִ��ʱ��: %.2f ms\n\n", cpu_time_used2);
    printf("̰�ķ�ִ��ʱ��: %.2f ms\n\n", cpu_time_used3);
    printf("���ݷ�ִ��ʱ��: %.2f ms\n\n", cpu_time_used4);

    return 0;
}
