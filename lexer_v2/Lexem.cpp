#include "Lexem.h"

/**
 * Реализация метода getTypeName для структуры Lexem.
 * Используется для отладки и вывода результатов тестирования.
 */
std::string Lexem::getTypeName() const {
    switch (type) {
        case LexemType::L_ID: 
            return "ID";
        case LexemType::L_KEYWORD: 
            return "KEYWORD";
        case LexemType::L_INT: 
            return "INT";
        case LexemType::L_FLOAT: 
            return "FLOAT";
        case LexemType::L_STRING: 
            return "STRING";
        case LexemType::L_ADDITIVE_OPERATOR: 
            return "ADD_OP";
        case LexemType::L_MULTIPLICATIVE_OPERATOR: 
            return "MULT_OP";
        case LexemType::L_COMPARISON_OPERATOR: 
            return "COMP_OP";
        case LexemType::L_ASSIGNMENT_OPERATOR: 
            return "ASSIGN";
        case LexemType::L_DELIMITER: 
            return "DELIM";
        case LexemType::L_TERMINATOR: 
            return "TERMINATOR";
        case LexemType::L_ERROR: 
            return "ERROR";
        default: 
            return "UNKNOWN";
    }
}