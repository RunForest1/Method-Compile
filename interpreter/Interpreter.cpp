#include "Interpreter.h"

Interpreter::Interpreter(const std::vector<Token>& rpnTokens) : rpn(rpnTokens) {}

double Interpreter::convertToDouble(const Token& t) {
    if (t.type == T_INT || t.type == T_FLOAT) {
        return std::stod(t.value);
    } else if (t.type == T_ID) {
        if (variables.find(t.value) == variables.end()) {
            // По требованию: можно без явных описаний (инициализируем нулем)
            variables[t.value] = 0.0;
        }
        return variables[t.value];
    }
    throw std::runtime_error("Ошибка семантики: ожидалось число или переменная в строке " + std::to_string(t.line));
}

void Interpreter::checkStack(size_t required, const std::string& op) {
    if (evalStack.size() < required) {
        throw std::runtime_error("Ошибка ОПС: недостаточно операндов для операции " + op);
    }
}

void Interpreter::run() {
    for (size_t i = 0; i < rpn.size(); ++i) {
        Token& t = rpn[i];

        // Операнды (числа и ID) просто кладем в стек
        if (t.type == T_INT || t.type == T_FLOAT || t.type == T_ID || t.type == T_STRING) {
            evalStack.push(t);
        } 
        // Операторы
        else if (t.type == T_OPERATOR) {
            if (t.value == ":=") {
                checkStack(2, ":=");
                Token valToken = evalStack.top(); evalStack.pop();
                Token varToken = evalStack.top(); evalStack.pop();

                if (varToken.type != T_ID) 
                    throw std::runtime_error("Ошибка: Слева от ':=' должен быть ID. Строка: " + std::to_string(t.line));

                variables[varToken.value] = convertToDouble(valToken);
            } 
            else {
                // Бинарные арифметические операции
                checkStack(2, t.value);
                double right = convertToDouble(evalStack.top()); evalStack.pop();
                double left = convertToDouble(evalStack.top()); evalStack.pop();
                double result = 0;

                if (t.value == "+") result = left + right;
                else if (t.value == "-") result = left - right;
                else if (t.value == "*") result = left * right;
                else if (t.value == "/") {
                    if (right == 0) throw std::runtime_error("Ошибка: Деление на ноль в строке " + std::to_string(t.line));
                    result = left / right;
                }
                
                // Результат вычисления кладем обратно как временный FLOAT
                evalStack.push(Token(T_FLOAT, std::to_string(result), t.line, t.column));
            }
        }
        // Ключевые слова (Ввод/Вывод)
        else if (t.type == T_KEYWORD) {
            if (t.value == "PRINT") {
                checkStack(1, "PRINT");
                std::cout << "---> OUTPUT: " << convertToDouble(evalStack.top()) << std::endl;
                evalStack.pop();
            }
            else if (t.value == "READ") {
                checkStack(1, "READ");
                Token varToken = evalStack.top(); evalStack.pop();
                if (varToken.type != T_ID) throw std::runtime_error("READ ожидает имя переменной");
                
                double inputVal;
                std::cout << "INPUT (" << varToken.value << "): ";
                std::cin >> inputVal;
                variables[varToken.value] = inputVal;
            }
        }
    }
}

void Interpreter::dumpVariables() {
    std::cout << "\n--- Table of variables ---" << std::endl;
    for (auto const& [name, val] : variables) {
        std::cout << name << " = " << val << std::endl;
    }
}