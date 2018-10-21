#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;
static int rootExpression;
static int indent = 1;

void printIndent() {
	for(int i = 0; i < indent; i++)
		printf("\t");
}

int ex(nodeType *p) {
	int needSemicolon = 1;

	if (!p)
		return 0;

	//intf("/* rootExpression = %d, indent = %d*/", rootExpression, indent);
	if (!rootExpression)
		printIndent();

	rootExpression++;

	switch(p->type) {
	case typeCon:
		printf("%d", p->con.value);
		break;
	case typeId:
		printf("%c", p->id.i + 'A');
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			printf("while (");
			ex(p->opr.op[0]);
			printf(") {\n");
			{
				int oldRoot = rootExpression;
				rootExpression = 0;
				indent++;
				ex(p->opr.op[1]);
				indent--;
				rootExpression = oldRoot;
			}
			printIndent();
			printf("}");
			needSemicolon = 0;
			break;
		case IF:
			printf("if (");
			ex(p->opr.op[0]);
			printf(") {\n");
			{
				int oldRoot = rootExpression;
				rootExpression = 0;
				indent++;
				ex(p->opr.op[1]);
				indent--;
				rootExpression = oldRoot;
			}
			printIndent();
			printf("}");
			if (p->opr.nops > 2) {
				printf(" else {");
				{
					int oldRoot = rootExpression;
					rootExpression = 0;
					indent++;
					ex(p->opr.op[2]);
					indent--;
					rootExpression = oldRoot;
				}
				printIndent();
				printf("}");
			}
			needSemicolon = 0;
			break;
		case PRINT:
			printf("print(");
			ex(p->opr.op[0]);
			printf(")");
			break;
		case '=':
			printf("%c = ", p->opr.op[0]->id.i + 'A');
			ex(p->opr.op[1]);
			break;
		case UMINUS:
			printf("-(");
			ex(p->opr.op[0]);
			printf(")");
			break;
		case FACT:
	    printf("fact(");
			ex(p->opr.op[0]);
	    printf(")");
	    break;
		case LNTWO:
	    printf("lntwo(");
	    ex(p->opr.op[0]);
	    printf(")");
	    break;
		default:
			if (p->opr.oper == GCD) {
				printf("gcd(");
				ex(p->opr.op[0]);
				printf(", ");
				ex(p->opr.op[1]);
				printf(")");
			} else if (p->opr.oper == ';') {
				ex(p->opr.op[0]);
				printf(";\n");

				printIndent();
				ex(p->opr.op[1]);
			} else {
				printf("(");
				ex(p->opr.op[0]);
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
				ex(p->opr.op[1]);
				printf(")");
			}
			break;
		}
	}
	if (--rootExpression == 0) {
		if (needSemicolon)
			printf(";");
		printf("\n");
	}
	return 0;
}
