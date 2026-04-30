#pragma once
#include <vector>
#include <map>
#include "../lexer/Token.h"

// Символьная константа для оператора доступа к элементу массива в RPN
const std::string OP_INDEX = "[]"; 

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    std::vector<Token> output; // ОПС
    std::vector<Token> stack;  // стек операторов

    // Таблица приоритетов
    std::map<std::string, int> precedenceTable;

    Token peek();
    Token advance();

    int getPrecedence(const Token& t);
    bool isOperator(const Token& t);

    void initTable();      
    void parseExpression();

public:
    Parser(const std::vector<Token>& tokens);
    std::vector<Token> parse();
};