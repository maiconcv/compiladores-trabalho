#include "tacs.h"

// INTERNAL PROTOTYPES
TAC* makeBinOp(int type, TAC* code0, TAC* code1);
TAC* makeIfThen(TAC* code0, TAC* code1);

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
                case AST_IF: return makeIfThen(code[0], code[1]);
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
