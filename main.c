#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 100
#define RANGE_MAX 1000

typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        exit(1);
    }
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* insert(Node* root, int key) {
    if (root == NULL) {
        return createNode(key);
    }
    if (key < root->key) {
        root->left = insert(root->left, key);
    }
    else if (key > root->key) {
        root->right = insert(root->right, key);
    }
    return root;
}

int linearSearch(int arr[], int size, int target, int* comparisons) {
    *comparisons = 0;
    for (int i = 0; i < size; i++) {
        (*comparisons)++;
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

Node* bstSearch(Node* root, int target, int* comparisons) {
    *comparisons = 0;
    Node* current = root;
    while (current != NULL) {
        (*comparisons)++;
        if (target == current->key) {
            return current;
        }
        else if (target < current->key) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return NULL;
}

int main() {
    int arr[ARRAY_SIZE];
    Node* bstRoot = NULL;
    int targetValue;

    srand(time(NULL));
    printf("100개의 무작위 숫자 생성: \n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % (RANGE_MAX + 1);
        printf("%d ", arr[i]);
        bstRoot = insert(bstRoot, arr[i]);
    }
    printf("\n\n");

    targetValue = arr[49];
    printf("임의로 선택한 탐색 대상 숫자: %d\n\n", targetValue);

    clock_t start_linear = clock();
    int linearComparisons = 0;
    int linearResult = linearSearch(arr, ARRAY_SIZE, targetValue, &linearComparisons);
    clock_t end_linear = clock();
    double time_linear = (double)(end_linear - start_linear) / CLOCKS_PER_SEC * 1000;

    clock_t start_bst = clock();
    int bstComparisons = 0;
    Node* bstResult = bstSearch(bstRoot, targetValue, &bstComparisons);
    clock_t end_bst = clock();
    double time_bst = (double)(end_bst - start_bst) / CLOCKS_PER_SEC * 1000;

    printf("--- 선형 탐색 결과 ---\n");
    if (linearResult != -1) {
        printf("숫자 %d를 배열에서 찾았습니다.\n", targetValue);
        printf("탐색 위치: %d\n", linearResult);
    }
    else {
        printf("숫자 %d를 배열에서 찾지 못했습니다.\n", targetValue);
    }
    printf("비교 횟수: %d회\n", linearComparisons);
    printf("소요 시간: %.6fms\n\n", time_linear);

    printf("--- 이진 탐색 트리 결과 ---\n");
    if (bstResult != NULL) {
        printf("숫자 %d를 이진 탐색 트리에서 찾았습니다.\n", targetValue);
    }
    else {
        printf("숫자 %d를 이진 탐색 트리에서 찾지 못했습니다.\n", targetValue);
    }
    printf("비교 횟수: %d회\n", bstComparisons);
    printf("소요 시간: %.6fms\n", time_bst);

    return 0;
}