#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ERROR 0
#define TRUE 1
#define FALSE 2

void trim(char* buf) {
    char* src = buf;
    char* dst = buf;
    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

int check_paren(char* buf, int size) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (buf[i] == '(') count++;
        else if (buf[i] == ')') {
            count--;
            if (count < 0) return 0;
        }
    }
    return (count == 0);
}

int check_root(char* buf, int size) {
    if (size < 2) return 0;
    if (buf[0] != '(' || buf[size - 1] != ')') return 0;
    return 1;
}

int parse_node(const char* s, int* idx, int size, int* not_binary) {
    if (*idx >= size) return 0;

    if (s[*idx] == '(') {
        (*idx)++;
        if (*idx >= size) return 0;

        if (!isalpha((unsigned char)s[*idx])) return 0;
        while (*idx < size && isalpha((unsigned char)s[*idx])) (*idx)++;

        int child_count = 0;
        while (*idx < size && s[*idx] != ')') {
            if (s[*idx] == '(') {
                if (!parse_node(s, idx, size, not_binary)) return 0;
            }
            else if (isalpha((unsigned char)s[*idx])) {
                while (*idx < size && isalpha((unsigned char)s[*idx])) (*idx)++;
            }
            else {
                return 0;
            }
            child_count++;
            if (child_count > 2) *not_binary = 1;
        }

        if (*idx >= size || s[*idx] != ')') return 0;
        (*idx)++;
        return 1;
    }
    return 0;
}

int check_binary_tree(char* buf) {
    int size = strlen(buf);
    int idx = 0;
    int not_binary = 0;

    if (!parse_node(buf, &idx, size, &not_binary)) return ERROR;
    if (idx != size) return ERROR;

    return not_binary ? FALSE : TRUE;
}

int main() {
    char buf[1000];
    if (!fgets(buf, sizeof(buf), stdin)) {
        printf("ERROR");
        return 0;
    }

    trim(buf);
    int size = strlen(buf);

    if (!check_paren(buf, size) || !check_root(buf, size)) {
        printf("ERROR");
        return 0;
    }

    int result = check_binary_tree(buf);
    if (result == ERROR) {
        printf("ERROR");
    }
    else if (result == TRUE) {
        printf("TRUE");
    }
    else {
        printf("FALSE");
    }

    return 0;
}
