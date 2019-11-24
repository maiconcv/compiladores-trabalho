#include "tacs.h"

AST* getRootAST();

// INTERNAL PROTOTYPES
TAC* makeBinOp(int type, TAC* code0, TAC* code1);
TAC* makeIfThen(TAC* code0, TAC* code1);
TAC* makeIfThenElse(TAC* code0, TAC* code1, TAC* code2);
TAC* makeWhile(TAC* code0, TAC* code1);
TAC* makeFor(HASH_NODE* var, TAC* code0, TAC* code1, TAC* code2, TAC* code3);
HASH_NODE* findParam(AST* paramList, int currIteration, int n);

TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2){
        TAC* newtac;

        newtac = (TAC*)calloc(1, sizeof(TAC));
        newtac->type = type;
        newtac->res = res;
        newtac->op1 = op1;
        newtac->op2 = op2;
        newtac->prev = 0;
        newtac->next = 0;

        return newtac;
}

void tacPrintSingle(TAC* tac){
        if(!tac) return;
        if(tac->type == TAC_SYMBOL) return;

        fprintf(stderr, "TAC(");
        switch (tac->type) {
                case TAC_SYMBOL: fprintf(stderr, "TAC_SYMBOL"); break;
                case TAC_ADD: fprintf(stderr, "TAC_ADD"); break;
                case TAC_SUB: fprintf(stderr, "TAC_SUB"); break;
                case TAC_MUL: fprintf(stderr, "TAC_MUL"); break;
                case TAC_DIV: fprintf(stderr, "TAC_DIV"); break;
                case TAC_MOVE: fprintf(stderr, "TAC_MOVE"); break;
                case TAC_IFZ: fprintf(stderr, "TAC_IFZ"); break;
                case TAC_LABEL: fprintf(stderr, "TAC_LABEL"); break;
                case TAC_LT: fprintf(stderr, "TAC_LT"); break;
                case TAC_GT: fprintf(stderr, "TAC_GT"); break;
                case TAC_LE: fprintf(stderr, "TAC_LE"); break;
                case TAC_GE: fprintf(stderr, "TAC_GE"); break;
                case TAC_EQ: fprintf(stderr, "TAC_EQ"); break;
                case TAC_DIF: fprintf(stderr, "TAC_DIF"); break;
                case TAC_AND: fprintf(stderr, "TAC_AND"); break;
                case TAC_OR: fprintf(stderr, "TAC_OR"); break;
                case TAC_NOT: fprintf(stderr, "TAC_NOT"); break;
                case TAC_JUMP: fprintf(stderr, "TAC_JUMP"); break;
                case TAC_READ: fprintf(stderr, "TAC_READ"); break;
                case TAC_BEGINFUN: fprintf(stderr, "TAC_BEGINFUN"); break;
                case TAC_ENDFUN: fprintf(stderr, "TAC_ENDFUN"); break;
                case TAC_RETURN: fprintf(stderr, "TAC_RETURN"); break;
                case TAC_PRINT: fprintf(stderr, "TAC_PRINT"); break;
                case TAC_MOVEVECT: fprintf(stderr, "TAC_MOVEVECT"); break;
                case TAC_VECTREAD: fprintf(stderr, "TAC_VECTREAD"); break;
                case TAC_PARAM: fprintf(stderr, "TAC_PARAM"); break;
                case TAC_VARDECL: fprintf(stderr, "TAC_VARDECL"); break;
                case TAC_VECTDECL: fprintf(stderr, "TAC_VECTDECL"); break;
                case TAC_FUNCALL: fprintf(stderr, "TAC_FUNCALL"); break;
                case TAC_ARG: fprintf(stderr, "TAC_ARG"); break;
                case TAC_BREAK: fprintf(stderr, "TAC_BREAK"); break;
                default: fprintf(stderr, "UNKNOWN"); break;
        }

        if(tac->res) fprintf(stderr, ",%s", tac->res->text);
        else fprintf(stderr, ",0");

        if(tac->op1) fprintf(stderr, ",%s", tac->op1->text);
        else fprintf(stderr, ",0");

        if(tac->op2) fprintf(stderr, ",%s", tac->op2->text);
        else fprintf(stderr, ",0");

        fprintf(stderr, ");\n");
}

