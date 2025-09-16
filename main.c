// (A (B (D E) C (F G)))
// (A(B(DC)C(FG)))
// 괄호 갯수 검사
// (AB(DE)) -> 루트 개수 이상

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

		}
	}
}

int check_paren(char* buf, int size) {

}

int check_root(char* buf, int size) {

}

int main() {
	char buf[1000];
	fgets(buf, sizeof(buf), stdin);
	printf("%s", buf);

	trim(buf);
	printf("%s")

	if (check_paren(buf, strlen(buf)) == 0 || check_root(buf, strlen(buf) == 0) {
		printf("ERROR");
		return 0;
	}

	int result = check_binary_tree(buf);

	if(result = TRUE)
}