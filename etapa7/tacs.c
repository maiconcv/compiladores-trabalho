#include "tacs.h"

typedef struct operands{
        char* op1;
        char* op2;
} OPERANDS;

int vectorSizeFlag = 0;
int vectorDatatype = 0;
int funcallDatatype = 0;

HASH_NODE* forVariable = 0;
int forInitValue = 0;
int forLimitValue = 0;
int forIncValue = 0;
int forBeginFlag = 0;
int forCurrIndex = 0;
int tacForBeginFound = 0;

AST* getRootAST();

// INTERNAL PROTOTYPES
TAC* makeBinOp(int type, TAC* code0, TAC* code1);
TAC* makeIfThen(TAC* code0, TAC* code1);
TAC* makeIfThenElse(TAC* code0, TAC* code1, TAC* code2);
TAC* makeWhile(TAC* code0, TAC* code1, HASH_NODE* labelLeaveWhile);
TAC* makeFor(HASH_NODE* var, TAC* code0, TAC* code1, TAC* code2, TAC* code3, HASH_NODE* labelLeaveFor);
HASH_NODE* findParam(AST* paramList, int currIteration, int n);
char* findLitCharFullVarName(char* text, const char* varName);
OPERANDS fillOperands(HASH_NODE* op1, HASH_NODE* op2);
void loadVarsIntoCorrectRegisters(FILE* fout, HASH_NODE* op1, HASH_NODE* op2, OPERANDS op);

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
                case TAC_FORINIT: fprintf(stderr, "TAC_FORINIT"); break;
                case TAC_FORBEGIN: fprintf(stderr, "TAC_FORBEGIN"); break;
                case TAC_FOREND: fprintf(stderr, "TAC_FOREND"); break;
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

TAC* generateCode(AST* ast, HASH_NODE* funCallName, int funArgCounter, HASH_NODE* labelLeaveFlowControlCmd){
        if(!ast) return 0;

        TAC* code[MAX_SONS];

        HASH_NODE* currFunCall = 0;
        int currArgCounter = 0;
        HASH_NODE* currArgCounterSymbol = 0;

        //HASH_NODE* labelLeaveFlowControlCmd = 0;
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
        else if(ast->type == AST_WHILE || ast->type == AST_FOR){
                labelLeaveFlowControlCmd = makeLabel();
                //fprintf(stderr, "label criada %s\n", labelLeaveFlowControlCmd->text);
        }

        // generate code for my sons first
        for(int i = 0; i < MAX_SONS; ++i)
                code[i] = generateCode(ast->son[i], currFunCall, currArgCounter, labelLeaveFlowControlCmd);

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
                case AST_WHILE: return makeWhile(code[0], code[1], labelLeaveFlowControlCmd);
                        break;
                case AST_FOR: return makeFor(ast->symbol, code[0], code[1], code[2], code[3], labelLeaveFlowControlCmd);
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
                case AST_VECTDECL:{
                        HASH_NODE* hasInitValues;
                        if(code[2] == 0)
                                hasInitValues = hashInsert("0", SYMBOL_LITINT, 0);
                        else
                                hasInitValues = hashInsert("1", SYMBOL_LITINT, 0);
                        return tacJoin(tacJoin(code[1], tacCreate(TAC_VECTDECL, ast->symbol, code[1]?code[1]->res:0, hasInitValues)), code[2]);
                }
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
                case AST_VECTREAD:{
                        HASH_NODE* result = makeTemp();
                        result->datatype = ast->symbol->datatype;
                        return tacJoin(code[0], tacCreate(TAC_VECTREAD, result, ast->symbol, code[0]?code[0]->res:0));
                }
                        break;
                case AST_BRACKETS: return code[0];
                        break;
                case AST_LCMD: return tacJoin(code[0], code[1]);
                        break;
                case AST_LDECL: return tacJoin(code[0], code[1]);
                        break;
                case AST_FUNCALL:{
                        HASH_NODE* temp = makeTemp();
                        HASH_NODE* func = hashFind(ast->symbol->text);
                        temp->datatype = func->datatype;
                        return tacJoin(code[0], tacCreate(TAC_FUNCALL, temp, ast->symbol, 0));
                }
                        break;
                case AST_FUNARG: return tacJoin(tacJoin(code[0], tacCreate(TAC_ARG, code[0]?code[0]->res:0, currFunCall, currArgCounterSymbol)), code[1]);
                        break;
                case AST_BREAK: return tacCreate(TAC_BREAK, labelLeaveFlowControlCmd, 0, 0);
                        break;
                default: return tacJoin(tacJoin(tacJoin(code[0], code[1]), code[2]), code[3]);
                        break;
        }
        return 0;
}

