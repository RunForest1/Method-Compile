#pragma once
#include <string>

// Типы токенов
enum TokenType {
    T_INT,
    T_FLOAT,
    T_ID,
    T_KEYWORD,
    T_OPERATOR,
    T_SEPARATOR,
    T_STRING,
    T_EOF,
    T_ERROR
};

// Структура токена
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token();
    Token(TokenType t, const std::string& v, int l, int c);
};