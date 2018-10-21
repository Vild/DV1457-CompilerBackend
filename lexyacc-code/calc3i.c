#include <stdio.h>
#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

#define _e(fmt, ...) do { fflush(stdout); fprintf(stdout, "\x1b[1;31m" fmt "\x1b[0m", ##__VA_ARGS__); } while (0)

static int pushCounter = 0;

#define PUSHCOUNTER_ADD() do { pushCounter++; } while (0)
#define PUSHCOUNTER_SUB() do { pushCounter--;						\
		if (pushCounter < 0)																\
			_e("\t\t\t\t// pushCounter: %d\n", pushCounter);	\
	} while (0)
#define PUSHCOUNTER_EXPECT(n) do {												\
		if (n != pushCounter) {																\
			_e("\t\t//PROBLEM HERE %d != %d\n", n, pushCounter);	\
			exit(-1);																						\
		}																											\
	} while (0)

int ex(nodeType *p) {
	int lbl1 = 0, lbl2 = 0;

	if (!p)
		return 0;

	switch(p->type) {
	case typeCon:
		printf("\tpushq\t$%d\n", p->con.value);
		PUSHCOUNTER_ADD();
		break;
	case typeId:
		printf("\tpushq\t%c\n", p->id.i + 'A');
		PUSHCOUNTER_ADD();
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			printf("L%03d:\n", lbl1 = lbl++);

			int before = pushCounter;
			ex(p->opr.op[0]);
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\ttest\t%rax,%rax\n");
			printf("\tjz\tL%03d\n", lbl2 = lbl++);

			before = pushCounter;
			ex(p->opr.op[1]);
			PUSHCOUNTER_EXPECT(before);

			printf("\tjmp\tL%03d\n", lbl1);
			printf("L%03d:\n", lbl2);
			break;
		case IF:
			ex(p->opr.op[0]);

			if (p->opr.nops > 2) {
				/* if else */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				ex(p->opr.op[1]);
				PUSHCOUNTER_EXPECT(before);

				printf("\tjmp\tL%03d\n", lbl2 = lbl++);
				printf("L%03d:\n", lbl1);

				before = pushCounter;
				ex(p->opr.op[2]);
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl2);
			} else {
				/* if */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				ex(p->opr.op[1]);
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl1);
			}
			break;
		case PRINT:
			ex(p->opr.op[0]);

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tprint\n");
			break;
		case '=':
			ex(p->opr.op[1]);

			printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'A');
			PUSHCOUNTER_SUB();
			break;
		case UMINUS:
			ex(p->opr.op[0]);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\tneg\t%rax\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case FACT:
			ex(p->opr.op[0]);

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tfact\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case LNTWO:
			ex(p->opr.op[0]);

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tlntwo\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		default:
			ex(p->opr.op[0]);
			ex(p->opr.op[1]);

			// Don't try and get any values from a combined expression
			if (p->opr.oper == ';')
				break;

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tpopq\t%rax\n");
			PUSHCOUNTER_SUB();

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
				printf("\tmul\t%rdi\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '/':
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
