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

// Lisp Value
typedef struct
{
	int type;
	long num;
	// Error and Symbol type have some string data
	char* err;
	char* sym;
	// Pointer to Pointers to lval, and their count
	int count;
	struct lval** cell;
} lval;

// Create Enum of possible lval values
enum
{
	LVAL_NUM,
	LVAL_ERR,
	LVAL_SYM,
	LVAL_SEXPR
};

// Create Enum of possible error types
enum lval_err_types
{
	LERR_DIV_ZERO,
	LERR_BAD_OP,
	LERR_BAD_NUM
};

// contruct a pointer to a new Number lval
lval* lval_num(long x)
{
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = x;
	return v;
}

// Construct a pointer to a new Error lval
lval* lval_err(char* m)
{
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_ERR;
	v->err = malloc(strlen(m) + 1);
	strcpy(v->err, m);
	return v;
}

// Construct a pointer to a new Symbol lval
lval* lval_sym(char* s)
{
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->sym = malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

// Construct a pointer to a new empty Sexpr lval
lval* lval_sexpr(void)
{
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->count = 0;
	v->cell = NULL;
	return v;
}

// Delete lval
void lval_del(lval* v)
{
	switch(v->type) {
		// No need to do anything for the Number lval as `cell` and other pointers will be empty
		case LVAL_NUM:
			break;

		case LVAL_ERR:
			free(v->err);
			break;

		case LVAL_SYM:
			free(v->sym);
			break;

		// if Sexpr, then delete all elements inside
		case LVAL_SEXPR:
			for (int i = 0; i < v->count; i++) {
				lval_del(v->cell[i]);
			}
			// Also free the memory allocated to contain the pointers
			free(v->cell);

		break;
	}

	// finally, free the memory allocated for the "lval" struct itself
	free(v);
}


// Print an lval
void lval_print(lval v)
{
	switch (v.type)
	{
	case LVAL_NUM:
		printf("%li", v.num);
		break;

	case LVAL_ERR:
		if (v.err == LERR_DIV_ZERO)
		{
			printf("Error: Division by zero");
		}
		if (v.err == LERR_BAD_OP)
		{
			printf("Error: Invalid Operation");
		}
		if (v.err == LERR_BAD_NUM)
		{
			printf("Error: Invalid Number");
		}
		break;
	}
}

// Print an lval followed by a newline
void lval_println(lval v)
{
	lval_print(v);
	putchar('\n');
}

lval* lval_read_num(mpc_ast_t* t) {
	errno = 0;
	long x = strtol(t->contents, NULL, 10);
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
	// if Symbol or number return conversion to that type
	if (strstr(t->tag, "number")) {
		return lval_read_num(t);
	}

	if (strstr(t->tag, "symbol")) {
		return lval_sym(t->contents);
	}

	// if root or sexpr, then create empty list
	lval* x = NULL;

	if (strcmp(t->tag, ">") == 0) {
		x = lval_sexpr();
	}

	if (strstr(t->tag, "sexpr")) {
		x = lval_sexpr();
	}

	// fill the list above with any valid expression contained within
	for (int i = 0; i < t->children_num; i++) {
		if (strcmp(t->children[i]->contents, "(" ) == 0) {
			continue;
		}
		if (strcmp(t->children[i]->contents, ")" == 0)) {
			continue;
		}

		if (strcmp(t->children[i]->tag, "regex") == 0) {
			continue;
		}

		x = lval_add(x, lval_read(t->children[i]));
	}

	return x;
}

lval* lval_add(lval* v, lval* x) {
	v->count++;
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);
	v->cell[v->count - 1] = x;
	return v;
}


lval eval_op(lval x, char *op, lval y)
{
	// If either value is an error, return it
	if (x.type == LVAL_ERR)
	{
		return x;
	}
	if (x.type == LVAL_ERR)
	{
		return y;
	}

	// strcmp returns 0 if strings are equal, else non-zero
	if (strcmp(op, "+") == 0)
	{
		return lval_num(x.num + y.num);
	}
	if (strcmp(op, "*") == 0)
	{
		return lval_num(x.num * y.num);
	}
	if (strcmp(op, "-") == 0)
	{
		return lval_num(x.num - y.num);
	}
	if (strcmp(op, "/") == 0)
	{
		return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
	}
	if (strcmp(op, "%") == 0)
	{
		return lval_num(x.num % y.num);
	}
	if (strcmp(op, "^") == 0)
	{
		return lval_num(pow(x.num, y.num));
	}

	return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *t)
{
	// if tagged as number return it immediately
	if (strstr(t->tag, "number"))
	{
		// Check if there is any error in conversion
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}

	// if given the input "- 3", returns 3
	// if (strcmp(t->children[1]->contents, "-"))
	// {
	// 	lval rest = eval(t->children[2]);
	// 	return lval_num(-1 * rest.num);
	// }

	// the operator is always the second child
	char *op = t->children[1]->contents;

	// we store the third child in x
	lval x = eval(t->children[2]);

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
	mpc_parser_t *Symbol = mpc_new("symbol");
	mpc_parser_t *Sexpr = mpc_new("sexpr");
	mpc_parser_t *Expr = mpc_new("expr");
	mpc_parser_t *Lispy = mpc_new("lispy");

	// Define the parers with the following language

	mpca_lang(MPCA_LANG_DEFAULT,
		"                                                             \
			number : /-?[0-9]+[.]?[0-9]*/ ;                             \
			symbol : '+' | '*' | '-' | '/' | '\%' | '^' ;               \
			sexpr : '(' <expr>* ')'   ;                                 \
			expr : <number> | <symbol> | <sexpr> ; \
			lispy : /^/ <expr>* /$/ ;                        \
		", Number, Symbol, Sexpr, Expr, Lispy);

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

			lval result = eval(r.output);
			lval_println(result);

			mpc_ast_delete(r.output);
		}
		else
		{
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		free(input);
	}

	mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);

	return 0;
}
