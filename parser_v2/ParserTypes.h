#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include <string>
#include "Lexem.h"

// Список нетерминалов грамматики
enum class NonTerm {
    Program, Statement, Assignment, IfStatement, ElsePart, 
    WhileStatement, ReadStatement, WriteStatement, Condition, 
    Expression, ExpressionTail, Term, TermTail, Factor, 
    UnaryOperand, ArrayIndex, ArrayTail, SemanticTrigger
};

enum class SymbolType { TERMINAL, NON_TERMINAL, SEMANTIC_ACTION };

struct StackSymbol {
    SymbolType type;
    int id;
    std::string action;

    // Конструктор для терминалов и нетерминалов
    StackSymbol(SymbolType t, int i) : type(t), id(i), action("") {}
    
    // Конструктор для семантических действий
    StackSymbol(SymbolType t, std::string a) : type(t), id(0), action(a) {}
};

#endif