#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

static int indent = 1;

static void printIndent() {
	for(int i = 0; i < indent; i++)
		printf("\t");
}

int ex(nodeType *p) {
	/* Does this expression need to end with a semicolon */
	int needSemicolon = 1;
	int needNewLine = 1;

	if (!p)
		return 0;

	switch(p->type) {
	case typeCon:
		printIndent();
		printf("push(%d)", p->con.value);
		break;
	case typeId:
		printIndent();
		printf("push(%c)", p->id.i + 'a');
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			/*
			 * It was implemented like this as the /p->opr.op[0]/ code need to run
			 * each time to decide if the loop body should run, and the C
			 * implementations need to emulate a stack. This would have been a lot
			 * if just a .calc->C translation was allowed!
			 *
			 * Output code:
			 * while (true) {
			 *  {
			 *    op[0]();
			 *    if (!pop()) break;
			 *  }
			 *  // Real code here
			 * }
			 */
			printIndent();
			printf("while (1) {\n");
			{
				/* Add one more level of indentation in the body */
				indent++;
				printIndent();
				printf("{\n");
				{
					indent++;
					ex(p->opr.op[0]);
					printIndent();
					printf("if (!pop()) break;\n");
					indent--;
				}
				printIndent();
				printf("}\n");
				ex(p->opr.op[1]);
				indent--;
			}
			printIndent();
			printf("}");
			needSemicolon = 0;
			break;
		case IF:
			ex(p->opr.op[0]);
			printIndent();
			printf("if (pop()) {\n");
			{
				/* Add one more level of indentation in the body */
				indent++;
				ex(p->opr.op[1]);
				indent--;
			}
			printIndent();
			printf("}");
			if (p->opr.nops > 2) {
				printf(" else {\n");
				{
					/* Add one more level of indentation in the body */
					indent++;
					ex(p->opr.op[2]);
					indent--;
				}
				printIndent();
				printf("}");
			}
			needSemicolon = 0;
			break;
		case PRINT:
			ex(p->opr.op[0]);
			printIndent();
			printf("print(pop())");
			break;
		case '=':
			ex(p->opr.op[1]);
			printIndent();
			printf("%c = pop()", p->opr.op[0]->id.i + 'a');
			break;
		case UMINUS:
			ex(p->opr.op[0]);
			printIndent();
			printf("push(-pop())");
			break;
		case FACT:
			ex(p->opr.op[0]);
			printIndent();
	    printf("push(fact(pop()))");
			break;
		case LNTWO:
	    ex(p->opr.op[0]);
			printIndent();
	    printf("push(lntwo(pop()))");
			break;
		default:
			if (p->opr.oper == GCD) {
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);
				printIndent();
				printf("{\n");
				{
					indent++;
					printIndent();
					printf("int64_t arg2 = pop();\n");
					printIndent();
					printf("int64_t arg1 = pop();\n");
					printIndent();
					printf("push(gcd(arg1, arg2));\n");
					indent--;
				}
				printIndent();
				printf("}");
				needSemicolon = 0;
			} else if (p->opr.oper == ';') {
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);
				needSemicolon = 0;
				needNewLine = 0;
			} else {
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);

				printIndent();
				printf("{\n");
				{
					indent++;
					printIndent();
					printf("int64_t arg2 = pop();\n");
					printIndent();
					printf("int64_t arg1 = pop();\n");
					printIndent();
					printf("push(arg1");
					switch(p->opr.oper) {
					case '+':   printf(" + "); break;
					case '-':   printf(" - "); break;
					case '*':   printf(" * "); break;
					case '/':   printf(" / "); break;
					case '<':   printf(" < "); break;
					case '>':   printf(" > "); break;
					case GE:    printf(" >= "); break;
					case LE:    printf(" <= "); break;
					case NE:    printf(" != "); break;
					case EQ:    printf(" == "); break;
					}
					printf("arg2);\n");
					indent--;
				}
				printIndent();
				printf("}");
				needSemicolon = 0;
			}
			break;
		}
	}

	if (needSemicolon)
		printf(";");
	if (needNewLine)
		printf("\n");
	return 0;
}
