#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <vector>
#include <functional>
#include <map>
#include "Lexer.h"
#include "Rpn.h"
#include "ParserTypes.h"

class Parser {
public:
    Parser(Lexer& lexer);
    // Запуск анализа и получение итоговой ОПС
    std::vector<RpnElement> parse();

private:
    Lexer& lexer;
    Lexem currentLexem;
    std::stack<StackSymbol> parseStack;
    std::stack<int> labelStack; // Магазин меток для программ P1-P5[cite: 13]
    std::vector<RpnElement> rpn;

    std::map<std::string, std::function<void()>> semanticActions;

    void initSemanticTable(); // Метод инициализации таблицы действий
    void executeAction(const std::string& action);
    void pushProduction(NonTerm nt, LexemType lt);
};

#endif