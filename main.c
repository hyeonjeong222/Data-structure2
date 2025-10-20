#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ========== 상수 정의 ==========
#define DATA_SIZE 1000      // 데이터 개수
#define SEARCH_COUNT 1000   // 탐색 횟수
#define MAX_VALUE 10000     // 데이터 최대 값 (0~10000)

// ========== BST/AVL 노드 구조체 정의 ==========
typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
    int height;
} Node;

// ========== 함수 원형 (Prototype) 선언 (E0040, C2059 오류가 발생하면 이 블록의 세미콜론을 확인) ==========
int height(Node* N);
int max(int a, int b);
Node* newNode(int key);
int getBalance(Node* N);
Node* rightRotate(Node* y);
Node* leftRotate(Node* x);
void freeTree(Node* root); // E0147 오류 해결

void arrayInsert(int arr[], int* size, int key);
Node* bstInsert(Node* node, int key);
Node* avlInsert(Node* node, int key);

int arrayLinearSearch(const int arr[], int size, int key, int* steps);
Node* bstSearch(Node* root, int key, int* steps);
Node* avlSearch(Node* root, int key, int* steps);

void generateRandomData(int data[]);
void generateSorted0To999(int data[]);
void generateSorted999To0(int data[]);
void generateFormulaData(int data[]);
void generateSearchKeys(int keys[]);
void runTest(int data[], const char* dataType);


// =================================================================
// ----------------- BST/AVL 헬퍼 함수 정의 (E0169 오류 해결) -----------------
// =================================================================

int height(Node* N) {
    if (N == NULL)
        return 0;
    return N->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

Node* newNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        perror("메모리 할당 실패");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

int getBalance(Node* N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

void freeTree(Node* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}


// =================================================================
// -------------------- 데이터 삽입 및 탐색 함수 정의 ---------------------
// =================================================================

void arrayInsert(int arr[], int* size, int key) {
    if (*size < DATA_SIZE) {
        arr[*size] = key;
        (*size)++;
    }
}

Node* bstInsert(Node* node, int key) {
    if (node == NULL)
        return newNode(key);
    if (key < node->key)
        node->left = bstInsert(node->left, key);
    else if (key > node->key)
        node->right = bstInsert(node->right, key);
    return node;
}

Node* avlInsert(Node* node, int key) {
    if (node == NULL)
        return newNode(key);

    if (key < node->key)
        node->left = avlInsert(node->left, key);
    else if (key > node->key)
        node->right = avlInsert(node->right, key);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // LL Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    // RR Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    // LR Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // RL Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

int arrayLinearSearch(const int arr[], int size, int key, int* steps) {
    for (int i = 0; i < size; i++) {
        (*steps)++;
        if (arr[i] == key) {
            return 1;
        }
    }
    return 0;
}

Node* bstSearch(Node* root, int key, int* steps) {
    if (root == NULL || root->key == key) {
        if (root != NULL) (*steps)++;
        return root;
    }
    (*steps)++;
    if (key < root->key) {
        return bstSearch(root->left, key, steps);
    }
    else {
        return bstSearch(root->right, key, steps);
    }
}

Node* avlSearch(Node* root, int key, int* steps) {
    return bstSearch(root, key, steps);
}


// =================================================================
// -------------------- 데이터 생성 함수 정의 ---------------------------
// =================================================================

void generateRandomData(int data[]) {
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % (MAX_VALUE + 1);
    }
}

void generateSorted0To999(int data[]) {
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i;
    }
}

void generateSorted999To0(int data[]) {
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = DATA_SIZE - 1 - i;
    }
}

void generateFormulaData(int data[]) {
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i * (i % 2 + 2);
    }
}

void generateSearchKeys(int keys[]) {
    for (int i = 0; i < SEARCH_COUNT; i++) {
        keys[i] = rand() % (MAX_VALUE + 1);
    }
}


// =================================================================
// -------------------- 메인 테스트 실행 로직 정의 ------------------------
// =================================================================

void runTest(int data[], const char* dataType) {
    int i;
    int array[DATA_SIZE];
    int array_size = 0;
    Node* bstRoot = NULL;
    Node* avlRoot = NULL;

    printf("--- 데이터 유형: %s ---\n", dataType);

    // 1. 데이터 삽입
    for (i = 0; i < DATA_SIZE; i++) {
        arrayInsert(array, &array_size, data[i]);
        bstRoot = bstInsert(bstRoot, data[i]);
        avlRoot = avlInsert(avlRoot, data[i]);
    }

    // 2. 탐색 키 생성
    int searchKeys[SEARCH_COUNT];
    generateSearchKeys(searchKeys);

    // 3. 탐색 및 평균 횟수 계산 (E0079, E0018, E0757 오류 해결)
    double totalArraySteps = 0.0;
    double totalBSTSteps = 0.0;
    double totalAVLSteps = 0.0;
    int steps;

    for (i = 0; i < SEARCH_COUNT; i++) {
        int key = searchKeys[i];

        // 배열 선형 탐색
        steps = 0;
        arrayLinearSearch(array, array_size, key, &steps);
        totalArraySteps += steps;

        // BST 탐색
        steps = 0;
        bstSearch(bstRoot, key, &steps);
        totalBSTSteps += steps;

        // AVL 탐색
        steps = 0;
        avlSearch(avlRoot, key, &steps);
        totalAVLSteps += steps;
    }

    // 4. 결과 출력
    printf("Array: 평균 탐색 횟수: %.2f\n", totalArraySteps / SEARCH_COUNT);
    printf("BST: 평균 탐색 횟수: %.2f\n", totalBSTSteps / SEARCH_COUNT);
    printf("AVL: 평균 탐색 횟수: %.2f\n", totalAVLSteps / SEARCH_COUNT);
    printf("\n");

    // 5. 메모리 해제
    freeTree(bstRoot);
    freeTree(avlRoot);
}

// ========== 메인 함수 ==========
int main() {
    // C4244 경고 해결 및 난수 초기화
    srand((unsigned int)time(NULL));

    int data[DATA_SIZE];

    // 1. 랜덤 데이터 테스트
    generateRandomData(data);
    runTest(data, "1. 무작위 정수 (0~10000)");

    // 2. 오름차순 데이터 테스트
    generateSorted0To999(data);
    runTest(data, "2. 오름차순 정렬 (0~999)");

    // 3. 내림차순 데이터 테스트
    generateSorted999To0(data);
    runTest(data, "3. 내림차순 정렬 (999~0)");

    // 4. 공식 기반 데이터 테스트
    generateFormulaData(data);
    runTest(data, "4. 공식 기반 (i * (i % 2 + 2))");

    return 0;
}