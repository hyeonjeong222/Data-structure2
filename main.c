#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define VERTICES 100
#define SPARSE_EDGES 100
#define DENSE_EDGES 4000
#define TEST_COUNT 100

typedef struct EdgeNode {
    int dest;
    struct EdgeNode* next;
} EdgeNode;

typedef struct GraphAdjList {
    EdgeNode* head[VERTICES];
    int num_edges;
} GraphAdjList;

typedef struct GraphAdjMatrix {
    int matrix[VERTICES][VERTICES];
    int num_edges;
} GraphAdjMatrix;

void initAdjList(GraphAdjList* graph) {
    int i;
    for (i = 0; i < VERTICES; i++) {
        graph->head[i] = NULL;
    }
    graph->num_edges = 0;
}

void initAdjMatrix(GraphAdjMatrix* graph) {
    int i, j;
    for (i = 0; i < VERTICES; i++) {
        for (j = 0; j < VERTICES; j++) {
            graph->matrix[i][j] = 0;
        }
    }
    graph->num_edges = 0;
}

int insertEdgeList(GraphAdjList* graph, int u, int v, long long* comparisons) {
    EdgeNode* current = graph->head[u];
    while (current != NULL) {
        (*comparisons)++;
        if (current->dest == v) {
            return 0;
        }
        current = current->next;
    }

    EdgeNode* newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (newNode == NULL) return -1;
    newNode->dest = v;
    newNode->next = graph->head[u];
    graph->head[u] = newNode;
    graph->num_edges++;

    return 1;
}

int deleteEdgeList(GraphAdjList* graph, int u, int v, long long* comparisons) {
    EdgeNode* current = graph->head[u];
    EdgeNode* prev = NULL;

    while (current != NULL) {
        (*comparisons)++;
        if (current->dest == v) {
            if (prev == NULL) {
                graph->head[u] = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            graph->num_edges--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

int manageEdgeMatrix(GraphAdjMatrix* graph, int u, int v, int action, long long* comparisons) {
    (*comparisons)++;

    if (action == 1) {
        if (graph->matrix[u][v] == 0) {
            graph->matrix[u][v] = 1;
            graph->num_edges++;
            return 1;
        }
    }
    else {
        if (graph->matrix[u][v] == 1) {
            graph->matrix[u][v] = 0;
            graph->num_edges--;
            return 1;
        }
    }
    return 0;
}

int isConnectedList(GraphAdjList* graph, int u, int v, long long* comparisons) {
    EdgeNode* current = graph->head[u];
    while (current != NULL) {
        (*comparisons)++;
        if (current->dest == v) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int isConnectedMatrix(GraphAdjMatrix* graph, int u, int v, long long* comparisons) {
    (*comparisons)++;
    return graph->matrix[u][v] == 1;
}

int printNeighborsList(GraphAdjList* graph, int u, long long* comparisons) {
    EdgeNode* current = graph->head[u];
    int count = 0;
    while (current != NULL) {
        (*comparisons)++;
        count++;
        current = current->next;
    }
    return count;
}

int printNeighborsMatrix(GraphAdjMatrix* graph, int u, long long* comparisons) {
    int v, count = 0;
    for (v = 0; v < VERTICES; v++) {
        (*comparisons)++;
        if (graph->matrix[u][v] == 1) {
            count++;
        }
    }
    return count;
}

void generateRandomGraph(int num_edges, GraphAdjList* list, GraphAdjMatrix* matrix) {
    int u, v;
    initAdjList(list);
    initAdjMatrix(matrix);

    while (list->num_edges < num_edges) {
        u = rand() % VERTICES;
        v = rand() % VERTICES;

        if (u == v) continue;

        long long temp_comp = 0;
        insertEdgeList(list, u, v, &temp_comp);
        manageEdgeMatrix(matrix, u, v, 1, &temp_comp);
    }
}

void runTest(int num_edges, const char* density) {
    GraphAdjList list;
    GraphAdjMatrix matrix;

    generateRandomGraph(num_edges, &list, &matrix);

    size_t list_mem = sizeof(list) + (list.num_edges * sizeof(EdgeNode));
    size_t matrix_mem = sizeof(matrix);

    long long list_insert_delete_comp = 0;
    long long matrix_insert_delete_comp = 0;
    int i;
    int u_test, v_test;

    for (i = 0; i < TEST_COUNT; i++) {
        u_test = rand() % VERTICES;
        v_test = rand() % VERTICES;

        insertEdgeList(&list, u_test, v_test, &list_insert_delete_comp);
        deleteEdgeList(&list, u_test, v_test, &list_insert_delete_comp);

        manageEdgeMatrix(&matrix, u_test, v_test, 1, &matrix_insert_delete_comp);
        manageEdgeMatrix(&matrix, u_test, v_test, 0, &matrix_insert_delete_comp);
    }

    long long list_check_comp = 0;
    long long matrix_check_comp = 0;

    for (i = 0; i < TEST_COUNT; i++) {
        u_test = rand() % VERTICES;
        v_test = rand() % VERTICES;
        isConnectedList(&list, u_test, v_test, &list_check_comp);
        isConnectedMatrix(&matrix, u_test, v_test, &matrix_check_comp);
    }

    long long list_neighbor_comp = 0;
    long long matrix_neighbor_comp = 0;

    for (i = 0; i < VERTICES; i++) {
        printNeighborsList(&list, i, &list_neighbor_comp);
        printNeighborsMatrix(&matrix, i, &matrix_neighbor_comp);
    }

    printf("\n======================================================\n");
    printf("그래프 유형: %s (N=%d, E=%d)\n", density, VERTICES, num_edges);
    printf("======================================================\n");

    printf("1. 인접 리스트\n");
    printf("메모리: %zu Bytes\n", list_mem);
    printf("간선 삽입/삭제 평균 비교: %.2f 번\n", (double)list_insert_delete_comp / (TEST_COUNT * 2));
    printf("두 정점 연결 확인 평균 비교: %.2f 번\n", (double)list_check_comp / TEST_COUNT);
    printf("한 노드의 인접 노드 탐색 평균 비교: %.2f 번\n", (double)list_neighbor_comp / VERTICES);
    printf("------------------------------------------------------\n");

    printf("2. 인접 행렬\n");
    printf("메모리: %zu Bytes\n", matrix_mem);
    printf("간선 삽입/삭제 평균 비교: %.2f 번\n", (double)matrix_insert_delete_comp / (TEST_COUNT * 2));
    printf("두 정점 연결 확인 평균 비교: %.2f 번\n", (double)matrix_check_comp / TEST_COUNT);
    printf("한 노드의 인접 노드 탐색 평균 비교: %.2f 번\n", (double)matrix_neighbor_comp / VERTICES);
    printf("======================================================\n");
}

int main() {
    srand((unsigned int)time(NULL));

    runTest(SPARSE_EDGES, "희소");
    runTest(DENSE_EDGES, "밀집");

    return 0;
}