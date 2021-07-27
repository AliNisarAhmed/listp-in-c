#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// fake readline function
char *readline(char *prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}

/* Fake add_history function */
void add_history(char *unused) {}

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

long eval_op(long x, char *op, long y)
{
	// strcmp returns 0 if strings are equal, else non-zero
	if (strcmp(op, "+") == 0)
	{
		return x + y;
	}
	if (strcmp(op, "*") == 0)
	{
		return x * y;
	}
	if (strcmp(op, "-") == 0)
	{
		return x - y;
	}
	if (strcmp(op, "/") == 0)
	{
		return x / y;
	}
	if (strcmp(op, "%") == 0)
	{
		return x % y;
	}
	if (strcmp(op, "^") == 0)
	{
		return pow(x, y);
	}
	return 0;
}

long eval(mpc_ast_t *t)
{
	// if tagged as number return it immediately
	if (strstr(t->tag, "number"))
	{
		return atoi(t->contents);
	}

	// if given the input "- 3", returns 3
	if (strcmp(t->children[1]->contents, "-"))
	{
		long rest = eval(t->children[2]);
		return -1 * rest;
	}

	// the operator is always the second child
	char *op = t->children[1]->contents;

	// we store the third child in x
	long x = eval(t->children[2]);

	// iterate on the remaining children and combining
	int i = 3;

	while (strstr(t->children[i]->tag, "expr"))
	{
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}
int number_of_nodes(mpc_ast_t *t)
{
	if (t->children_num == 0)
	{
		return 1;
	}
	if (t->children_num >= 1)
	{
		int total = 1;
		for (int i = 0; i < t->children_num; i++)
		{
			total = total + number_of_nodes(t->children[i]);
		}
		return total;
	}
	return 0;
}

int count_leaves(mpc_ast_t *t)
{
	if (t->children_num == 0)
	{
		return 1;
	}
	if (t->children_num > 0)
	{
		int total = 0;
		for (int j = 0; j < t->children_num; j++)
		{
			total = total + count_leaves(t->children[j]);
		}
		return total;
	}
	return 0;
}

int max(int a1, int a2)
{
	if (a1 > a2)
	{
		return a1;
	}

	return a2;
}

int longest_branch_acc(mpc_ast_t *t, int acc)
{
	if (t->children_num == 0)
	{
		return acc;
	}
	if (t->children_num > 0)
	{
		int maxTillNow = 1;
		for (int i = 0; i < t->children_num; i++)
		{
			maxTillNow = max(maxTillNow, longest_branch_acc(t->children[i], acc + 1));
		}
		return maxTillNow;
	}

	return 0;
}

int longest_branch(mpc_ast_t *t)
{
	return longest_branch_acc(t, 0);
}

int main(int argc, char **argv)
{

	// Create some parsers

	mpc_parser_t *Number = mpc_new("number");
	mpc_parser_t *Operator = mpc_new("operator");
	mpc_parser_t *Expr = mpc_new("expr");
	mpc_parser_t *Lispy = mpc_new("lispy");

	// Define the parers with the following language

	mpca_lang(MPCA_LANG_DEFAULT,
						"                                                \
			number : /-?[0-9]+[.]?[0-9]*/ ;                          \
			operator : '+' | '*' | '-' | '/' | '\%' | '^' |\"add\" | \"sub\" | \"mul\" | \"div\" ;       \
			expr : <number> | '(' <operator> <expr>+ ')' | '-' <expr> ; \
			lispy : /^/ <operator> <expr>+ /$/ ;           \
		",
						Number, Operator, Expr, Lispy);

	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");

	while (1)
	{
		char *input = readline("lispy> ");

		add_history(input);

		// Attempt to parse the user input
		mpc_result_t r;

		if (mpc_parse("<stdin>", input, Lispy, &r))
		{
			mpc_ast_print(r.output);

			int leaves = count_leaves(r.output);
			printf("Number of leaves: %i\n", leaves);

			int branches = longest_branch(r.output);
			printf("Longest branch: %i\n", branches);

			long result = eval(r.output);
			printf("%li\n", result);

			mpc_ast_delete(r.output);
		}
		else
		{
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		free(input);
	}

	mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}
