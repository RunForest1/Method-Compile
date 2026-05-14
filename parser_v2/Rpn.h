#ifndef RPN_H
#define RPN_H

#include <string>
#include <vector>

// Типы элементов ОПС
enum class RpnElementType {
    ADDR_VAR,   // Переменная
    CONST_VAL,  // Константа
    OPERATOR,   // Код операции (+, :=, J, JF и т.д.)
    LABEL       // Маркер в массиве ОПС (адрес перехода)
};

struct RpnElement {
    RpnElementType type;
    std::string value;
};

#endif