void tacPrintBackwards(TAC* tac){
        for(; tac; tac = tac->prev)
                tacPrintSingle(tac);
}

TAC* tacJoin(TAC* l1, TAC* l2){
        TAC* tac;
        if(!l1) return l2;
        if(!l2) return l1;

        for(tac = l2; tac->prev; tac = tac->prev)
                ;

        tac->prev = l1;
        return l2;
}

TAC* generateCode(AST* ast, HASH_NODE* funCallName, int funArgCounter){
        if(!ast) return 0;

        TAC* code[MAX_SONS];

        HASH_NODE* currFunCall = 0;
        int currArgCounter = 0;
        HASH_NODE* currArgCounterSymbol = 0;
        if(ast->type == AST_FUNCALL){
                currFunCall = ast->symbol;
                currArgCounter = 0;
        }
        else if(ast->type == AST_FUNARG){
                currFunCall = funCallName;
                currArgCounter = funArgCounter + 1;

                char text[100];
                sprintf(text, "%d", currArgCounter);
                currArgCounterSymbol = hashInsert(text, SYMBOL_LITINT, 0);
        }

        // generate code for my sons first
        for(int i = 0; i < MAX_SONS; ++i)
                code[i] = generateCode(ast->son[i], currFunCall, currArgCounter);

        // then process this node
        switch (ast->type) {
                case AST_SYMBOL: return tacCreate(TAC_SYMBOL, ast->symbol, 0, 0);
                        break;
                case AST_ASSIGN: return tacJoin(code[0], tacCreate(TAC_MOVE, ast->symbol, code[0]?code[0]->res:0, 0));
                        break;
                case AST_ADD: return makeBinOp(TAC_ADD, code[0], code[1]);
                        break;
                case AST_SUB: return makeBinOp(TAC_SUB, code[0], code[1]);
                        break;
                case AST_MUL: return makeBinOp(TAC_MUL, code[0], code[1]);
                        break;
                case AST_DIV: return makeBinOp(TAC_DIV, code[0], code[1]);
                        break;
                case AST_LT: return makeBinOp(TAC_LT, code[0], code[1]);
                        break;
                case AST_GT: return makeBinOp(TAC_GT, code[0], code[1]);
                        break;
                case AST_LE: return makeBinOp(TAC_LE, code[0], code[1]);
                        break;
                case AST_GE: return makeBinOp(TAC_GE, code[0], code[1]);
                        break;
                case AST_EQ: return makeBinOp(TAC_EQ, code[0], code[1]);
                        break;
                case AST_DIF: return makeBinOp(TAC_DIF, code[0], code[1]);
                        break;
                case AST_AND: return makeBinOp(TAC_AND, code[0], code[1]);
                        break;
                case AST_OR: return makeBinOp(TAC_OR, code[0], code[1]);
                        break;
                case AST_NOT: return tacJoin(code[0], tacCreate(TAC_NOT, makeTemp(), code[0]?code[0]->res:0, 0));
                        break;
                case AST_IF: return makeIfThen(code[0], code[1]);
                        break;
                case AST_IFELSE: return makeIfThenElse(code[0], code[1], code[2]);
                        break;
                case AST_WHILE: return makeWhile(code[0], code[1]);
                        break;
                case AST_FOR: return makeFor(ast->symbol, code[0], code[1], code[2], code[3]);
                        break;
                case AST_READ: return tacCreate(TAC_READ, ast->symbol, 0, 0);
                        break;
                case AST_RETURN: return tacJoin(code[0], tacCreate(TAC_RETURN, code[0]?code[0]->res:0, 0, 0));
                        break;
                case AST_FUNDECL: return tacJoin(tacJoin(tacJoin(tacCreate(TAC_BEGINFUN, ast->symbol, 0, 0), code[1]), code[2]), tacCreate(TAC_ENDFUN, ast->symbol, 0, 0));
                        break;
                case AST_PARAM: return tacCreate(TAC_PARAM, ast->symbol, 0, 0);
                        break;
                case AST_LSTPARAM: return tacJoin(code[0], code[1]);
                        break;
                case AST_BLOCK: return code[0];
                        break;
                case AST_VARDECL: return tacCreate(TAC_VARDECL, ast->symbol, code[1]?code[1]->res:0, 0);
                        break;
                case AST_VECTDECL: return tacJoin(tacJoin(code[1], tacCreate(TAC_VECTDECL, ast->symbol, code[1]?code[1]->res:0, 0)), code[2]);
                        break;
                case AST_VECTINIT: return code[0];
                        break;
                case AST_LSTLIT: return tacJoin(code[0], code[1]);
                        break;
                case AST_PRINT: return code[0];
                        break;
                case AST_PRINTARG: return tacJoin(tacJoin(code[0], tacCreate(TAC_PRINT, code[0]?code[0]->res:0, 0, 0)), code[1]);
                        break;
                case AST_VECTASSIGN: return tacJoin(tacJoin(code[0], code[1]), tacCreate(TAC_MOVEVECT, ast->symbol, code[0]?code[0]->res:0, code[1]?code[1]->res:0));
                        break;
                case AST_VECTREAD: return tacJoin(code[0], tacCreate(TAC_VECTREAD, makeTemp(), ast->symbol, code[0]?code[0]->res:0));
                        break;
                case AST_BRACKETS: return code[0];
                        break;
                case AST_LCMD: return tacJoin(code[0], code[1]);
                        break;
                case AST_LDECL: return tacJoin(code[0], code[1]);
                        break;
                case AST_FUNCALL: return tacJoin(code[0], tacCreate(TAC_FUNCALL, makeTemp(), ast->symbol, 0));
                        break;
                case AST_FUNARG: return tacJoin(tacJoin(code[0], tacCreate(TAC_ARG, code[0]?code[0]->res:0, currFunCall, currArgCounterSymbol)), code[1]);
                        break;
                case AST_BREAK: return tacCreate(TAC_BREAK, 0, 0, 0);
                        break;
                default: return tacJoin(tacJoin(tacJoin(code[0], code[1]), code[2]), code[3]);
                        break;
        }
        return 0;
}

