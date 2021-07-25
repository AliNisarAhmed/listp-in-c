#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__

#include <editline/readline.h>
#include <editline/history.h>

#else

#include <string.h>

static char buffer[2048];

// defining a fake readline for windows
char *readline(char *prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}

// fake add history for windows
void add_history(char *unused) {}

#endif

int main(int argc, char **argv)
{
	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while (1)
	{
		char *input = readline("lispy> ");

		add_history(input);

		printf("Back at ya: %s\n", input);

		free(input);
	}

	return 0;
}