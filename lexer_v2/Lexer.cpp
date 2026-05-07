#include "Lexer.h"
#include <iostream>
#include <cctype>

// Инициализация статической таблицы переходов (согласно пункту 1.2 спецификации)
// Строки: Промежуточные состояния (0-10)
// Столбцы: Категории символов (C_LETTER, C_DIGIT, и т.д.)
const State Lexer::transitionTable[11][C_COUNT] = {
    //  LETTER    DIGIT    DOT      QUOTE     COLON    NOT      EQUAL    COMP     SLASH    ASTER    ADD      DELIM    SPACE    TERM     OTHER
    { S_ID,    S_INT,   S_ERROR, S_STRING, S_COLON, S_NOT,   S_COMP_OP,S_COMP_OP,S_SLASH, S_F,     S_F,     S_F,     S_START, S_F,     S_ERROR }, // S_START
    { S_ID,    S_ID,    S_F_B,   S_F_B,    S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B   }, // S_ID
    { S_F_B,   S_INT,   S_FLOAT, S_F_B,    S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B   }, // S_INT
    { S_F_B,   S_FLOAT, S_F_B,   S_F_B,    S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B   }, // S_FLOAT
    { S_STRING,S_STRING,S_STRING,S_F,      S_STRING,S_STRING,S_STRING,S_STRING,S_STRING,S_STRING,S_STRING,S_STRING,S_STRING,S_ERROR, S_STRING }, // S_STRING
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR,  S_ERROR, S_ERROR, S_F,     S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR }, // S_COLON
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR,  S_ERROR, S_ERROR, S_F,     S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR }, // S_NOT
    { S_F_B,   S_F_B,   S_F_B,   S_F_B,    S_F_B,   S_F_B,   S_F,     S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B   }, // S_COMP_OP
    { S_F_B,   S_F_B,   S_F_B,   S_F_B,    S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_COM_B, S_F_B,   S_F_B,   S_F_B,   S_F_B,   S_F_B   }, // S_SLASH
    { S_COM_B, S_COM_B, S_COM_B, S_COM_B,  S_COM_B, S_COM_B, S_COM_B, S_COM_B, S_COM_B, S_COM_E, S_COM_B, S_COM_B, S_COM_B, S_ERROR, S_COM_B }, // S_COM_B
    { S_COM_B, S_COM_B, S_COM_B, S_COM_B,  S_COM_B, S_COM_B, S_COM_B, S_COM_B, S_START, S_COM_B, S_COM_B, S_COM_B, S_COM_B, S_ERROR, S_COM_B }  // S_COM_E
};

Lexer::Lexer(const std::string& sourceCode) 
    : source(sourceCode), pos(0), currentLine(1), currentCol(1) {}

void Lexer::printError(const Lexem& errLexem) const {
    if (errLexem.type != LexemType::L_ERROR) return;

    std::cerr << "Лексическая ошибка: "
              << "строка " << errLexem.line 
              << ", символ " << errLexem.column 
              << ". Недопустимая последовательность / символ (внутри квадратных скобок): [" << errLexem.value << "]" 
              << std::endl;
}

// Классификация входного символа (пункт 1.3)
CharClass Lexer::getCharClass(char c) const {
    if (c == '\0') return C_TERMINATOR;
    if (isspace(static_cast<unsigned char>(c))) return C_SPACE;
    if (isalpha(static_cast<unsigned char>(c))) return C_LETTER;
    if (isdigit(static_cast<unsigned char>(c))) return C_DIGIT;
    
    switch (c) {
        case '.': return C_DOT;
        case '\'': return C_QUOTE;
        case ':': return C_COLON;
        case '!': return C_NOT;
        case '=': return C_EQUAL;
        case '<':
        case '>': return C_COMPARE;
        case '/': return C_SLASH;
        case '*': return C_ASTERISK;
        case '+':
        case '-': return C_ADDITIVE;
        case '(': case ')':
        case '[': case ']':
        case ';': case ',': return C_DELIMITER;
        default: return C_OTHER;
    }
}

