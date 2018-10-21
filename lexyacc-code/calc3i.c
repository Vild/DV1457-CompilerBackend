#include <stdio.h>
#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

#define _e(fmt, ...) do { fflush(stdout); fprintf(stdout, "\x1b[1;31m" fmt "\x1b[0m", ##__VA_ARGS__); } while (0)


/* These are used to make sure that the push and pop count is not uneven. */
static int pushCounter = 0;

#define PUSHCOUNTER_ADD() do { pushCounter++; } while (0)
#define PUSHCOUNTER_SUB() do { pushCounter--;							\
		if (pushCounter < 0)																	\
			_e("\t\t\t\t/* pushCounter: %d */\n", pushCounter);	\
	} while (0)
#define PUSHCOUNTER_EXPECT(n) do {															\
		if (n != pushCounter) {																			\
			_e("\t\t/* PROBLEM HERE %d != %d */\n", n, pushCounter);	\
			exit(-1);																									\
		}																														\
	} while (0)

int ex(nodeType *p) {
	int lbl1 = 0, lbl2 = 0;
	int before = 0;

	if (!p)
		return 0;

	switch(p->type) {
	case typeCon:
		printf("\tpushq\t$%d\n", p->con.value);
		PUSHCOUNTER_ADD();
		break;
	case typeId:
		printf("\tpushq\t%c\n", p->id.i + 'a');
		PUSHCOUNTER_ADD();
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			printf("L%03d:\n", lbl1 = lbl++);

			/*
			 * Expect that the expression in /p->opr.op[0]/ will push *one* value to
			 * the stack. This value will then be compared, and if it is non-zero the
			 * loop body will be run.
			 */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\ttest\t%rax,%rax\n");
			printf("\tjz\tL%03d\n", lbl2 = lbl++);

			/* Don't allow the loop body to have a stray value on the stack */
			before = pushCounter;
			ex(p->opr.op[1]);
			PUSHCOUNTER_EXPECT(before);

			printf("\tjmp\tL%03d\n", lbl1);
			printf("L%03d:\n", lbl2);
			break;
		case IF:
			/*
			 * Expect that the expression in /p->opr.op[0]/ will push *one* value to
			 * the stack. This value will then be compared, and if it is non-zero the
			 * "if" body will be run, else jump into the "else" body, or jump over the
			 * entire if. Whichever choice is the correct one.
			 */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			if (p->opr.nops > 2) { /* if else */
				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				/* Don't allow the "if" body to have a stray value on the stack */
				before = pushCounter;
				ex(p->opr.op[1]);
				PUSHCOUNTER_EXPECT(before);

				printf("\tjmp\tL%03d\n", lbl2 = lbl++);
				printf("L%03d:\n", lbl1);

				/* Don't allow the "else" body to have a stray value on the stack */
				before = pushCounter;
				ex(p->opr.op[2]);
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl2);
			} else { /* if */
				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				/* Don't allow the "if" body to have a stray value on the stack */
				before = pushCounter;
				ex(p->opr.op[1]);
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl1);
			}
			break;
		case PRINT:
			/* Expect the expression to push one value to the stack */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tprint\n");
			break;
		case '=':
			/* Expect the expression to push one value to the stack */
			before = pushCounter;
			ex(p->opr.op[1]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'a');
			PUSHCOUNTER_SUB();
			break;
		case UMINUS:
			/* Expect the expression to push one value to the stack */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\tneg\t%rax\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case FACT:
			/* Expect the expression to push one value to the stack */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tfact\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case LNTWO:
			/* Expect the expression to push one value to the stack */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tlntwo\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		default:
			/*
			 * Combined expressions will not push any values to the stack.
			 * So just evaluate the expressions and break.
			 */
			if (p->opr.oper == ';') {
				before = pushCounter;
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);
				PUSHCOUNTER_EXPECT(before);
				break;
			}

			/* Expect each expression to only push one value to the stack, each */
			before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);
			ex(p->opr.op[1]);
			PUSHCOUNTER_EXPECT(before + 2);

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tpopq\t%rax\n");
			PUSHCOUNTER_SUB();

			/*
			 * This macro function generates the correct way of handling the different
			 * ways of comparing the two numbers. If the expression is true, push 1,
			 * else push 0.
			 * The /if/ or /while/ statement expects to be able to pop a value.
			 */
#define CMP(function) do {															\
				printf("\tcmp %rdi, %rax\n");										\
				printf("\t" function " L%03d\n", lbl1 = lbl++);	\
				printf("\tpushq\t$0\n");												\
				printf("\tjmp\tL%03d\n", lbl2 = lbl++);					\
				printf("L%03d:\n", lbl1);												\
				printf("\tpushq\t$1\n");												\
				printf("L%03d:\n", lbl2);												\
				PUSHCOUNTER_ADD();															\
			} while (0)

			switch(p->opr.oper) {
			case GCD:
				printf("\tmov\t%rdi, %rsi\n");
				printf("\tmov\t%rax, %rdi\n");
				printf("\tcall\tgcd\n");
				printf("\tpush\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '+':
				printf("\tadd\t%rdi, %rax\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '-':
				printf("\tsub\t%rdi, %rax\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '*':
				printf("\timul\t%rdi\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '/':
				// 'cqo' will sign-extend %rax into %rdx, a requirement for idiv.
				printf("\tcqo\n");
				printf("\tidiv\t%rdi\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '<':
				CMP("jl");
				break;
			case '>':
				CMP("jg");
				break;
			case GE:
				CMP("jge");
				break;
			case LE:
				CMP("jle");
				break;
			case NE:
				CMP("jne");
				break;
			case EQ:
				CMP("je");
				break;
			default:
				_e("Unknown operator: '%c' / %d\n", (char)p->opr.oper, p->opr.oper);
			}
		}
	}

	return 0;
}
