#include <stdio.h>
#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;

#define NDEBUG

#ifndef NDEBUG
#define _(fmt, ...) do { fflush(stdout); fprintf(stderr, "\x1b[1;33m" fmt "\x1b[0m", ##__VA_ARGS__); } while (0)
#define _e(fmt, ...) do { fflush(stdout); fprintf(stderr, "\x1b[1;31m" fmt "\x1b[0m", ##__VA_ARGS__); } while (0)
#else
#define _(...)
#define _e(...)
#endif

static int pushCounter = 0;

#define PUSHCOUNTER_ADD() do { pushCounter++;  _("\t\t\t\t// pushCounter: %d\n", pushCounter); } while (0)
#define PUSHCOUNTER_SUB() do { pushCounter--;						\
		if (pushCounter < 0)																\
			_e("\t\t\t\t// pushCounter: %d\n", pushCounter);	\
		else																								\
			_("\t\t\t\t// pushCounter: %d\n", pushCounter);		\
	} while (0)
#define PUSHCOUNTER_EXPECT(n) do {												\
		if (n != pushCounter) {																\
			_("\t\t//PROBLEM HERE %d != %d\n", n, pushCounter);	\
			exit(-1);																						\
		}																											\
	} while (0)

int ex(nodeType *p) {
	int lbl1 = 0, lbl2 = 0;

	static int inBlock = 0;
	static int blockCount = 0;
	static int subBlockCount = 0;

	if (!p)
		return 0;

	int rootBlock = !inBlock;
	if (rootBlock) {
		subBlockCount = 0;
		_("\n// ======== Block %d ========\n", blockCount++);
		inBlock = 1;
	} else
		_("\n    // ======== Sub-block %d.%d ========\n", blockCount, subBlockCount++);

	switch(p->type) {
	case typeCon:
		_("\t\t\t\t// typeCon\n");
		printf("\tpushq\t$%d\n", p->con.value);
		PUSHCOUNTER_ADD();
		break;
	case typeId:
		_("\t\t\t\t// typeId\n");
		printf("\tpushq\t%c\n", p->id.i + 'A');
		PUSHCOUNTER_ADD();
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			_("\t\t\t\t// typeOpr == WHILE\n");
			printf("L%03d:\n", lbl1 = lbl++);

			int before = pushCounter;
			_("\t// <CONDITION>\n");
			ex(p->opr.op[0]);
			_("\t// </CONDITION>\n");
			PUSHCOUNTER_EXPECT(before + 1);

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\ttest\t%rax,%rax\n");
			printf("\tjz\tL%03d\n", lbl2 = lbl++);

			before = pushCounter;
			_("\t// <BODY>\n");
			ex(p->opr.op[1]);
			_("\t// </BODY>\n");
			PUSHCOUNTER_EXPECT(before);

			printf("\tjmp\tL%03d\n", lbl1);
			printf("L%03d:\n", lbl2);
			break;
		case IF:
			_("\t\t\t\t// typeOpr == IF\n");
			_("\t// <CONDITION>\n");
			ex(p->opr.op[0]);
			_("\t// </CONDITION>\n");

			if (p->opr.nops > 2) {
				/* if else */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				_("\t// <BODY>\n");
				ex(p->opr.op[1]);
				_("\t// </BODY>\n");
				PUSHCOUNTER_EXPECT(before);

				printf("\tjmp\tL%03d\n", lbl2 = lbl++);
				printf("L%03d:\n", lbl1);

				before = pushCounter;
				_("\t// <ELSE-BODY>\n");
					ex(p->opr.op[2]);
					_("\t// </ELSE-BODY>\n");
					PUSHCOUNTER_EXPECT(before);

					printf("L%03d:\n", lbl2);
			} else {
				/* if */

				printf("\tpop\t%rax\n");
				PUSHCOUNTER_SUB();
				printf("\ttest\t%rax,%rax\n");
				printf("\tjz\tL%03d\n", lbl1 = lbl++);

				int before = pushCounter;
				_("\t// <BODY>\n");
				ex(p->opr.op[1]);
				_("\t// </BODY>\n");
				PUSHCOUNTER_EXPECT(before);

				printf("L%03d:\n", lbl1);
			}
			break;
		case PRINT:
			_("\t\t\t\t// typeOpr == PRINT\n");
			_("\t// <EXPRESSION>\n");
			ex(p->opr.op[0]);
			_("\t// </EXPRESSION>\n");

			printf("\tpopq\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tprint\n");
			break;
		case '=':
			_("\t\t\t\t// typeOpr == EQUALS\n");
			_("\t// <EXPRESSION>\n");
			ex(p->opr.op[1]);
			_("\t// </EXPRESSION>\n");

			printf("\tpopq\t%c\n", p->opr.op[0]->id.i + 'A');
			PUSHCOUNTER_SUB();
			break;
		case UMINUS:
			_("\t\t\t\t// typeOpr == UMINUS\n");
			_("\t// <EXPRESSION>\n");
			ex(p->opr.op[0]);
			_("\t// </EXPRESSION>\n");

			printf("\tpop\t%rax\n");
			PUSHCOUNTER_SUB();
			printf("\tneg\t%rax\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case FACT:
			_("\t\t\t\t// typeOpr == FACT\n");
			_("\t// <EXPRESSION>\n");
			ex(p->opr.op[0]);
			_("\t// </EXPRESSION>\n");

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tfact\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		case LNTWO:
			_("\t\t\t\t// typeOpr == LNTWO\n");
			_("\t// <EXPRESSION>\n");
			ex(p->opr.op[0]);
			_("\t// </EXPRESSION>\n");

			printf("\tpop\t%rdi\n");
			PUSHCOUNTER_SUB();
			printf("\tcall\tlntwo\n");
			printf("\tpush\t%rax\n");
			PUSHCOUNTER_ADD();
			break;
		default:
			#ifndef NDEBUG
			{
				_("\t\t\t\t// typeOpr == <DEFAULT> (");
				switch (p->opr.oper) {
				case GCD: _("GCD"); break;
				case '+': _("+"); break;
				case '-': _("-"); break;
				case '*': _("*"); break;
				case '/': _("/"); break;
				case '<': _("<"); break;
				case '>': _(">"); break;
				case GE: _("GE"); break;
				case LE: _("LE"); break;
				case NE: _("NE"); break;
				case EQ: _("EQ"); break;
				default: _("%c|%d", (char)p->opr.oper, p->opr.oper); break;	
				}
				_(")\n");
			}
			#endif
			_("\t// <LEFT>\n");
			ex(p->opr.op[0]);
			_("\t// </LEFT>\n");
			_("\t// <RIGHT>\n");
			ex(p->opr.op[1]);
			_("\t// </RIGHT>\n");

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

	if (rootBlock)
		inBlock = 0;
	return 0;
}
