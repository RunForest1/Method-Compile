#pragma once
#include <vector>
#include "../lexer/Token.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    std::vector<Token> output; // ОПС
    std::vector<Token> stack;  // стек операторов

    Token peek();
    Token advance();

    int getPrecedence(const Token& t); // Приоритет
    bool isOperator(const Token& t);

    void parseExpression();

public:
    Parser(const std::vector<Token>& tokens);

    std::vector<Token> parse(); // возвращает ОПС
};