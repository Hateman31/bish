#include <iostream>
#include <cassert>
#include "IR.h"
#include "TypeChecker.h"

using namespace Bish;

void TypeChecker::visit(ReturnStatement *node) {
    if (node->type() != UndefinedTy) return;
    node->value->accept(this);
    node->set_type(node->value->type());
    assert(node->parent()->type() == UndefinedTy);
    propagate_if_undef(node->parent(), node);
}

void TypeChecker::visit(Function *node) {
    if (node->type() != UndefinedTy) return;
    for (std::vector<Variable *>::const_iterator I = node->args.begin(),
             E = node->args.end(); I != E; ++I) {
        (*I)->accept(this);
    }
    if (node->body) node->body->accept(this);
}

void TypeChecker::visit(FunctionCall *node) {
    if (node->type() != UndefinedTy) return;
    node->function->accept(this);
    node->set_type(node->function->type());
}

void TypeChecker::visit(ExternCall *node) {
    if (node->type() != UndefinedTy) return;
    node->set_type(UndefinedTy);
}

void TypeChecker::visit(Assignment *node) {
    if (node->type() != UndefinedTy) return;
    node->variable->accept(this);
    node->value->accept(this);
    if (node->variable->type() != UndefinedTy && node->value->type() != UndefinedTy) {
        assert(node->variable->type() == node->value->type() &&
               "Invalid type in assignment.");
    } else {
        node->variable->set_type(node->value->type());
    }
    node->set_type(node->variable->type());
}

void TypeChecker::visit(BinOp *node) {
    if (node->type() != UndefinedTy) return;
    node->a->accept(this);
    node->b->accept(this);
    propagate_if_undef(node->a, node->b);
    assert(node->a->type() == node->b->type());
    node->set_type(node->a->type());
}

void TypeChecker::visit(UnaryOp *node) {
    node->a->accept(this);
    node->set_type(node->a->type());
}

void TypeChecker::visit(Integer *node) {
    node->set_type(IntegerTy);
}

void TypeChecker::visit(Fractional *node) {
    node->set_type(FractionalTy);
}

void TypeChecker::visit(String *node) {
    node->set_type(StringTy);
}

void TypeChecker::visit(Boolean *node) {
    node->set_type(BooleanTy);
}

void TypeChecker::propagate_if_undef(IRNode *a, IRNode *b) {
    if (a->type() == UndefinedTy) {
        a->set_type(b->type());
    } else if (b->type() == UndefinedTy) {
        b->set_type(a->type());
    }
}
