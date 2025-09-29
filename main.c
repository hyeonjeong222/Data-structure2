#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 트리 노드와 배열 크기 정의
#define MAX_NODES 100
char tree_array[MAX_NODES];
int node_count = 0;
int max_index = 0;

// 입력 문자열 파싱을 위한 전역 변수
char input_str[256];
int pos = 0;

// 스택 자료구조 정의 (반복적 순회에 사용)
typedef struct {
    int items[MAX_NODES];
    int top;
} Stack;

void init_stack(Stack* s) {
    s->top = -1;
}

int is_empty(Stack* s) {
    return s->top == -1;
}

void push(Stack* s, int value) {
    if (s->top < MAX_NODES - 1) {
        s->items[++s->top] = value;
    }
}

int pop(Stack* s) {
    if (!is_empty(s)) {
        return s->items[s->top--];
    }
    return -1;
}

// 괄호 형식의 트리를 배열로 변환하는 함수
void parse_and_build(int index) {
    if (pos >= strlen(input_str)) {
        return;
    }

    char node_label = input_str[pos];

    if (node_label == '(' || node_label == ')') {
        return;
    }

    tree_array[index] = node_label;
    if (index > max_index) {
        max_index = index;
    }
    node_count++;
    pos++;

    if (pos < strlen(input_str) && input_str[pos] == '(') {
        pos++;
        parse_and_build(index * 2);
        pos++;
    }

    if (pos < strlen(input_str) && input_str[pos] == '(') {
        pos++;
        parse_and_build(index * 2 + 1);
        pos++;
    }
}

// 재귀적 순회 함수들
void pre_order_recursive(int index) {
    if (index > 0 && tree_array[index] != '\0') {
        printf("%c", tree_array[index]);
        pre_order_recursive(index * 2);
        pre_order_recursive(index * 2 + 1);
    }
}

void in_order_recursive(int index) {
    if (index > 0 && tree_array[index] != '\0') {
        in_order_recursive(index * 2);
        printf("%c", tree_array[index]);
        in_order_recursive(index * 2 + 1);
    }
}

void post_order_recursive(int index) {
    if (index > 0 && tree_array[index] != '\0') {
        post_order_recursive(index * 2);
        post_order_recursive(index * 2 + 1);
        printf("%c", tree_array[index]);
    }
}

// 반복적 순회 함수들 (스택 사용)
void pre_order_iterative(int index) {
    Stack s;
    init_stack(&s);
    push(&s, index);

    while (!is_empty(&s)) {
        int current_index = pop(&s);
        if (current_index > 0 && tree_array[current_index] != '\0') {
            printf("%c", tree_array[current_index]);

            if (current_index * 2 + 1 < MAX_NODES && tree_array[current_index * 2 + 1] != '\0') {
                push(&s, current_index * 2 + 1);
            }
            if (current_index * 2 < MAX_NODES && tree_array[current_index * 2] != '\0') {
                push(&s, current_index * 2);
            }
        }
    }
}

void in_order_iterative(int index) {
    Stack s;
    init_stack(&s);
    int current_index = index;

    while (current_index > 0 || !is_empty(&s)) {
        while (current_index > 0 && tree_array[current_index] != '\0') {
            push(&s, current_index);
            current_index *= 2;
        }
        current_index = pop(&s);
        if (current_index != -1) {
            printf("%c", tree_array[current_index]);
            current_index = current_index * 2 + 1;
        }
    }
}

void post_order_iterative(int index) {
    Stack s1, s2;
    init_stack(&s1);
    init_stack(&s2);
    push(&s1, index);

    while (!is_empty(&s1)) {
        int current_index = pop(&s1);
        push(&s2, current_index);

        if (current_index * 2 < MAX_NODES && tree_array[current_index * 2] != '\0') {
            push(&s1, current_index * 2);
        }
        if (current_index * 2 + 1 < MAX_NODES && tree_array[current_index * 2 + 1] != '\0') {
            push(&s1, current_index * 2 + 1);
        }
    }

    while (!is_empty(&s2)) {
        printf("%c", tree_array[pop(&s2)]);
    }
}

int main() {
    printf("괄호 형식의 이진 트리를 입력하세요 (예: (A (B(C)(D)) (E(F)(G))))\n");
    printf("주의: 공백을 포함한 전체 문자열을 입력해도 됩니다.\n");

    // fgets로 공백을 포함한 한 줄 전체를 입력받음
    if (fgets(input_str, sizeof(input_str), stdin) == NULL) {
        printf("Input Error\n");
        return 1;
    }

    // 입력 문자열에서 모든 공백과 줄바꿈 문자 제거
    char clean_input[256];
    int k = 0;
    for (int j = 0; j < strlen(input_str); j++) {
        if (!isspace(input_str[j])) {
            clean_input[k++] = input_str[j];
        }
    }
    clean_input[k] = '\0';
    strcpy(input_str, clean_input);

    printf("\nInput: %s\n", input_str);

    memset(tree_array, '\0', sizeof(tree_array));
    pos = 0;

    if (input_str[0] == '(' && isalpha(input_str[1])) {
        pos = 1;
        parse_and_build(1);
    }
    else {
        printf("Output:\nERROR\n");
        return 1;
    }

    printf("Output:\n");

    printf("pre-order(recursive): ");
    pre_order_recursive(1);
    printf("\n");

    printf("in-order(recursive): ");
    in_order_recursive(1);
    printf("\n");

    printf("post-order(recursive): ");
    post_order_recursive(1);
    printf("\n");

    printf("pre-order(iterative): ");
    pre_order_iterative(1);
    printf("\n");

    printf("in-order(iterative): ");
    in_order_iterative(1);
    printf("\n");

    printf("post-order(iterative): ");
    post_order_iterative(1);
    printf("\n");

    return 0;
}