#include "Parser.h"
#include <stdexcept> // Для std::runtime_error

void Parser::initTable() {
    precedenceTable["+"] = 1;
    precedenceTable["-"] = 1;
    precedenceTable["*"] = 2;
    precedenceTable["/"] = 2;
    precedenceTable[":="] = 0; 
    
    // Приоритет индексации массива [] очень высокий (как у постфиксных операций)
    precedenceTable[OP_INDEX] = 3; 
}

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {
    initTable();
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

int Parser::getPrecedence(const Token& t) {
    if (precedenceTable.count(t.value)) {
        return precedenceTable[t.value];
    }
    return 0;
}

void Parser::parseExpression() {
    while (pos < tokens.size()) {
        Token t = advance();

        // --- ИГНОРИРОВАНИЕ ПУСТЫХ ТОКЕНОВ И РАЗДЕЛИТЕЛЕЙ ---
        // Если токен пустой или это просто разделитель (кроме скобок, которые обрабатываем отдельно)
        if (t.value.empty()) {
            continue; // Пропускаем пустые токены, чтобы не падало с "Unexpected token ''"
        }

        // 1. Операнды: числа, идентификаторы, СТРОКИ
        if (t.type == T_INT || t.type == T_FLOAT || t.type == T_ID || t.type == T_STRING) {
            output.push_back(t);
        }
        // 2. Операторы (+, -, *, /, :=)
        else if (isOperator(t)) {
             // Важно: не выталкиваем '[' из стека, пока не встретим ']'
             while (!stack.empty() && 
                   stack.back().value != "(" && 
                   stack.back().value != "[" && 
                   getPrecedence(stack.back()) >= getPrecedence(t)) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(t);
        }
        // 3. Круглые скобки ()
        else if (t.value == "(") {
            stack.push_back(t);
        }
        else if (t.value == ")") {
            bool foundOpen = false;
            while (!stack.empty()) {
                if (stack.back().value == "(") {
                    foundOpen = true;
                    stack.pop_back();
                    break;
                }
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!foundOpen) throw std::runtime_error("Syntax Error: Mismatched ')'");
        }
        // 4. Квадратные скобки []
        else if (t.value == "[") {
            stack.push_back(t);
        }
        else if (t.value == "]") {

            bool foundOpen = false;
            while (!stack.empty()) {
                if (stack.back().value == "[") {
                    foundOpen = true;
                    stack.pop_back();
                    break;
                }
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!foundOpen) throw std::runtime_error("Syntax Error: Mismatched ']'");
            
            Token indexOp;
            indexOp.type = T_OPERATOR;
            indexOp.value = OP_INDEX;
            indexOp.line = t.line;
            indexOp.column = t.column;
            output.push_back(indexOp);
        }
        // 5. Разделители и конец инструкции
        else if (t.type == T_SEPARATOR) {
            continue;
        }
        else if (t.type == T_KEYWORD) {
             throw std::runtime_error("Syntax Error: Unexpected keyword '" + t.value + "' in expression");
        }
        else {
            // Сюда мы попадаем, если токен не распознан
            throw std::runtime_error("Syntax Error: Unexpected token '" + t.value + "' (Type: " + std::to_string(t.type) + ") at line " + std::to_string(t.line));
        }
    }

    // Выгрузка остатка стека
    while (!stack.empty()) {
         if (stack.back().value == "(" || stack.back().value == "[") {
             throw std::runtime_error("Syntax Error: Unclosed bracket/parenthesis");
         }
         output.push_back(stack.back());
         stack.pop_back();
    }
}

std::vector<Token> Parser::parse() {
    parseExpression();
    return output;
}