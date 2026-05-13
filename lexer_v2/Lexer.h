#ifndef LEXER_H
#define LEXER_H

#include "Lexem.h"
#include <set>
#include <string>

// Состояния автомата (пункт 1.2)
enum State
{
    S_START,
    S_ID,
    S_INT,
    S_FLOAT,
    S_STRING,
    S_COLON,
    S_NOT,
    S_COMP_OP,
    S_SLASH,
    S_COM_B,
    S_COM_E,
    S_F,
    S_F_B,
    S_ERROR
};

// Категории символов (пункт 1.3)
enum CharClass
{
    C_LETTER,
    C_DIGIT,
    C_DOT,
    C_QUOTE,
    C_COLON,
    C_NOT,
    C_EQUAL,
    C_COMPARE,
    C_SLASH,
    C_ASTERISK,
    C_ADDITIVE,
    C_DELIMITER,
    C_SPACE,
    C_TERMINATOR,
    C_OTHER,
    C_COUNT
};

class Lexer
{
public:
    Lexer(const std::string &source);

    // Основной метод для получения следующей лексемы
    Lexem getNextLexem();

private:
    std::string source;
    size_t pos;
    int currentLine;
    int currentCol;

    static const State transitionTable[11][C_COUNT];

    CharClass getCharClass(char c) const;
    bool isKeyword(const std::string &s) const;
    LexemType determineLexemType(State lastState, const std::string &value);
    void printError(const Lexem &errLexem) const;

    const std::set<std::string> keywords = {
        "if", "then", "else", "while", "do", "read", "write", "int", "float", "sin", "cos",
        "log",
        "exp",
        "tanh",
        "ctanh",
        "power"};
};

#endif