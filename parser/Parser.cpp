#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), pos(0) {}

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
    if (t.value == "+" || t.value == "-") return 1;
    if (t.value == "*" || t.value == "/") return 2;
    return 0;
}

void Parser::parseExpression() {
    while (pos < tokens.size()) {
        Token t = advance();

        if (t.type == T_INT || t.type == T_FLOAT || t.type == T_ID) {
            output.push_back(t);
        }
        else if (isOperator(t)) {
            while (!stack.empty() &&
                   getPrecedence(stack.back()) >= getPrecedence(t)) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(t);
        }
        else if (t.value == "(") {
            stack.push_back(t);
        }
        else if (t.value == ")") {
            while (!stack.empty() && stack.back().value != "(") {
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!stack.empty()) stack.pop_back(); 
        }
        else if (t.value == ";") {
            break;
        }
    }

    while (!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }
}

std::vector<Token> Parser::parse() {
    parseExpression();
    return output;
}