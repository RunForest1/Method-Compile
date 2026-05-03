#ifndef RPN_H
#define RPN_H

#include <string>
#include <vector>
#include <stdexcept>

// Типы элементов в ОПС
enum class RPNItemType {
    LITERAL,        // Константа (значение)
    VARIABLE,       // Переменная (в стек кладется ССЫЛКА на нее)
    LABEL,          // Метка для переходов
    OPERATOR,       // +, -, *, /, <, >, <=, >=, ==, !=
    ASSIGN,         // :=
    JMP,            // j (безусловный переход)
    JF,             // jf (переход по лжи)
    ARRAY_IDX_1D,   // i (индексация 1D массива)
    ARRAY_IDX_2D,   // i2 (индексация 2D массива)
    ALLOC_1D,       // m1 (выделение памяти 1D)
    ALLOC_2D,       // m2 (выделение памяти 2D)
    READ,           // r (ввод)
    WRITE,          // w (вывод)
    FUNCTION        // sqrt, exp, log
};

// Элемент ОПС
struct RPNItem {
    RPNItemType type;
    std::string value;
    int line;
    int column;
};

// Типы данных
enum class DataType { INT, FLOAT, STRING, BOOL };

// Хранилище значения
struct Value {
    DataType type = DataType::INT;
    int i_val = 0;
    double f_val = 0.0;
    std::string s_val = "";
    bool b_val = false;
};

// "Паспорт" массива (описатель)
struct ArrayPassport {
    int dim1 = 0;
    int dim2 = 0;
    bool is2D = false;
    std::vector<Value> data;
};

// Элемент магазина (стека) интерпретатора
enum class StackItemType { VALUE, VAR_REF, ARRAY_ELEM_REF };

struct StackItem {
    StackItemType type;
    Value val;                     // Если это VALUE
    std::string varName;           // Если это VAR_REF (ссылка на переменную/паспорт)
    int arrayIndex = -1;           // Если это ARRAY_ELEM_REF, смещение в массиве varName
};

#endif