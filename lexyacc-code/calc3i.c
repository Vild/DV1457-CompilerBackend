#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

//#define NDEBUG

#ifndef NDEBUG
#define _(...) fprintf(stdout, __VA_ARGS__)
#else
#define _(...)
#endif


static int pushCounter = 0;

#define PUSHCOUNTER_ADD() do { pushCounter++;  _("// pushCounter: %d\n", pushCounter); } while (0)
#define PUSHCOUNTER_SUB() do { pushCounter--;  _("// pushCounter: %d\n", pushCounter); } while (0)
#define PUSHCOUNTER_EXPECT(n) do { if (n != pushCounter) _("\t\t//PROBLEM HERE %d != %d\n", n, pushCounter); } while (0)

int ex(nodeType *p) {
	int lbl1 = 0, lbl2 = 0;

	if (!p)
		return 0;
	switch(p->type) {
	case typeCon:
		_("\t\t\t\t// typeCon\n");
		printf("\tpushq\t$%d\n", p->con.value);
		PUSHCOUNTER_ADD();
		break;
	case typeId:
		_("\t\t\t\t// typeId\n");
		printf("\tpushq\t%c\n", p->id.i + 'a');
		PUSHCOUNTER_ADD();
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			_("\t\t\t\t// typeOpr == WHILE\n");
			printf("L%03d:\n", lbl1 = lbl++);

			int before = pushCounter;
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\ttest\t%rax,%rax\n");
			printf("\tjz\tL%03d\n", lbl2 = lbl++);

			before = pushCounter;
			_("\t\t\t\t// <OP1>\n");
				ex(p->opr.op[1]);
				_("\t\t\t\t// </OP1>\n");
				PUSHCOUNTER_EXPECT(before);

				printf("\tjmp\tL%03d\n", lbl1);
				printf("L%03d:\n", lbl2);
				break;
		case IF:
			_("\t\t\t\t// typeOpr == IF\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");

			if (p->opr.nops > 2) {
				/* if else */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				_("\t\t\t\t// <OP1>\n");
				ex(p->opr.op[1]);
				_("\t\t\t\t// </OP1>\n");
				PUSHCOUNTER_EXPECT(before);

				printf("\tjmp\tL%03d\n", lbl2 = lbl++);
				printf("L%03d:\n", lbl1);

				before = pushCounter;
				_("\t\t\t\t// <OP2>\n");
					ex(p->opr.op[2]);
					_("\t\t\t\t// </OP2>\n");
					PUSHCOUNTER_EXPECT(before);

					printf("L%03d:\n", lbl2);
			} else {
				/* if */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				_("\t\t\t\t// <OP1>\n");
				ex(p->opr.op[1]);
				_("\t\t\t\t// </OP1>\n");
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl1);
			}
			break;
		case PRINT:
			_("\t\t\t\t// typeOpr == PRINT\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tprint\n");
			break;
		case '=':
			_("\t\t\t\t// typeOpr == EQUALS\n");
			_("\t\t\t\t// <OP1>\n");
			ex(p->opr.op[1]);
			_("\t\t\t\t// </OP1>\n");

			printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'a');
			PUSHCOUNTER_SUB();
			break;
		case UMINUS:
			_("\t\t\t\t// typeOpr == UMINUS\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\tneg\t%rax\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case FACT:
			_("\t\t\t\t// typeOpr == FACT\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tfact\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case LNTWO:
			_("\t\t\t\t// typeOpr == LNTWO\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tlntwo\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		default:
			_("\t\t\t\t// typeOpr == <DEFAULT>\n");
			_("\t\t\t\t// <OP0>\n");
			ex(p->opr.op[0]);
			_("\t\t\t\t// </OP0>\n");
			_("\t\t\t\t// <OP1>\n");
			ex(p->opr.op[1]);
			_("\t\t\t\t// </OP1>\n");

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
				printf("\tdiv\t%rdi\n");
				printf("\tpushq\t%rax\n");
				PUSHCOUNTER_ADD();
				break;
			case '<':
				CMP("jb");
				break;
			case '>':
				CMP("ja");
				break;
			case GE:
				CMP("jae");
				break;
			case LE:
				CMP("jbe");
				break;
			case NE:
				CMP("jne");
				break;
			case EQ:
				CMP("je");
				break;
			}
		}
	}
	return 0;
}
