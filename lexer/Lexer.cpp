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
    : inputCode(code), pos(0), line(1), column(1) {
    // Подготавливаем логику переходов
    initTable();
}

// ================= ТАБЛИЦА ПЕРЕХОДОВ =================

void Lexer::initTable() {
    // Настройка переходов согласно слайду 21 лекции

    // Из начального состояния S_START (S)
    table[{S_START, C_LETTER}] = S_ID;
    table[{S_START, C_DIGIT}]  = S_INT;
    table[{S_START, C_QUOTE}]  = S_STRING;
    table[{S_START, C_COLON}]  = S_OPERATOR;
    table[{S_START, C_SLASH}]  = S_COMMENT_START;
    table[{S_START, C_DOT}]    = S_FINAL; // Точка сама по себе — разделитель или ошибка
    table[{S_START, C_SPACE}]  = S_START; // Пробелы игнорируются (S -> S)
    table[{S_START, C_EOF}]    = S_FINAL;

    // Идентификаторы (I)
    table[{S_ID, C_LETTER}] = S_ID;
    table[{S_ID, C_DIGIT}]  = S_ID;
    // Для остальных символов в S_ID перехода нет -> сработает Z* (откат)

    // Целые числа (C)
    table[{S_INT, C_DIGIT}] = S_INT;
    table[{S_INT, C_DOT}]   = S_DOT_SEEN; // Переход к вещественному числу

    // Промежуточное состояние после точки (D)
    table[{S_DOT_SEEN, C_DIGIT}] = S_FLOAT;

    // Вещественные числа (E)
    table[{S_FLOAT, C_DIGIT}] = S_FLOAT;

    // Строки (расширение функциональности)
    table[{S_STRING, C_LETTER}] = S_STRING;
    table[{S_STRING, C_DIGIT}]  = S_STRING;
    table[{S_STRING, C_SPACE}]  = S_STRING;
    table[{S_STRING, C_QUOTE}]  = S_FINAL; // Завершение строки

    // Операторы (обработка составных, напр. := )
    table[{S_OPERATOR, C_EQUALS}] = S_FINAL;
    
    // Комментарии (состояния K, L, M)
    table[{S_COMMENT_START, C_STAR}]   = S_COMMENT_BODY;  // /*
    table[{S_COMMENT_BODY, C_LETTER}]  = S_COMMENT_BODY;
    table[{S_COMMENT_BODY, C_DIGIT}]   = S_COMMENT_BODY;
    table[{S_COMMENT_BODY, C_SPACE}]   = S_COMMENT_BODY;
    table[{S_COMMENT_BODY, C_STAR}]    = S_COMMENT_ENDING; // *
    table[{S_COMMENT_ENDING, C_SLASH}] = S_START;          // / -> возврат в начало
    table[{S_COMMENT_ENDING, C_STAR}]  = S_COMMENT_ENDING; // Остаемся в M
}

// ================= КЛАСС СИМВОЛА =================

Lexer::CharClass Lexer::getCharClass(char c) {
    if (c == '\0') return C_EOF;
    if (std::isalpha(c) || c == '_') return C_LETTER;
    if (std::isdigit(c)) return C_DIGIT;
    if (c == '.') return C_DOT;
    if (std::isspace(c)) return C_SPACE;
    if (c == ':') return C_COLON;
    if (c == '=') return C_EQUALS;
    if (c == '/') return C_SLASH;
    if (c == '*') return C_STAR;
    if (c == '"') return C_QUOTE;
    if (std::string("(),;[]{}").find(c) != std::string::npos) return C_SEP;
    return C_OTHER;
}

// ================= ДВИЖЕНИЕ =================

char Lexer::peek() {
    // Проверяем на конец файла
    if (pos >= inputCode.size()) return '\0';
    // Иначе возвращаем символ на текущей позиции
    return inputCode[pos];
}

char Lexer::advance() {
    // Получаем текущий символ и продвигаем позицию
    char c = peek();
    pos++;

    // Обновляем координаты: если это перевод строки, увеличиваем номер строки и сбрасываем столбец
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
    State currentState = S_START;
    State lastActiveState = S_START;
    std::string buffer = "";

    int startLine = line;
    int startCol = column;

    // Основной цикл работает, пока не достигнуто состояние S_FINAL
    while (currentState != S_FINAL) {
        char currentChar = peek();
        CharClass charClass = getCharClass(currentChar);

        // Проверяем возможность перехода по таблице
        if (table.count({currentState, charClass})) {
            lastActiveState = currentState;
            currentState = table[{currentState, charClass}];

            // Логика поглощения символов
            if (currentState == S_START) {
                // Если мы вернулись в S_START (пробелы или конец комментария)
                // то просто продвигаемся, не сохраняя в буфер
                advance();
                startLine = line;
                startCol = column;
                buffer = "";
            } else if (currentState != S_FINAL) {
                // Накопление лексемы (напр. в состояниях I, C, E)
                buffer += currentChar;
                advance();
            } else {
                // Если перешли в S_FINAL (напр. закрывающая кавычка), 
                // поглощаем символ и выходим из цикла
                buffer += currentChar;
                advance();
            }
        }
        else {
            // Если перехода нет — это состояние Z* 
            // Мы НЕ вызываем advance(), символ остается для следующего токена.
            lastActiveState = currentState;
            currentState = S_FINAL;
        }
    }

    // Обработка случая, когда мы дошли до конца файла в S_START
    if (buffer.empty() && peek() == '\0') {
        return Token(T_EOF, "", line, column);
    }

    // Определяем финальный тип токена на основе "предпоследнего" состояния
    TokenType finalType = mapStateToTokenType(lastActiveState, buffer);

    // Дополнительная проверка на разделители, если автомат в S_START
    if (finalType == T_ERROR && lastActiveState == S_START) {
        char c = peek();
        if (getCharClass(c) == C_SEP) {
            return Token(T_SEPARATOR, std::string(1, advance()), startLine, startCol);
        }
    }

    return Token(finalType, buffer, startLine, startCol);
}

// ================= ОПРЕДЕЛЕНИЕ ТИПА =================

TokenType Lexer::mapStateToTokenType(State lastActiveState, const std::string& buffer) {
    switch (lastActiveState) {
        case S_ID:
            // Проверка на ключевые слова (слайд 19)
            if (keywords.count(buffer)) return T_KEYWORD;
            return T_ID;
        case S_INT:
            return T_INT;
        case S_FLOAT:
            return T_FLOAT;
        case S_STRING:
            return T_STRING;
        case S_OPERATOR:
        case S_COMMENT_START: // Если комментарий не начался, '/' это оператор
            return T_OPERATOR;
        default:
            return T_ERROR;
    }
}
