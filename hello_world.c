#include <stdio.h>

int main(int argc, char** argv) {
	for (int i = 0; i < 5; i++) {
		puts("Hello World");
	}

	puts("______\n");

	int i = 0;
	while (i < 5) {
		puts("Hello While");
		i++;
	}

	puts("______\n");

	print_hello_world_n(5);

	return 0;
}

int add_together(int x, int y) {
	return x + y;
}

typedef struct {
	float x;
	float y;
} point;

int print_hello_world_n(int n) {
	for (int i = 0; i < n; i++) {
		puts("Hello World from function");
	}
	return 0;
}