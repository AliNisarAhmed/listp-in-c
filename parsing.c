#include "mpc.h"

// Create some parsers

mpc_parser_t *Number = mpc_new("number");
mpc_parser_t *Operator = mpc_new("operator");
mpc_parser_t *Expr = mpc_new("expr");
mpc_parser_t *Lispy = mpc_new("lispy");

// Define the parers with the following language

mpca_lang(MPCA_LANG_DEFAULT,
	"                                                \
		number : /-?[0-9]+/ ;                          \
		operator : '+' | '*' | '-' | '/' ;             \
		expr : <number> | '(' <operator> <expr>+ ')' ; \
		lispy : /^/ <operator> <expr>+ /$/ ;           \
	",
	Number, Operator, Expr, Lispy);