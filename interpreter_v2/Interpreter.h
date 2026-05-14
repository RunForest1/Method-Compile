#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <map>
#include "Rpn.h"
#include "Value.h"

const double PI = 3.14159265358979323846;
const int ITERATIONS = 200; // Количество итераций для точности

class Interpreter {
public:
    Interpreter(const std::vector<RpnElement>& code);
    void run();

private:
    std::vector<RpnElement> rpn;
    size_t ip = 0;
    std::stack<Value> stack;
    std::map<std::string, Value> memory;

    Value pop();
    Value resolve(Value v);
    void executeOperator(const std::string& op);
    void applyBinary(const std::string& op, Value l, Value r);
};

#endif