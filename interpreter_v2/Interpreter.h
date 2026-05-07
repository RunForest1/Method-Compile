#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "RPN.h"
#include <map>
#include <stack>
#include <iostream>
#include <cmath>

class Interpreter {
public:
    void execute(const std::vector<RPNItem>& rpn);
    Value getVariableValue(const std::string& name);

private:
    std::map<std::string, Value> variables;
    std::map<std::string, ArrayPassport> arrays;
    std::stack<StackItem> stack;
    
    int ip; // Счетчик команд (номер исполняемого элемента ОПС)

    [[noreturn]] void throwError(const std::string& msg, const RPNItem& item) const;
    
    StackItem popStack(const RPNItem& item);
    Value getValue(const StackItem& item, const RPNItem& opItem);
    
    void castToCommon(Value& v1, Value& v2);
    void evaluateMath(const std::string& op, const RPNItem& item);
    void evaluateLogic(const std::string& op, const RPNItem& item);
    void evaluateFunction(const std::string& func, const RPNItem& item);
};

#endif