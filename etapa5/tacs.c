#include "tacs.h"

// INTERNAL PROTOTYPES
TAC* makeBinOp(int type, TAC* code0, TAC* code1);
TAC* makeIfThen(TAC* code0, TAC* code1);
TAC* makeIfThenElse(TAC* code0, TAC* code1, TAC* code2);

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

TAC* generateCode(AST* ast){
        if(!ast) return 0;

        TAC* code[MAX_SONS];

        // generate code for my sons first
        for(int i = 0; i < MAX_SONS; ++i)
                code[i] = generateCode(ast->son[i]);

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