TAC* makeBinOp(int type, TAC* code0, TAC* code1){
        return tacJoin(tacJoin(code0, code1), tacCreate(type, makeTemp(), code0?code0->res:0, code1?code1->res:0));
}

TAC* makeIfThen(TAC* code0, TAC* code1){
        HASH_NODE* label = 0;
        TAC* tacif = 0;
        TAC* taclabel = 0;

        label = makeLabel();
        tacif = tacCreate(TAC_IFZ, label, code0?code0->res:0, 0);
        taclabel = tacCreate(TAC_LABEL, label, 0, 0);

        return tacJoin(tacJoin(tacJoin(code0, tacif), code1), taclabel);
}

TAC* makeIfThenElse(TAC* code0, TAC* code1, TAC* code2){
        HASH_NODE* labelFalse = makeLabel();
        HASH_NODE* labelTrue = makeLabel();
        TAC* tacif = tacCreate(TAC_IFZ, labelFalse, code0?code0->res:0, 0);
        TAC* tacjump = tacCreate(TAC_JUMP, labelTrue, 0, 0);
        TAC* taclabelFalse = tacCreate(TAC_LABEL, labelFalse, 0, 0);
        TAC* taclabelTrue = tacCreate(TAC_LABEL, labelTrue, 0, 0);

        return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(code0, tacif), code1), tacjump), taclabelFalse), code2), taclabelTrue);
}