TAC* makeBinOp(int type, TAC* code0, TAC* code1){
        HASH_NODE* temp = makeTemp();
        if(type == TAC_ADD || type == TAC_SUB || type == TAC_MUL || type == TAC_DIV)
                temp->datatype = DATATYPE_FLOAT;
        return tacJoin(tacJoin(code0, code1), tacCreate(type, temp, code0?code0->res:0, code1?code1->res:0));
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

TAC* makeWhile(TAC* code0, TAC* code1, HASH_NODE* labelLeaveWhile){
        HASH_NODE* labelBeforeWhile = makeLabel();
        //HASH_NODE* labelLeaveWhile = makeLabel();
        TAC* tacif = tacCreate(TAC_IFZ, labelLeaveWhile, code0?code0->res:0, 0);
        TAC* tacjump = tacCreate(TAC_JUMP, labelBeforeWhile, 0, 0);
        TAC* taclabelBeforeWhile = tacCreate(TAC_LABEL, labelBeforeWhile, 0, 0);
        TAC* taclabelLeaveWhile = tacCreate(TAC_LABEL, labelLeaveWhile, 0, 0);

        return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(taclabelBeforeWhile, code0), tacif), code1), tacjump), taclabelLeaveWhile);
}

TAC* makeFor(HASH_NODE* var, TAC* code0, TAC* code1, TAC* code2, TAC* code3, HASH_NODE* labelLeaveFor){
        TAC* tacForInit = tacCreate(TAC_FORINIT, var, code0->res, code1->res);
        TAC* tacForBegin = tacCreate(TAC_FORBEGIN, code2->res, 0, 0);
        TAC* tacForEnd = tacCreate(TAC_FOREND, 0, 0, 0);

        return tacJoin(tacJoin(tacJoin(tacForInit, tacForBegin), code3), tacForEnd);
}

void tacPrintForwards(TAC* tac){
        if(!tac) return;

        tacPrintForwards(tac->prev);
        tacPrintSingle(tac);
}

