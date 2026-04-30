#ifndef LEXEM_H
#define LEXEM_H

#include <string>

// Типы лексем согласно пункту 1.1 спецификации
enum class LexemType {
    L_ID = 1,                   // Имена (идентификаторы)
    L_KEYWORD,                  // Служебные слова
    L_INT,                      // Целые числа
    L_FLOAT,                    // Вещественные числа
    L_STRING,                   // Символьные строки
    L_ADDITIVE_OPERATOR,        // + -
    L_MULTIPLICATIVE_OPERATOR,  // * /
    L_COMPARISON_OPERATOR,      // <, >, ==, <=, >=, !=
    L_ASSIGNMENT_OPERATOR,      // :=
    L_DELIMITER,                // ( ) [ ] ; ,
    L_TERMINATOR,               // Конец файла (⊥)
    L_ERROR                     // Ошибка
};

struct Lexem {
    LexemType type;
    std::string value;
    int line;
    int column;

    std::string getTypeName() const;
};

#endif