TAC* makeWhile(TAC* code0, TAC* code1){
        HASH_NODE* labelBeforeWhile = makeLabel();
        HASH_NODE* labelLeaveWhile = makeLabel();
        TAC* tacif = tacCreate(TAC_IFZ, labelLeaveWhile, code0?code0->res:0, 0);
        TAC* tacjump = tacCreate(TAC_JUMP, labelBeforeWhile, 0, 0);
        TAC* taclabelBeforeWhile = tacCreate(TAC_LABEL, labelBeforeWhile, 0, 0);
        TAC* taclabelLeaveWhile = tacCreate(TAC_LABEL, labelLeaveWhile, 0, 0);

        return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(taclabelBeforeWhile, code0), tacif), code1), tacjump), taclabelLeaveWhile);
}

TAC* makeFor(HASH_NODE* var, TAC* code0, TAC* code1, TAC* code2, TAC* code3){
        HASH_NODE* labelBeforeFor = makeLabel();
        HASH_NODE* labelLeaveFor = makeLabel();
        HASH_NODE* compareResult = makeTemp();

        TAC* tacMoveInitValue = tacCreate(TAC_MOVE, var, code0?code0->res:0, 0);
        TAC* tacLabelBeforeFor = tacCreate(TAC_LABEL, labelBeforeFor, 0, 0);

        TAC* tacCompare;
        int incNumber = atoi(code2->res->text);
        if(incNumber >= 0)
                tacCompare = tacCreate(TAC_LT, compareResult, var, code1?code1->res:0);
        else
                tacCompare = tacCreate(TAC_GT, compareResult, var, code1?code1->res:0);

        TAC* tacIf = tacCreate(TAC_IFZ, labelLeaveFor, compareResult, 0);
        TAC* tacInc = tacCreate(TAC_ADD, makeTemp(), var, code2?code2->res:0);
        TAC* tacMoveInc = tacCreate(TAC_MOVE, var, tacInc?tacInc->res:0, 0);
        TAC* tacJump = tacCreate(TAC_JUMP, labelBeforeFor, 0, 0);
        TAC* tacLabelLeaveFor = tacCreate(TAC_LABEL, labelLeaveFor, 0, 0);

        return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(code0, tacMoveInitValue), tacLabelBeforeFor), code1), tacCompare), tacIf), code3), code2), tacInc), tacMoveInc), tacJump), tacLabelLeaveFor);
}

void tacPrintForwards(TAC* tac){
        if(!tac) return;

        tacPrintForwards(tac->prev);
        tacPrintSingle(tac);
}

