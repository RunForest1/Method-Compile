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

        if (t.value.empty()) {
            continue; 
        }

        // 1. Операнды
        if (t.type == T_INT || t.type == T_FLOAT || t.type == T_ID || t.type == T_STRING) {
            output.push_back(t);
        }
        // 2. Операторы
        else if (isOperator(t)) {
             while (!stack.empty() && 
                   stack.back().value != "(" && 
                   stack.back().value != "[" && 
                   getPrecedence(stack.back()) >= getPrecedence(t)) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(t);
        }
        // 3. Скобки ()
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
        // 4. Скобки []
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
            indexOp.value = OP_INDEX; // Убедитесь, что OP_INDEX определен, например, как "[]" или "@"
            indexOp.line = t.line;
            indexOp.column = t.column;
            output.push_back(indexOp);
        }
        // 5. Разделители (;)
        else if (t.type == T_SEPARATOR) {
            // Выгружаем остаток стека, так как инструкция закончилась
            while (!stack.empty()) {
                 if (stack.back().value == "(" || stack.back().value == "[") {
                     throw std::runtime_error("Syntax Error: Unclosed bracket in statement");
                 }
                 output.push_back(stack.back());
                 stack.pop_back();
            }
            continue;
        }
        else if (t.type == T_KEYWORD) {
             throw std::runtime_error("Syntax Error: Unexpected keyword '" + t.value + "'");
        }
        else {
            throw std::runtime_error("Syntax Error: Unexpected token '" + t.value + "'");
        }
    }

    // Финальная выгрузка (для последнего выражения, если после него нет ;)
    while (!stack.empty()) {
         if (stack.back().value == "(" || stack.back().value == "[") {
             throw std::runtime_error("Syntax Error: Unclosed bracket/parenthesis at EOF");
         }
         output.push_back(stack.back());
         stack.pop_back();
    }
}

std::vector<Token> Parser::parse() {
    parseExpression();
    return output;
}