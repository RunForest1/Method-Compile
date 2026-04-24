#pragma once
#include <string>
#include <map>
#include "Token.h"

class Lexer {
private:
    std::string input;
    size_t pos;
    int line;
    int column;

    // Состояния автомата
    enum State {
        S_START,
        S_ID,
        S_INT,
        S_FLOAT,
        S_STRING,
        S_OPERATOR
    };

    // Классы символов
    enum CharClass {
        C_LETTER,
        C_DIGIT,
        C_DOT,
        C_SPACE,
        C_OP,
        C_SEP,
        C_QUOTE,
        C_OTHER
    };

    // Таблица переходов
    std::map<std::pair<State, CharClass>, State> table;

    // Методы
    void initTable();
    CharClass getCharClass(char c);

    char peek();
    char advance();

public:
    Lexer(const std::string& code);
    Token nextToken();
};