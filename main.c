#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// ========== 상수 정의 ==========
#define V 10            // 정점의 개수
#define E 20            // 간선의 개수
#define MAX_WEIGHT 10   // 최대 무작위 가중치
#define INF INT_MAX     // 무한대 값 (INT_MAX 사용)

// ========== 전역 변수 및 함수 원형 ==========
int graph[V][V]; // 인접 행렬 (가중치 저장)

void initGraph();
void generateRandomGraph();
void dijkstra(int startNode);
int minDistance(int dist[], int visited[]);

// ========== 헬퍼 함수 정의 ==========

// 그래프 초기화 (가중치 0 또는 INF)
void initGraph() {
    int i, j;
    for (i = 0; i < V; i++) {
        for (j = 0; j < V; j++) {
            if (i == j) {
                graph[i][j] = 0; // 자기 자신으로의 거리는 0
            }
            else {
                graph[i][j] = 0; // 초기에는 간선 없음 (0으로 표시)
            }
        }
    }
}

// 무작위 그래프 생성
void generateRandomGraph() {
    int i;
    int u, v, weight;
    int edges_added = 0;

    printf("--- 생성된 무작위 간선 (출발 -> 도착 : 가중치) ---\n");

    while (edges_added < E) {
        // 무작위 정점 및 가중치 선택
        u = rand() % V;
        v = rand() % V;
        weight = (rand() % MAX_WEIGHT) + 1; // 1~10 사이의 가중치

        // 자기 루프 방지 및 이미 간선이 존재하는지 확인 (더 작은 가중치로 갱신)
        if (u != v) {
            if (graph[u][v] == 0 || weight < graph[u][v]) {
                graph[u][v] = weight;
                edges_added++;
                printf("(%d -> %d : %d)", u, v, weight);
                if (edges_added % 5 == 0) printf("\n"); else printf(" | ");
            }
        }
    }
    printf("\n------------------------------------------------------\n");
}

// 방문하지 않은 정점 중 최소 거리를 가진 정점을 찾는 함수
int minDistance(int dist[], int visited[]) {
    int min = INF, min_index = -1;
    int v;

    for (v = 0; v < V; v++) {
        if (visited[v] == 0 && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// ========== 다익스트라 알고리즘 구현 ==========

void dijkstra(int startNode) {
    int dist[V];     // 시작 노드로부터의 최단 거리 저장
    int visited[V];  // 정점 방문 여부
    int parent[V];   // 최단 경로 상의 이전 노드 저장
    int i, u, v;

    // 초기화
    for (i = 0; i < V; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1; // 경로 추적을 위해 부모 노드 초기화
    }

    dist[startNode] = 0;

    // V-1번 반복하여 모든 정점을 처리
    for (i = 0; i < V - 1; i++) {
        // 1. 현재 최단 거리를 가진 정점 u 선택
        u = minDistance(dist, visited);

        if (u == -1 || dist[u] == INF) break; // 도달할 수 없는 경우 종료

        visited[u] = 1;

        // 2. 인접 정점 v의 거리 갱신
        for (v = 0; v < V; v++) {
            // u에서 v로의 간선이 존재하고 (graph[u][v] > 0),
            // v가 아직 방문되지 않았으며,
            // u를 거쳐 v로 가는 경로가 현재까지의 dist[v]보다 짧다면 갱신
            if (!visited[v] && graph[u][v] > 0 && dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u; // 경로 추적
            }
        }
    }

    // 결과 출력
    printf("\n[시작 노드 %d로부터의 최단 경로]\n", startNode);
    printf("------------------------------------------------------\n");
    printf("| 도착 노드 | 최단 거리 | 경로 |\n");
    printf("------------------------------------------------------\n");
    for (i = 0; i < V; i++) {
        if (i != startNode) {
            printf("| %-9d | %-9s | ", i, (dist[i] == INF ? "INF" : ""));
            if (dist[i] != INF) {
                printf("%-4d", startNode); // 시작 노드 출력
                int current = i;
                int path_stack[V], top = 0;

                // 경로 역추적 및 스택에 저장
                while (parent[current] != startNode && current != -1) {
                    path_stack[top++] = parent[current];
                    current = parent[current];
                }

                // 스택에 저장된 중간 경로 출력
                while (top > 0) {
                    printf(" -> %-4d", path_stack[--top]);
                }

                printf(" -> %-4d", i); // 도착 노드 출력
                printf(" (%d)", dist[i]); // 최단 거리 출력
            }
            else {
                printf("도달 불가");
            }
            printf(" |\n");
        }
    }
    printf("------------------------------------------------------\n");
}


// ========== 메인 함수 ==========

int main() {
    // 난수 초기화 및 시드 설정
    srand((unsigned int)time(NULL));

    // 1. 그래프 초기화 및 생성
    initGraph();
    generateRandomGraph();

    // 2. 모든 노드를 시작점으로 다익스트라 실행 (10회)
    printf("\n======================================================\n");
    printf("             모든 노드 쌍 간의 최단 경로\n");
    printf("======================================================\n");

    int start;
    for (start = 0; start < V; start++) {
        dijkstra(start);
    }

    return 0;
}