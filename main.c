#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
    char data;
    struct Node* children[2];
    int childCount;
} Node;

Node* createNode(char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    newNode->data = data;
    newNode->children[0] = NULL;
    newNode->children[1] = NULL;
    newNode->childCount = 0;
    return newNode;
}

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->children[0]);
    freeTree(root->children[1]);
    free(root);
}

Node* buildTree(const char* input, int* index, int* error) {
    if (*error) return NULL;

    Node* root = NULL;
    int len = strlen(input);

    while (*index < len) {
        char c = input[*index];

        if (c == '(') {
            (*index)++;
            if (*index >= len || !isupper(input[*index])) {
                *error = 1;
                return NULL;
            }
            char nodeData = input[*index];
            root = createNode(nodeData);

            (*index)++;
            while (*index < len && input[*index] == ' ') {
                (*index)++;
            }

            if (*index < len && input[*index] == ')') {
                return root;
            }

            while (*index < len && input[*index] == '(') {
                if (root->childCount >= 2) {
                    *error = 1;
                    return NULL;
                }
                root->children[root->childCount++] = buildTree(input, index, error);
                if (*error) return NULL;
                while (*index < len && input[*index] == ' ') { 
                    (*index)++;
                }
            }

            if (*index < len && input[*index] == ')') { 
                (*index)++;
                return root;
            }
            else {
                *error = 1; 
                return NULL;
            }

        }
        else if (c == ')' || c == ' ') {
            (*index)++;
        }
        else {
            *error = 1;
            return NULL;
        }
    }
    return root;
}

int isBinaryTree(Node* root) {
    if (root == NULL) {
        return 1; 
    }
    if (root->childCount > 2) {
        return 0;
    }
    for (int i = 0; i < root->childCount; i++) {
        if (!isBinaryTree(root->children[i])) {
            return 0;
        }
    }
    return 1;
}

int main() {
    char input[256];
    printf("광호 형태 트리를 입력하세요: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    int index = 0;
    int error = 0;

    Node* root = buildTree(input, &index, &error);

    if (error) {
        printf("ERROR\n");
    }
    else {
        if (isBinaryTree(root)) {
            printf("TRUE\n");
        }
        else {
            printf("FALSE\n");
        }
    }

    freeTree(root);
    return 0;
}