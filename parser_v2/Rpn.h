#ifndef RPN_H
#define RPN_H

#include <string>
#include <vector>

// Типы элементов ОПС согласно спецификации
enum class RpnElementType {
    ADDR_VAR,   // Ссылка на таблицу идентификаторов
    CONST_VAL,  // Константа
    OPERATOR,   // Код операции (+, :=, J, JF и т.д.)
    LABEL       // Индекс элемента в массиве ОПС (адрес перехода)
};

struct RpnElement {
    RpnElementType type;
    std::string value;
};

#endif