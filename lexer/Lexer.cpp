#include "Lexer.h"
#include <cctype>

// Ключевые слова
static std::map<std::string, TokenType> keywords = {
    {"READ", T_KEYWORD},
    {"PRINT", T_KEYWORD},
    {"IF", T_KEYWORD},
    {"THEN", T_KEYWORD},
    {"ELSE", T_KEYWORD},
    {"WHILE", T_KEYWORD},
    {"DO", T_KEYWORD},
    {"END", T_KEYWORD}
};

// ================= ИНИЦИАЛИЗАЦИЯ =================

Lexer::Lexer(const std::string& code)
    : input(code), pos(0), line(1), column(1) {
    initTable();
}

// ================= ТАБЛИЦА ПЕРЕХОДОВ =================

void Lexer::initTable() {
    table[{S_START, C_LETTER}] = S_ID;
    table[{S_START, C_DIGIT}] = S_INT;
    table[{S_START, C_QUOTE}] = S_STRING;
    table[{S_START, C_OP}] = S_OPERATOR;

    table[{S_ID, C_LETTER}] = S_ID;
    table[{S_ID, C_DIGIT}] = S_ID;

    table[{S_INT, C_DIGIT}] = S_INT;
    table[{S_INT, C_DOT}] = S_FLOAT;

    table[{S_FLOAT, C_DIGIT}] = S_FLOAT;

    table[{S_OPERATOR, C_OP}] = S_OPERATOR;
}

// ================= КЛАСС СИМВОЛА =================

Lexer::CharClass Lexer::getCharClass(char c) {
    if (std::isalpha(c) || c == '_') return C_LETTER;
    if (std::isdigit(c)) return C_DIGIT;
    if (c == '.') return C_DOT;
    if (std::isspace(c)) return C_SPACE;
    if (std::string("+-*/=<>!").find(c) != std::string::npos) return C_OP;
    if (std::string("(),;[]{}").find(c) != std::string::npos) return C_SEP;
    if (c == '"') return C_QUOTE;
    return C_OTHER;
}

// ================= ДВИЖЕНИЕ =================

char Lexer::peek() {
    if (pos >= input.size()) return '\0';
    return input[pos];
}

char Lexer::advance() {
    char c = peek();
    pos++;

    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    return c;
}

// ================= ОСНОВНОЙ АВТОМАТ =================

Token Lexer::nextToken() {
    State state = S_START;
    std::string buffer = "";

    int startLine = line;
    int startCol = column;

    while (true) {
        char c = peek();

        if (c == '\0') {
            if (state == S_ID) return Token(T_ID, buffer, startLine, startCol);
            if (state == S_INT) return Token(T_INT, buffer, startLine, startCol);
            if (state == S_FLOAT) return Token(T_FLOAT, buffer, startLine, startCol);
            if (state == S_OPERATOR) return Token(T_OPERATOR, buffer, startLine, startCol);
            return Token(T_EOF, "", line, column);
        }

        CharClass cc = getCharClass(c);

        if (table.count({state, cc})) {
            state = table[{state, cc}];

            if (state != S_START)
                buffer += c;

            advance();
        }
        else {
            // ОБРАБОТКА ЗАВЕРШЕНИЯ
            if (state == S_START) {
                if (cc == C_SPACE) {
                    advance();
                    startLine = line;
                    startCol = column;
                    continue;
                }

                if (cc == C_SEP) {
                    advance();
                    return Token(T_SEPARATOR, std::string(1, c), startLine, startCol);
                }

                return Token(T_ERROR, std::string(1, c), startLine, startCol);
            }

            if (state == S_ID) {
                if (keywords.count(buffer))
                    return Token(T_KEYWORD, buffer, startLine, startCol);
                return Token(T_ID, buffer, startLine, startCol);
            }

            if (state == S_INT)
                return Token(T_INT, buffer, startLine, startCol);

            if (state == S_FLOAT)
                return Token(T_FLOAT, buffer, startLine, startCol);

            if (state == S_OPERATOR)
                return Token(T_OPERATOR, buffer, startLine, startCol);
        }
    }
}