void generateASM(TAC* tac, FILE* fout){
        static int functionCounter = 0;
        static int logicOpLabelCounter = 0;

        if(!tac) return;

        if(forBeginFlag == 1 && tac->type == TAC_FORINIT){
                goto JUMP_RECURSION;
        }

        if(tac->prev)
                generateASM(tac->prev, fout);

        JUMP_RECURSION:
        if(forBeginFlag == 0);
                //fprintf(stderr, "for nao iniciado, entrar no switch! %d\n", tac->type);
        else if(forCurrIndex == -1){
                if(tac->type == TAC_FOREND);
                        //fprintf(stderr, "primeiro for end, entra pra colocar indice em 0! %d\n", tac->type);
                else{
                        //fprintf(stderr, "nao chegou primeiro for end, pulando! %d\n", tac->type);
                        return;
                }
        }
        else if(tac->type == TAC_FORBEGIN){
                //fprintf(stderr, "segundo for begin, pular pra nao resetar! %d\n", tac->type);
                tacForBeginFound = 1;
                return;
        }
        else if(tacForBeginFound == 1);
                //fprintf(stderr, "encontrei for begin, comandos dentro do for, entrando! %d\n", tac->type);
        else{
                //fprintf(stderr, "comandos fora do for, pulando! %d\n", tac->type);
                return;
        }

        switch (tac->type) {
                case TAC_VARDECL:{
                        HASH_NODE* var = hashFind(tac->res->text);
                        switch (var->datatype) {
                                case DATATYPE_INT:
                                case DATATYPE_LONG:
                                        fprintf(fout, "\n\t.data\n"
                                                        "_%s:\t.long\t%s\n", tac->res->text, tac->op1->text);
                                        break;
                                case DATATYPE_BYTE:
                                        fprintf(fout, "\n\t.data\n"
                                                        "_%s:\t.long\t%d\n", tac->res->text, (int)(tac->op1->text[1]));
                                        break;
                                case DATATYPE_BOOL:{
                                        int value = 0;
                                        if(strcmp(tac->op1->text, "TRUE") == 0)
                                                value = 1;
                                        fprintf(fout, "\n\t.data\n"
                                                        "_%s:\t.long\t%d\n", tac->res->text, value);
                                }
                                        break;
                                case DATATYPE_FLOAT:{
                                        int num = floatToBinaryToInt(tac->op1->text);
                                        fprintf(fout, "\n\t.data\n"
                                                        "_%s:\t.long\t%d\n", tac->res->text, num);
                                }
                                        break;
                                default:
                                        break;
                        }
                }
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
                                                /*"\tmovl\t$0, %%eax\n"*/
                                                "\tpopq\t%%rbp\n"
                                                "\tret\n"
                                                ".LFE%d:\n"
                                                "\t.size\t%s, .-%s\n", functionCounter, tac->res->text, tac->res->text);
                        functionCounter++;
                        break;
                case TAC_PRINT:
                        if(forBeginFlag && tac->res->type == SYMBOL_SCALAR && strcmp(tac->res->text, forVariable->text) == 0){
                                fprintf(fout, "## TAC_PRINT_VAR\n"
                                                "\tmovl\t$%d, %%esi\n"
                                                "\tleaq\tLC0(%%rip), %%rdi\n"
                                                "\tcall\tprintf@PLT\n", forCurrIndex);
                        }
                        else{
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
                                case SYMBOL_LITINT: fprintf(fout, "## TAC_PRINT_LITINT\n"
                                                                "\tmovl\t_%s(%%rip), %%esi\n"
                                                                "\tleaq\tLC0(%%rip), %%rdi\n"
                                                                "\tcall\tprintf@PLT\n", tac->res->text);
                                        break;
                                case SYMBOL_LITBOOL: fprintf(fout, "## TAC_PRINT_LITBOOL\n"
                                                                "\tleaq\t%s(%%rip), %%rdi\n"
                                                                "\tcall\tprintf@PLT\n", tac->res->text);
                                        break;
                                case SYMBOL_LITREAL:{
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "## TAC_PRINT_LITREAL\n"
                                                        "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tcvtss2sd\t%%xmm0, %%xmm0\n"
                                                        "\tleaq\tLC1(%%rip), %%rdi\n"
                                                        "\tmovl\t$1, %%eax\n"
                                                        "\tcall\tprintf@PLT\n", LITFLOAT_VAR_NAME, counter);
                                }
                                        break;
                                case SYMBOL_TEMP:
                                case SYMBOL_SCALAR:{
                                        HASH_NODE* var = hashFind(tac->res->text);
                                        switch (var->datatype) {
                                                case DATATYPE_INT:
                                                case DATATYPE_LONG:
                                                        fprintf(fout, "## TAC_PRINT_VAR\n"
                                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                                "\tmovl\t%%eax, %%esi\n"
                                                                "\tleaq\tLC0(%%rip), %%rdi\n"
                                                                "\tmovl\t$0, %%eax\n"
                                                                "\tcall\tprintf@PLT\n", tac->res->text);
                                                        break;
                                                case DATATYPE_BYTE:
                                                        fprintf(fout, "## TAC_PRINT_VAR_CHAR\n"
                                                                        "\tmovzbl\t_%s(%%rip), %%eax\n"
                                                                        "\tmovsbl\t%%al, %%eax\n"
                                                                        "\tmovl\t%%eax, %%edi\n"
                                                                        "\tcall\tputchar@PLT\n", tac->res->text);
                                                        break;
                                                case DATATYPE_BOOL:
                                                        fprintf(fout, "## TAC_PRINT_VAR_BOOL\n"
                                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                                "\ttestl\t%%eax, %%eax\n"
                                                                "\tje\t.%s%d\n"
                                                                "\tleaq\tTRUE(%%rip), %%rdi\n"
                                                                "\tcall\tprintf@PLT\n"
                                                                "\tjmp\t.%s%d\n"
                                                                ".%s%d:\n"
                                                                "\tleaq\tFALSE(%%rip), %%rdi\n"
                                                                "\tcall\tprintf@PLT\n"
                                                                ".%s%d:\n", tac->res->text,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1);
                                                        logicOpLabelCounter += 2;
                                                        break;
                                                case DATATYPE_FLOAT:
                                                        fprintf(fout, "## TAC_PRINT_VAR_FLOAT\n"
                                                                        "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                                	"\tcvtss2sd\t%%xmm0, %%xmm0\n"
                                                                	"\tleaq\tLC1(%%rip), %%rdi\n"
                                                                	"\tmovl\t$1, %%eax\n"
                                                                	"\tcall\tprintf@PLT\n", tac->res->text);
                                                        break;
                                        }
                                }
                                        break;
                        }
                        }
                        break;
                case TAC_MOVE:{
                        fprintf(fout, "## TAC_MOVE\n");
                        if(forBeginFlag == 1 && tac->op1->type == SYMBOL_SCALAR && strcmp(tac->op1->text, forVariable->text) == 0){
                                fprintf(fout, "\tmovl\t$%d, _%s(%%rip)\n", forCurrIndex, tac->res->text);
                        }
                        else if(tac->op1->type == SYMBOL_LITCHAR){ // case var = litchar;
                                int counter = findCounter(tac->op1->text);
                                if(tac->res->datatype == DATATYPE_FLOAT){ // case floatVar = litchar;
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", LITCHAR_VAR_NAME, counter, tac->res->text);
                                }
                                else{ // case nonfloatVar = litchar;
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", LITCHAR_VAR_NAME, counter, tac->res->text);
                                }
                        }
                        else if(tac->op1->type == SYMBOL_LITREAL){
                                int counter = findCounter(tac->op1->text);
                                if(tac->res->datatype == DATATYPE_FLOAT){ // case floatVar = litreal;
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", LITFLOAT_VAR_NAME, counter, tac->res->text);
                                }
                                else{ // case nonfloatVar = litreal;
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tcvttss2si\t%%xmm0, %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", LITFLOAT_VAR_NAME, counter, tac->res->text);
                                }
                        }
                        else if(tac->op1->type == SYMBOL_SCALAR && tac->op1->datatype == DATATYPE_BYTE){ // case var = byteVar;
                                if(tac->res->datatype == DATATYPE_FLOAT){ // case floatVar = byteVar;
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                }
                                else{ // case nonfloatVar = byteVar;
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                }
                        }
                        else{ // any other case
                                if(tac->res->datatype == DATATYPE_FLOAT){ // case floatVar = any;
                                        if(tac->op1->datatype == DATATYPE_FLOAT){ // case floatVar = floatData; (var, temp)
                                                fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                                "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                        }
                                        else{ // case floatVar = nonfloatData;
                                                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                                "\tcvtsi2ss\t%%eax, %%xmm0\n"
                                                                "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                        }
                                }
                                else{
                                        if(tac->op1->datatype == DATATYPE_FLOAT){ // case nonFloatVar = floatData; (lit, var, temp)
                                                fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                                "\tcvttss2si\t%%xmm0, %%eax\n"
                                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                        }
                                        else{ // case nonFloatVar = any;
                                                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                                "\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                                        }
                                }
                        }
                }
                        break;
                case TAC_ADD:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_ADD\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\taddss\t%%xmm1, %%xmm0\n"
                	               "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_SUB:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_SUB\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tsubss\t%%xmm1, %%xmm0\n"
                	               "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_MUL:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_MUL\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tmulss\t%%xmm1, %%xmm0\n"
                	               "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_DIV:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_DIV\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tdivss\t%%xmm1, %%xmm0\n"
                	               "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_IFZ:
                        if(forBeginFlag == 1){
                                fprintf(fout, "## TAC_IFZ\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\ttestl\t%%eax, %%eax\n"
                                                "\tje\t.%s_%d\n", tac->op1->text, tac->res->text, logicOpLabelCounter);
                        }
                        else{
                                fprintf(fout, "## TAC_IFZ\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\ttestl\t%%eax, %%eax\n"
                                                "\tje\t.%s\n", tac->op1->text, tac->res->text);
                        }
                        break;
                case TAC_LABEL:
                        if(forBeginFlag == 1){
                                fprintf(fout, "## TAC_LABEL\n"
                                                ".%s_%d:\n", tac->res->text, logicOpLabelCounter);
                                logicOpLabelCounter++;
                        }
                        else{
                                fprintf(fout, "## TAC_LABEL\n"
                                                ".%s:\n", tac->res->text);
                        }
                        break;
                case TAC_JUMP:
                        if(forBeginFlag == 1){
                                fprintf(fout, "## TAC_JUMP\n"
                                                "\tjmp\t.%s_%d\n", tac->res->text, logicOpLabelCounter+1);
                        }
                        else{
                                fprintf(fout, "## TAC_JUMP\n"
                                                "\tjmp\t.%s\n", tac->res->text);
                        }
                        break;
                case TAC_EQ:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_EQ\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm1, %%xmm0\n"
                	               "\tsetnp\t%%al\n"
                	               "\tmovl\t$0, %%edx\n"
                	               "\tucomiss\t%%xmm1, %%xmm0\n"
                	               "\tcmovne\t%%edx, %%eax\n"
                	               "\tmovzbl\t%%al, %%eax\n"
                	               "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_DIF:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_DIF\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm1, %%xmm0\n"
                	               "\tsetp\t%%al\n"
                	               "\tmovl\t$1, %%edx\n"
                	               "\tucomiss\t%%xmm1, %%xmm0\n"
                	               "\tcmovne\t%%edx, %%eax\n"
                	               "\tmovzbl\t%%al, %%eax\n"
                	               "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_LT:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_LT\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm0, %%xmm1\n"
                                        "\tseta\t%%al\n"
                                        "\tmovzbl\t%%al, %%eax\n"
                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_GT:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_GT\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm1, %%xmm0\n"
                                        "\tseta\t%%al\n"
                                        "\tmovzbl\t%%al, %%eax\n"
                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_LE:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_LE\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm0, %%xmm1\n"
                                        "\tsetnb\t%%al\n"
                                        "\tmovzbl\t%%al, %%eax\n"
                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_GE:{
                        OPERANDS op = fillOperands(tac->op1, tac->op2);
                        fprintf(fout, "## TAC_GT\n");
                        loadVarsIntoCorrectRegisters(fout, tac->op1, tac->op2, op);
                        fprintf(fout, "\tucomiss\t%%xmm1, %%xmm0\n"
                                        "\tsetnb\t%%al\n"
                                        "\tmovzbl\t%%al, %%eax\n"
                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                }
                        break;
                case TAC_AND: fprintf(fout, "## TAC_AND\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                                "\ttestl\t%%eax, %%eax\n"
                                                "\tje\t.%s%d\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\ttestl\t%%eax, %%eax\n"
                                        	"\tje\t.%s%d\n"
                                        	"\tmovl\t$1, %%eax\n"
                                        	"\tjmp\t.%s%d\n"
                                                ".%s%d:\n"
                                        	"\tmovl\t$0, %%eax\n"
                                                ".%s%d:\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                tac->op2->text,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1,
                                                                                tac->res->text);
                        logicOpLabelCounter += 2;
                        break;
                case TAC_OR: fprintf(fout, "## TAC_OR\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\ttestl\t%%eax, %%eax\n"
                                        	"\tjne\t.%s%d\n"
                                        	"\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\ttestl\t%%eax, %%eax\n"
                                        	"\tje\t.%s%d\n"
                                                ".%s%d:\n"
                                        	"\tmovl\t$1, %%eax\n"
                                        	"\tjmp\t.%s%d\n"
                                                ".%s%d:\n"
                                        	"\tmovl\t$0, %%eax\n"
                                                ".%s%d:\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                tac->op2->text,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+2,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+1,
                                                                                LABEL_LOGIC_OP, logicOpLabelCounter+2,
                                                                                tac->res->text);
                        logicOpLabelCounter += 3;
                        break;
                case TAC_NOT: fprintf(fout, "## TAC_NOT\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\ttestl\t%%eax, %%eax\n"
                                        	"\tsete\t%%al\n"
                                        	"\tmovzbl\t%%al, %%eax\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", tac->op1->text, tac->res->text);
                        break;
                case TAC_FUNCALL:{
                        HASH_NODE* func = hashFind(tac->op1->text);
                        funcallDatatype = func->datatype;

                        fprintf(fout, "## TAC_FUNCALL\n"
                                        "\tmovl\t$0, %%eax\n"
                                        "\tcall\t%s\n", tac->op1->text);
                        if(funcallDatatype == DATATYPE_FLOAT){
                                fprintf(fout, "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text);
                        }
                        else{
                                fprintf(fout, "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text);
                        }
                }
                        break;
                case TAC_ARG:{
                        AST* fundecl = findFunctionDeclaration(getRootAST(), tac->op1->text);
                        HASH_NODE* param = findParam(fundecl->son[1], 1, atoi(tac->op2->text));

                        fprintf(fout, "## TAC_ARG\n");
                        if(forBeginFlag == 1 && tac->res->type == SYMBOL_SCALAR && strcmp(tac->res->text, forVariable->text) == 0){
                                fprintf(fout, "\tmovl\t$%d, _%s(%%rip)\n", forCurrIndex, param->text);
                        }
                        else if(param->datatype == DATATYPE_FLOAT){
                                if(tac->res->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", LITFLOAT_VAR_NAME, counter, param->text);
                                }
                                else if(tac->res->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", LITCHAR_VAR_NAME, counter, param->text);
                                }
                                else if(tac->res->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text, param->text);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n"
                                                        "\tmovss\t%%xmm0, _%s(%%rip)\n", tac->res->text, param->text);
                                }
                        }
                        else{
                                if(tac->res->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tcvttss2si\t%%xmm0, %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", LITFLOAT_VAR_NAME, counter, param->text);
                                }
                                else if(tac->res->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", LITCHAR_VAR_NAME, counter, param->text);
                                }
                                else if(tac->res->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                        "\tcvttss2si\t%%xmm0, %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text, param->text);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tmovl\t%%eax, _%s(%%rip)\n", tac->res->text, param->text);
                                }
                        }
                }
                        break;
                case TAC_RETURN:{
                        fprintf(fout, "## TAC_RETURN\n");
                        if(funcallDatatype == DATATYPE_FLOAT){
                                if(tac->res->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n", LITCHAR_VAR_NAME, counter);
                                }
                                else if(tac->res->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n", LITFLOAT_VAR_NAME, counter);
                                }
                                else if(tac->res->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n", tac->res->text);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                                        "\tcvtsi2ss\t%%eax, %%xmm0\n", tac->res->text);
                                }
                        }
                        else{
                                if(tac->res->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n", LITCHAR_VAR_NAME, counter);
                                }
                                else if(tac->res->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->res->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                        "\tcvttss2si\t%%xmm0, %%eax\n", LITFLOAT_VAR_NAME, counter);
                                }
                                else if(tac->res->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                        "\tcvttss2si\t%%xmm0, %%eax\n", tac->res->text);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac->res->text);
                                }
                        }

                }
                        break;
                case TAC_BREAK: if(tac->res != 0){
                        fprintf(fout, "## TAC_BREAK\n"
                                        "\tjmp\t.%s\n", tac->res->text);
                }
                        break;
                case TAC_READ: fprintf(fout, "## TAC_READ\n"
                                                "\tleaq\t_%s(%%rip), %%rsi\n", tac->res->text);
                        if(tac->res->datatype == DATATYPE_FLOAT){
                                fprintf(fout, "\tleaq\tLC1(%%rip), %%rdi\n");
                        }
                        else if(tac->res->datatype == DATATYPE_BYTE){
                                fprintf(fout, "\tleaq\tLC2(%%rip), %%rdi\n");
                        }
                        else{
                                fprintf(fout, "\tleaq\tLC0(%%rip), %%rdi\n");
                        }
                        fprintf(fout, "\tmovl\t$0, %%eax\n"
                                        "\tcall\t__isoc99_scanf@PLT\n");
                        break;
                case TAC_VECTDECL:{
                        int hasInitValues = atoi(tac->op2->text);
                        if(hasInitValues){
                                fprintf(fout, "\n\t.data\n"
                                                "_%s:\n", tac->res->text);
                                vectorSizeFlag = atoi(tac->op1->text);
                                vectorDatatype = tac->res->datatype;
                        }
                        else{
                                fprintf(fout, "\n\t.data\n"
                                                "\t.comm\t_%s,%s\n", tac->res->text, tac->op1->text);
                        }
                }
                        break;
                case TAC_SYMBOL:
                        if(vectorSizeFlag > 0){
                                if(vectorDatatype == DATATYPE_FLOAT){
                                        int num = floatToBinaryToInt(tac->res->text);
                                        fprintf(fout, "\t.long\t%d\n", num);
                                }
                                else
                                        fprintf(fout, "\t.long\t%s\n", tac->res->text);
                                vectorSizeFlag--;
                        }
                        break;
                case TAC_VECTREAD:
                        if(forBeginFlag == 1 && strcmp(tac->op2->text, forVariable->text) == 0){
                                fprintf(fout, "## TAC_VECTREAD\n"
                                                "\tmovl\t$%d, %%eax\n"
                                                "\tcltq\n"
                                                "\tleaq\t0(,%%rax,4), %%rdx\n"
                                                "\tleaq\t_%s(%%rip), %%rax\n"
                                        	"\tmovl\t(%%rdx,%%rax), %%eax\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", forCurrIndex, tac->op1->text, tac->res->text);
                        }
                        else
                                fprintf(fout, "## TAC_VECTREAD\n"
                                                "\tmovl\t_%s(%%rip), %%eax\n"
                                        	"\tcltq\n"
                                        	"\tleaq\t0(,%%rax,4), %%rdx\n"
                                        	"\tleaq\t_%s(%%rip), %%rax\n"
                                        	"\tmovl\t(%%rdx,%%rax), %%eax\n"
                                        	"\tmovl\t%%eax, _%s(%%rip)\n", tac->op2->text, tac->op1->text, tac->res->text); // 4 for int, other types may not be 4
                        break;
                case TAC_MOVEVECT:{
                        fprintf(fout, "## TAC_MOVEVECT\n");

                        // index
                        if(forBeginFlag && strcmp(tac->op1->text, forVariable->text) == 0){
                                fprintf(fout, "\tmovl\t$%d, %%eax\n", forCurrIndex);
                        }
                        else if(tac->op1->type == SYMBOL_LITREAL){
                                int counter = findCounter(tac->op1->text);
                                fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm0\n"
                                                "\tcvttss2si\t%%xmm0, %%eax\n", LITFLOAT_VAR_NAME, counter);
                        }
                        else if((tac->op1->type == SYMBOL_SCALAR || tac->op1->type == SYMBOL_TEMP) && tac->op1->datatype == DATATYPE_FLOAT){
                                fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n"
                                                "\tcvttss2si\t%%xmm0, %%eax\n", tac->op1->text);
                        }
                        else if(tac->op1->type == SYMBOL_LITCHAR){
                                int counter = findCounter(tac->op1->text);
                                fprintf(fout, "\tmovl\t_%s%d(%%rip), %%eax\n", LITCHAR_VAR_NAME, counter);
                        }
                        else{
                                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n", tac->op1->text);
                        }

                        // value
                        if(forBeginFlag == 1 && strcmp(tac->op2->text, forVariable->text) == 0){
                                fprintf(fout, "\tmovl\t$%d, %%edx\n"
                                                "\tcltq\n"
                                                "\tleaq\t0(,%%rax,4), %%rcx\n"
                                                "\tleaq\t_%s(%%rip), %%rax\n"
                                                "\tmovl\t%%edx, (%%rcx,%%rax)\n", forCurrIndex, tac->res->text);
                        }
                        else if(tac->res->datatype == DATATYPE_FLOAT){
                                if(tac->op2->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->op2->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm1\n", LITFLOAT_VAR_NAME, counter);
                                }
                                else if((tac->op2->type == SYMBOL_SCALAR || tac->op2->type == SYMBOL_TEMP) && tac->op2->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm1\n", tac->op2->text);
                                }
                                else if(tac->op2->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->op2->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%edx\n"
                                                        "\tcvtsi2ss\t%%edx, %%xmm1\n", LITCHAR_VAR_NAME, counter);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%edx\n"
                                                        "\tcvtsi2ss\t%%edx, %%xmm1\n", tac->op2->text);
                                }

                                fprintf(fout, "\tcltq\n"
                        	               "leaq\t0(,%%rax,4), %%rdx\n"
                        	               "leaq\t_%s(%%rip), %%rax\n"
                        	               "\tmovss\t%%xmm1, (%%rdx,%%rax)\n", tac->res->text);
                        }
                        else{
                                if(tac->op2->type == SYMBOL_LITREAL){
                                        int counter = findCounter(tac->op2->text);
                                        fprintf(fout, "\tmovss\t_%s%d(%%rip), %%xmm1\n"
                                                        "\tcvttss2si\t%%xmm1, %%edx\n", LITFLOAT_VAR_NAME, counter);
                                }
                                else if((tac->op2->type == SYMBOL_SCALAR || tac->op2->type == SYMBOL_TEMP) && tac->op2->datatype == DATATYPE_FLOAT){
                                        fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm1\n"
                                                        "\tcvttss2si\t%%xmm1, %%edx\n", tac->op2->text);
                                }
                                else if(tac->op2->type == SYMBOL_LITCHAR){
                                        int counter = findCounter(tac->op2->text);
                                        fprintf(fout, "\tmovl\t_%s%d(%%rip), %%edx\n", LITCHAR_VAR_NAME, counter);
                                }
                                else{
                                        fprintf(fout, "\tmovl\t_%s(%%rip), %%edx\n", tac->op2->text);
                                }

                                fprintf(fout, "\tcltq\n"
                                                "\tleaq\t0(,%%rax,4), %%rcx\n"
                                                "\tleaq\t_%s(%%rip), %%rax\n"
                                                "\tmovl\t%%edx, (%%rcx,%%rax)\n", tac->res->text);
                        }
                }
                        break;
                case TAC_FORINIT:
                        forVariable = tac->res;
                        forInitValue = atoi(tac->op1->text);
                        forLimitValue = atoi(tac->op2->text);
                        break;
                case TAC_FORBEGIN:
                        forIncValue = atoi(tac->res->text);
                        forBeginFlag = 1;
                        forCurrIndex = -1;
                        tacForBeginFound = 1;
                        break;
                case TAC_FOREND:
                        if(forCurrIndex != -1)
                                forCurrIndex += forIncValue;
                        else
                                forCurrIndex = forInitValue;

                        tacForBeginFound = 0;

                        fprintf(fout, "\tmovl\t$%d, _%s(%%rip)\n", forCurrIndex, forVariable->text);

                        if(forCurrIndex < forLimitValue)
                                generateASM(tac, fout);
                        else{
                                fprintf(fout, "## ACABOU O FOR!\n");

                                forVariable = 0;
                                forInitValue = 0;
                                forLimitValue = 0;
                                forIncValue = 0;
                                forBeginFlag = 0;
                                forCurrIndex = 0;
                                tacForBeginFound = 0;
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

char* findLitValueFullVarName(char* text, const char* varName){
        int counter = findCounter(text);
        char* varFullName = (char*)malloc(sizeof(varName) + sizeof(counter));
        strcpy(varFullName, varName);

        char num[10];
        sprintf(num, "%d", counter);
        strcat(varFullName, num);

        return varFullName;
}

OPERANDS fillOperands(HASH_NODE* op1, HASH_NODE* op2){
        OPERANDS op;
        if(op1->type == SYMBOL_LITCHAR)
                op.op1 = findLitValueFullVarName(op1->text, LITCHAR_VAR_NAME);
        else if(op1->type == SYMBOL_LITREAL)
                op.op1 = findLitValueFullVarName(op1->text, LITFLOAT_VAR_NAME);
        else
                op.op1 = op1->text;

        if(op2->type == SYMBOL_LITCHAR)
                op.op2 = findLitValueFullVarName(op2->text, LITCHAR_VAR_NAME);
        else if(op2->type == SYMBOL_LITREAL)
                op.op2 = findLitValueFullVarName(op2->text, LITFLOAT_VAR_NAME);
        else
                op.op2 = op2->text;

        return op;
}

void loadVarsIntoCorrectRegisters(FILE* fout, HASH_NODE* op1, HASH_NODE* op2, OPERANDS op){
        int op1Done = 0, op2Done = 0;

        if(forBeginFlag == 1 && op1->type == SYMBOL_SCALAR && strcmp(op1->text, forVariable->text) == 0){
                fprintf(fout, "\tmovl\t$%d, %%eax\n"
                                "\tcvtsi2ss\t%%eax, %%xmm0\n", forCurrIndex);
                op1Done = 1;
        }
        if(forBeginFlag == 1 && op2->type == SYMBOL_SCALAR && strcmp(op2->text, forVariable->text) == 0){
                fprintf(fout, "\tmovl\t$%d, %%edx\n"
                                "\tcvtsi2ss\t%%edx, %%xmm1\n", forCurrIndex);
                op2Done = 1;
        }

        if(op1Done == 1 && op2Done == 1)
                return;
        else if(op1Done == 1)
                goto OP1DONE;

        if(op1->type == SYMBOL_LITREAL ||
           ((op1->type == SYMBOL_SCALAR || op1->type == SYMBOL_TEMP) && op1->datatype == DATATYPE_FLOAT)){
                fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm0\n", op.op1);
        }
        else{
                fprintf(fout, "\tmovl\t_%s(%%rip), %%eax\n"
                                "\tcvtsi2ss\t%%eax, %%xmm0\n", op.op1);
        }

        if(op2Done == 1)
                return;

        OP1DONE:
        if(op2->type == SYMBOL_LITREAL ||
           ((op2->type == SYMBOL_SCALAR || op2->type == SYMBOL_TEMP) && op2->datatype == DATATYPE_FLOAT)){
                fprintf(fout, "\tmovss\t_%s(%%rip), %%xmm1\n", op.op2);
        }
        else{
                fprintf(fout, "\tmovl\t_%s(%%rip), %%edx\n"
                                "\tcvtsi2ss\t%%edx, %%xmm1\n", op.op2);
        }
}