void generateASM(TAC* tac, FILE* fout){
        static int functionCounter = 0;

        if(!tac) return;

        if(tac->prev)
                generateASM(tac->prev, fout);

        switch (tac->type) {
                case TAC_VARDECL: fprintf(fout, "\n\t.data\n"
                                                "_%s:\t.long\t%s\n", tac->res->text, tac->op1->text); // won't be 'long' for all, type will come from hash
                        break;
                case TAC_BEGINFUN: fprintf(fout, "\n## TAC_BEGINFUN\n"
                                                 "\t.globl\t%s\n"
                                                 "\t.type\t%s, @function\n"
                                                 "%s:\n"
                                                 ".LFB%d:\n"
                                                 "\tpushq\t%%rbp\n"
                                                 "\tmovq\t%%rsp, %%rbp\n", tac->res->text, tac->res->text, tac->res->text, functionCounter);
                        break;
                case TAC_ENDFUN: fprintf(fout, "## TAC_ENDFUN\n"
                                                "\tmovl\t$0, %%eax\n"
                                                "\tpopq\t%%rbp\n"
                                                "\tret\n"
                                                ".LFE%d:\n"
                                                "\t.size\t%s, .-%s\n", functionCounter, tac->res->text, tac->res->text);
                        functionCounter++;
                        break;
                case TAC_PRINT:
                        switch (tac->res->type) {
                                case SYMBOL_LITSTRING:{
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "## TAC_PRINT_LITSTRING\n"
                                                        "\tleaq\t_%s%d(%%rip), %%rdi\n"
                                                        "\tcall\tprintf@PLT\n", LITSTR_VAR_NAME, counter);
                                        break;
                                }
                                case SYMBOL_LITCHAR:{
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "## TAC_PRINT_LITCHAR\n"
                                                        "\tmovzbl\t_%s%d(%%rip), %%eax\n"
                                                        "\tmovsbl\t%%al, %%eax\n"
                                                        "\tmovl\t%%eax, %%edi\n"
                                                        "\tcall\tputchar@PLT\n", LITCHAR_VAR_NAME, counter);
                                        break;
                                }
                                case SYMBOL_SCALAR:
                                        fprintf(fout, "## TAC_PRINT_VAR\n"
                                                        "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tmovl\t%%eax, %%esi\n"
                                                        "\tleaq\t_LC0(%%rip), %%rdi\n"
                                                        "\tmovl\t$0, %%eax\n"
                                                        "\tcall\tprintf@PLT\n", tac->res->text);
                                        break;
                        }
                        break;
                case TAC_MOVE: fprintf(fout, "## TAC_MOVE\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                        break;
                case TAC_ADD: fprintf(fout, "## TAC_ADD\n"
                                                "\tmovl\t_%s(%%rip), %%edx\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\taddl\t%%edx, %%eax\n"
                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->op2->text, tac->res->text);
                        break;
                case TAC_SUB: fprintf(fout, "## TAC_SUB\n"
                                                "\tmovl\t_%s(%%rip), %%edx\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\tsubl\t%%eax, %%edx\n"
                                                "\tmovl\t%%edx, _%s(%%rip)\n", tac->op1->text, tac->op2->text, tac->res->text);
                        break;
                case TAC_MUL: fprintf(fout, "## TAC_MUL\n"
                                                "\tmovl\t_%s(%%rip), %%edx\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\timull\t%%edx, %%eax\n"
                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->op2->text, tac->res->text);
                        break;
                case TAC_DIV: fprintf(fout, "## TAC_DIV\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\tmovl\t_%s(%%rip), %%ecx\n"
                                                "\tcltd\n"
                                                "\tidivl\t%%ecx\n"
                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->op2->text, tac->res->text);
                        break;
                case TAC_IFZ: fprintf(fout, "## TAC_IFZ\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\ttestl\t%%eax, %%eax\n"
                                                "\tje\t.%s\n", tac->op1->text, tac->res->text);
                        break;
                case TAC_LABEL: fprintf(fout, "## TAC_LABEL\n"
                                                ".%s:\n", tac->res->text);
                        break;
                case TAC_JUMP: fprintf(fout, "## TAC_JUMP\n"
                                                "\tjmp\t.%s\n", tac->res->text);
                        break;
                case TAC_EQ: fprintf(fout, "## TAC_EQ\n"
                                                "\tmovl\t_%s(%%rip), %%edx\n"
                                        	"\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\tcmpl\t%%eax, %%edx\n"
                                        	"\tsete\t%%al\n"
                                        	"\tmovzbl\t%%al, %%eax\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->op2->text, tac->res->text);
                        break;
                case TAC_FUNCALL: fprintf(fout, "## TAC_FUNCALL\n"
                                                "\tmovl\t$0, %%eax\n"
                                                "\tcall\t%s\n", tac->op1->text);
                        break;
                case TAC_ARG:{
                        AST* fundecl = findFunctionDeclaration(getRootAST(), tac->op1->text);
                        HASH_NODE* param = findParam(fundecl->son[1], 1, atoi(tac->op2->text));
                        fprintf(fout, "## TAC_ARG\n"
                                        "\tmovl\t_%s(%%rip), %%eax\n"
                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text, param->text);
                }
                        break;
                default:
                        break;
        }
}

HASH_NODE* findParam(AST* paramList, int currIteration, int n){
        if(!paramList) return 0;

        if(currIteration == n)
                return paramList->son[0]->symbol;
        else
                return findParam(paramList->son[1], currIteration+1, n);
}
