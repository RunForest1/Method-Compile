#pragma once
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <iostream>
#include <stdexcept>
#include "../lexer/Token.h"

class Interpreter {
private:
    std::vector<Token> rpn;          // Обратная польская запись
    std::map<std::string, double> variables; // Таблица переменных (имя -> значение)
    std::map<std::string, std::vector<double>> arrays; // Для одномерных массивов
    std::stack<Token> evalStack;     // Стек токенов для вычислений

    // Вспомогательные функции
    double convertToDouble(const Token& t);
    void checkStack(size_t required, const std::string& op);

public:
    Interpreter(const std::vector<Token>& rpnTokens);
    void run();
    
    // Для отладки: вывести все переменные
    void dumpVariables();
};