// Основной цикл автомата
Lexem Lexer::getNextLexem() {
    State currentState = S_START;
    State prevState = S_START;
    std::string lexemeValue = "";
    
    // Координаты начала лексемы для корректной диагностики
    int startLine = currentLine;
    int startCol = currentCol;

    // Цикл работает, пока мы не перейдем в терминальное состояние (S_F, S_F_B или S_ERROR)
    while (currentState < S_F) {
        char c = (pos < source.length()) ? source[pos] : '\0';
        CharClass cc = getCharClass(c);
        State nextState = transitionTable[currentState][cc];

        if (nextState == S_START) {
            // Пропускаем символы, которые не формируют лексему (пробелы, комментарии)
            if (c == '\n') { currentLine++; currentCol = 1; }
            else { currentCol++; }
            pos++;
            // Обновляем начало потенциальной лексемы
            startLine = currentLine;
            startCol = currentCol;
        } 
        else if (nextState < S_F) {
            // Если мы перешли в состояние начала комментария, сбрасываем накопленный '/' (чтобы игнорировать комментарии в полной мере)
            if (nextState == S_COM_B) lexemeValue = "";
            // Накопление символа в промежуточных состояниях
            // Игнорируем накопление внутри тела комментариев
            if (currentState != S_COM_B && currentState != S_COM_E) {
                lexemeValue += c;
            }
            if (c == '\n') { currentLine++; currentCol = 1; }
            else { currentCol++; }
            pos++;
        }
        
        // Особый случай: конец файла в начальном состоянии
        if (c == '\0' && nextState == S_START) {
            return { LexemType::L_TERMINATOR, "⊥", currentLine, currentCol };
        }

        prevState = currentState;
        currentState = nextState;
    }

    // --- Пост-обработка после выхода из цикла ---

    // 1. Обработка ошибок (пункт 1.4)
    if (currentState == S_ERROR) {
        std::string errorValue = lexemeValue;
        
        // Из-за чудес русского языка и UTF-8 кодировки, мы должны аккуратно захватить весь символ, который вызвал ошибку, а он может занимать больше 1 байта
        if (pos < source.length()) {
            unsigned char firstByte = static_cast<unsigned char>(source[pos]);
            
            // UTF-8: определяем количество байт в символе по первому байту
            int utf8Bytes = 1;
            if ((firstByte & 0xE0) == 0xC0) utf8Bytes = 2;  // 110xxxxx
            else if ((firstByte & 0xF0) == 0xE0) utf8Bytes = 3;  // 1110xxxx
            else if ((firstByte & 0xF8) == 0xF0) utf8Bytes = 4;  // 11110xxx
            
            // Добавляем все байты символа в ошибочное значение
            for (int i = 0; i < utf8Bytes && pos + i < source.length(); ++i) {
                errorValue += source[pos + i];
            }
        }
        
        Lexem err = { LexemType::L_ERROR, errorValue, startLine, startCol };
        printError(err); 
        return err;
    }

    // 2. Обработка состояния S_F (символ принадлежит текущей лексеме)
    if (currentState == S_F) {
        char c = (pos < source.length()) ? source[pos] : '\0';
        // Добавляем символ, если это не конец файла
        if (c != '\0') {
            lexemeValue += c;
            pos++; currentCol++;
        } else if (lexemeValue.empty()) {
            return { LexemType::L_TERMINATOR, "⊥", currentLine, currentCol };
        }
    }

    // 3. Состояние S_F_B (Final Back)
    // Указатель pos НЕ сдвигается, так как текущий символ принадлежит следующей лексеме

    // Определяем финальный тип на основе накопленного значения и состояния
    return { determineLexemType(prevState, lexemeValue), lexemeValue, startLine, startCol };
}

// Вспомогательный метод для маппинга состояния в тип лексемы
LexemType Lexer::determineLexemType(State lastState, const std::string& value) {
    switch (lastState) {
        case S_ID:
            // Проверка: является ли идентификатор ключевым словом?
            if (keywords.count(value)) return LexemType::L_KEYWORD;
            return LexemType::L_ID;

        case S_INT:
            return LexemType::L_INT;

        case S_FLOAT:
            return LexemType::L_FLOAT;

        case S_STRING:
            return LexemType::L_STRING;

        case S_COLON: // Если вышли по S_F из двоеточия, значит нашли ':='
            if (value == ":=") return LexemType::L_ASSIGNMENT_OPERATOR;
            return LexemType::L_ERROR;

        case S_COMP_OP:
        case S_NOT:
            return LexemType::L_COMPARISON_OPERATOR;

        case S_SLASH:
            return LexemType::L_MULTIPLICATIVE_OPERATOR;
        case S_START:
        case S_F:
        case S_F_B:
            // Обработка одиночных символов (операции, разделители)
            if (value == "+" || value == "-") return LexemType::L_ADDITIVE_OPERATOR;
            if (value == "*" || value == "/") return LexemType::L_MULTIPLICATIVE_OPERATOR;
            if (value == "==" || value == "!=" || value == "<" || 
                value == ">" || value == "<=" || value == ">=") return LexemType::L_COMPARISON_OPERATOR;
            if (value.length() == 1 && std::string("()[]{};,").find(value[0]) != std::string::npos) {
                return LexemType::L_DELIMITER;
            }
            break;

        default:
            break;
    }
    return LexemType::L_ERROR;
}