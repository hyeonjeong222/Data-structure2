#include <stdio.h>
#include <string.h>
#include <ctype.h>

char input_str[256];
int pos = 0;
int total_nodes = 0;
int leaf_nodes = 0;

int parse_tree();

int parse_tree() {
    if (pos >= strlen(input_str) || input_str[pos] != '(') {
        return -1;
    }

    pos++;

    if (!isalpha(input_str[pos])) {
        if (input_str[pos] == ')') {
            pos++;
            return -1;
        }
        return -1;
    }

    pos++;
    total_nodes++;

    int max_child_height = -1;
    int child_count = 0;

    while (input_str[pos] != ')') {
        if (isalpha(input_str[pos])) {
            total_nodes++;
            child_count++;
            pos++;

            if (input_str[pos] == '(') {
                int child_height = parse_tree();
                if (child_height > max_child_height) {
                    max_child_height = child_height;
                }
            }
            else {
                if (-1 > max_child_height) {
                    max_child_height = -1;
                }
            }
        }
        else if (input_str[pos] == '(') {
            int child_height = parse_tree();
            if (child_height > max_child_height) {
                max_child_height = child_height;
            }
            child_count++;
        }
        else {
            return -1;
        }
    }

    if (input_str[pos] != ')') {
        return -1;
    }
    pos++;

    if (child_count == 0) {
        leaf_nodes++;
        return 0;
    }

    return max_child_height + 1;
}

int main() {
    printf("공백이 없는 괄호 형식의 트리를 입력하세요 (예: (A(B(CD)E(GH)))): \n");
    scanf("%s", input_str);

    printf("\nInput: %s\n", input_str);

    total_nodes = 0;
    leaf_nodes = 0;
    pos = 0;

    int root_height = parse_tree();

    if (root_height == -1 || pos != strlen(input_str)) {
        printf("Output: ERROR\n");
    }
    else {
        printf("Output: %d, %d, %d\n", root_height, total_nodes, leaf_nodes);
    }

    return 0;
}
