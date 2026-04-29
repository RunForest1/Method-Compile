#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

// ==================== ТИПЫ ДАННЫХ ====================

enum class ValueType {
    INT,
    FLOAT,
    STRING,
    ARRAY_INT,
    ARRAY_FLOAT,
    UNDEFINED
};

// Значение в памяти (может быть число, строка или массив)
struct Value {
    ValueType type;
    double numValue;
    std::string strValue;
    std::vector<double> arrayValue;  // для массивов

    Value() : type(ValueType::UNDEFINED), numValue(0.0) {}
    Value(double v) : type(ValueType::FLOAT), numValue(v) {}
    Value(int v) : type(ValueType::INT), numValue(static_cast<double>(v)) {}
    Value(const std::string& v) : type(ValueType::STRING), strValue(v), numValue(0.0) {}
    
    // Конвертация в число
    double toNumber() const {
        if (type == ValueType::INT || type == ValueType::FLOAT) {
            return numValue;
        }
        return 0.0;
    }

    // Конвертация в строку
    std::string toString() const {
        if (type == ValueType::STRING) {
            return strValue;
        }
        if (type == ValueType::INT || type == ValueType::FLOAT) {
            if (numValue == static_cast<int>(numValue)) {
                return std::to_string(static_cast<int>(numValue));
            }
            return std::to_string(numValue);
        }
        return "";
    }

    // Автоматическое преобразование типов
    void coerceTo(ValueType targetType) {
        if (type == targetType) return;

        if (targetType == ValueType::INT) {
            numValue = static_cast<int>(numValue);
            type = ValueType::INT;
        } else if (targetType == ValueType::FLOAT) {
            type = ValueType::FLOAT;
        }
    }
};

// ==================== ОПЕРАЦИИ ОПС ====================

enum class OpType {
    // Арифметические
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,        // унарный минус

    // Сравнение
    OP_LT,         // <
    OP_GT,         // >
    OP_LE,         // <=
    OP_GE,         // >=
    OP_EQ,         // ==
    OP_NE,         // !=

    // Логические
    OP_AND,        // &&
    OP_OR,         // ||
    OP_NOT,        // !

    // Операции со стеком
    OP_PUSH_INT,
    OP_PUSH_FLOAT,
    OP_PUSH_STRING,
    OP_PUSH_VAR,
    OP_POP_VAR,    // присваивание

    // Работа с массивами
    OP_ARRAY_DECL, // объявление массива
    OP_ARRAY_GET,  // получение элемента массива
    OP_ARRAY_SET,  // установка элемента массива

    // Управление потоком
    OP_JMP,        // безусловный прыжок
    OP_JMP_FALSE,  // прыжок если ложь
    OP_LABEL,      // метка

    // Ввод-вывод
    OP_READ,
    OP_PRINT,

    // Вспомогательные
    OP_NOP,
    OP_END
};

// Инструкция ОПС
struct Instruction {
    OpType op;
    std::string arg;   // для операндов (числа, переменные, метки)
    int jumpTarget;    // для прыжков

    Instruction(OpType o, const std::string& a = "", int jt = -1)
        : op(o), arg(a), jumpTarget(jt) {}
};

// Программа в виде ОПС
using Program = std::vector<Instruction>;

// ==================== ИНТЕРПРЕТАТОР ====================

class Interpreter {
private:
    Program program;
    std::map<std::string, Value> variables;
    std::map<std::string, std::pair<int, int>> arrayInfo; // name -> (size, type: 0=int, 1=float)
    std::vector<Value> stack;
    int pc;  // program counter

    Value stackPop();
    void stackPush(const Value& v);
    Value performOperation(OpType op, const Value& a, const Value& b);
    Value performUnaryOperation(OpType op, const Value& a);

public:
    Interpreter(const Program& prog);
    void execute();
    void printVariable(const std::string& name);
};
