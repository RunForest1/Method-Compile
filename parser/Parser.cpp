#include "Parser.h"

// Инициализация таблицы приоритетов
void Parser::initTable() {
    precedenceTable["+"] = 1;
    precedenceTable["-"] = 1;
    precedenceTable["*"] = 2;
    precedenceTable["/"] = 2;
    precedenceTable[":="] = 0; 
}

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {
    initTable(); // Заполняем таблицу при создании парсера
}

Token Parser::peek() {
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos];
}

Token Parser::advance() {
    return tokens[pos++];
}

bool Parser::isOperator(const Token& t) {
    return t.type == T_OPERATOR;
}

// Получаем приоритет из таблицы. Если оператора нет в таблице — возвращаем 0
int Parser::getPrecedence(const Token& t) {
    if (precedenceTable.count(t.value)) {
        return precedenceTable[t.value];
    }
    return 0;
}

void Parser::parseExpression() {
    while (pos < tokens.size()) {
        Token t = advance();

        // 1. Операнды сразу в выходной поток
        if (t.type == T_INT || t.type == T_FLOAT || t.type == T_ID || t.type == T_STRING) {
            output.push_back(t);
        }
        // 2. Операторы обрабатываем через таблицу приоритетов
        else if (isOperator(t)) {
            while (!stack.empty() && 
                   stack.back().value != "(" &&
                   getPrecedence(stack.back()) >= getPrecedence(t)) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(t);
        }
        // 3. Скобки
        else if (t.value == "(") {
            stack.push_back(t);
        }
        else if (t.value == ")") {
            while (!stack.empty() && stack.back().value != "(") {
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!stack.empty()) stack.pop_back(); // Удаляем '('
        }
        // 4. Конец выражения
        else if (t.value == ";") {
            break;
        }
    }

    // Выгружаем остаток стека
    while (!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }
}

std::vector<Token> Parser::parse() {
    parseExpression();
    return output;
}