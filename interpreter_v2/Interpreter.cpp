#include "Interpreter.h"
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter(const std::vector<RpnElement>& code) : rpn(code) {}

// Вычисление exp
double manual_exp(double x) {
    double res = 1.0;
    double term = 1.0;
    for (int i = 1; i <= ITERATIONS; ++i) {
        term *= x / i;
        res += term;
        if (term < 1e-15 && term > -1e-15) break;
    }
    return res;
}

// Вычисление sin
double manual_sin(double x) {
    // Приведение угла к диапазону [-PI, PI]
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    double res = x;
    double term = x;
    for (int i = 1; i <= ITERATIONS; ++i) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        res += term;
        if (term < 1e-15 && term > -1e-15) break;
    }
    return res;
}

// Вычисление cos
double manual_cos(double x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    double res = 1.0;
    double term = 1.0;
    for (int i = 1; i <= ITERATIONS; ++i) {
        term *= -x * x / ((2 * i - 1) * (2 * i));
        res += term;
        if (term < 1e-15 && term > -1e-15) break;
    }
    return res;
}

// Вычисление log
double manual_log(double x) {
    if (x <= 0) throw std::runtime_error("Math error: log of non-positive number");
    
    // Используем метод итераций для ln(x) через y = (x-1)/(x+1)
    double y = (x - 1) / (x + 1);
    double y2 = y * y;
    double res = 0;
    double term = y;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        res += term / (2 * i + 1);
        term *= y2;
        if (term < 1e-15) break;
    }
    return 2.0 * res;
}


void Interpreter::run() {
    ip = 0;
    while (ip < rpn.size()) {
        const auto& el = rpn[ip];
        if (el.type == RpnElementType::CONST_VAL) {
            if (el.value.find("'") != std::string::npos)
                stack.push(Value(el.value.substr(1, el.value.size() - 2)));
            else if (el.value.find(".") != std::string::npos)
                stack.push(Value(std::stod(el.value)));
            else
                stack.push(Value(std::stoll(el.value)));
        }
        else if (el.type == RpnElementType::ADDR_VAR) {
            Value addr(el.value);
            addr.type = ValueType::ADDRESS;
            stack.push(addr);
        }
        else if (el.type == RpnElementType::LABEL) {
            std::string clean_label = el.value;
            
            // Если в value прилетает "LBL(21)", очистим до "21"
            if (clean_label.find("(") != std::string::npos) {
                size_t start = clean_label.find("(") + 1;
                size_t end = clean_label.find(")");
                clean_label = clean_label.substr(start, end - start);
            }
            
            stack.push(Value(clean_label)); 
        }
        else if (el.type == RpnElementType::OPERATOR) {
            executeOperator(el.value);
        }
        ip++;
    }
}

Value Interpreter::pop() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    Value v = stack.top();
    stack.pop();
    return v;
}

Value Interpreter::resolve(Value v) {
    if (v.type == ValueType::ADDRESS) {
        if (memory.find(v.s_val) == memory.end()) return Value(0);
        return memory[v.s_val];
    }
    return v;
}

void Interpreter::executeOperator(const std::string& op) {
    if (op == ":=") {
        Value val = resolve(pop());
        Value addr = pop();
        memory[addr.s_val] = val;
    }
    else if (op == "NEG") {
        Value v = resolve(pop());
        if (v.type == ValueType::FLOAT) stack.push(Value(-v.f_val));
        else stack.push(Value(-v.i_val));
    } 
    else if (op == "WRITE") {
        std::cout << resolve(pop()).toString() << std::endl;
    } 
    else if (op == "READ") {
        Value addr = pop();
        std::string input;
        std::cin >> input;
        try {
            if (input.find('.') != std::string::npos) 
                memory[addr.s_val] = Value(std::stod(input));
            else 
                memory[addr.s_val] = Value(std::stoll(input));
        } catch (...) {
            memory[addr.s_val] = Value(input);
        }
    }
    else if (op == "J") {
        ip = std::stoul(pop().s_val) - 1;
    }
    else if (op == "JF") {
        std::string label_val = pop().s_val;
        size_t target = std::stoul(label_val);

        bool cond = resolve(pop()).toBool();

        if (!cond) ip = target - 1;
    }
    else if (op == "INDEX1") {
        // 1. Извлекаем индекс и СРАЗУ резолвим его в значение
        Value idxVal = resolve(pop()); 
        // Получаем целое число независимо от того, FLOAT это или INT
        int idx = (idxVal.type == ValueType::FLOAT) ? (int)idxVal.f_val : idxVal.i_val;

        // 2. Извлекаем имя массива (ADDR)
        Value array = pop(); 
        
        // 3. Склеиваем: "arr" + "[1]" -> "arr[1]"
        Value finalAddr(array.s_val + "[" + std::to_string(idx) + "]");
        finalAddr.type = ValueType::ADDRESS;
        stack.push(finalAddr);
    }
    else if (op == "INDEX2") {
        // Извлекаем второй индекс [j] и резолвим его в число
        Value idxVal2 = resolve(pop());
        int idx2 = (idxVal2.type == ValueType::FLOAT) ? (int)idxVal2.f_val : idxVal2.i_val;

        // Извлекаем первый индекс [i] и резолвим его в число
        Value idxVal1 = resolve(pop());
        int idx1 = (idxVal1.type == ValueType::FLOAT) ? (int)idxVal1.f_val : idxVal1.i_val;

        //  Извлекаем базу массива (ADDR(matrix))
        Value array = pop(); 
        
        // Склеиваем в формате matrix[idx1, idx2]
        Value finalAddr(array.s_val + "[" + std::to_string(idx1) + ", " + std::to_string(idx2) + "]");
        finalAddr.type = ValueType::ADDRESS;
        
        stack.push(finalAddr);
    }
    else if (op == "SIN") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        stack.push(Value(manual_sin(x)));
    }
    else if (op == "COS") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        stack.push(Value(manual_cos(x)));
    }
    else if (op == "TAN") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        double s = manual_sin(x);
        double c = manual_cos(x);
        if (c == 0) throw std::runtime_error("Math error: Tangent undefined (cos is 0)");
        stack.push(Value(s / c));
    }
    else if (op == "CTAN") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        double s = manual_sin(x);
        double c = manual_cos(x);
        if (s == 0) throw std::runtime_error("Math error: Cotangent undefined (sin is 0)");
        stack.push(Value(c / s));
    }
    else if (op == "EXP") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        stack.push(Value(manual_exp(x)));
    }
    else if (op == "LOG") {
        Value val = resolve(pop());
        double x = (val.type == ValueType::FLOAT) ? val.f_val : (double)val.i_val;
        stack.push(Value(manual_log(x)));
    }
    else if (op == "POW") {
        // В ОПЗ порядок: Base, Exp, POW. 
        //Извлечение для бинарной операции:
        Value v_exp = resolve(pop());
        Value v_base = resolve(pop());
        
        double b = (v_base.type == ValueType::FLOAT) ? v_base.f_val : (double)v_base.i_val;
        double e = (v_exp.type == ValueType::FLOAT) ? v_exp.f_val : (double)v_exp.i_val;

        // a^b = exp(b * ln(a))
        if (b == 0 && e <= 0) throw std::runtime_error("Math error: 0^0 or 0^negative");
        if (b < 0) throw std::runtime_error("Math error: negative base in POW");
        
        stack.push(Value(manual_exp(e * manual_log(b))));
    }

    else {
        Value r = resolve(pop());
        Value l = resolve(pop());
        applyBinary(op, l, r);
    }
}

void Interpreter::applyBinary(const std::string& op, Value l, Value r) {
// Вспомогательная лямбда для получения double из любого значения
    auto asDouble = [](const Value& v) {
        return (v.type == ValueType::FLOAT) ? v.f_val : (double)v.i_val;
    };

    if (op == "+" || op == "-" || op == "*" || op == "/") {
        if (l.type == ValueType::FLOAT || r.type == ValueType::FLOAT) {
            // Считаем как числа с плавающей точкой
            double dl = asDouble(l);
            double dr = asDouble(r);
            if (op == "+") stack.push(Value(dl + dr));
            else if (op == "-") stack.push(Value(dl - dr));
            else if (op == "*") stack.push(Value(dl * dr));
            else if (op == "/") {
                if (dr == 0) throw std::runtime_error("Division by zero");
                stack.push(Value(dl / dr));
            }
        } else {
            if (op == "+") stack.push(Value(l.i_val + r.i_val));
            else if (op == "-") stack.push(Value(l.i_val - r.i_val));
            else if (op == "*") stack.push(Value(l.i_val * r.i_val));
            else if (op == "/") {
                if (r.i_val == 0) throw std::runtime_error("Division by zero");
                stack.push(Value(l.i_val / r.i_val));
            }
        }
    }
    else if (op == "<") stack.push(Value(asDouble(l) < asDouble(r)));
    else if (op == ">") stack.push(Value(asDouble(l) > asDouble(r)));
    
    else if (op == "<=") stack.push(Value(asDouble(l) <= asDouble(r)));
    else if (op == ">=") stack.push(Value(asDouble(l) >= asDouble(r))); 
    else if (op == "==" || op == "=") stack.push(Value(asDouble(l) == asDouble(r)));
    else if (op == "!=") stack.push(Value(asDouble(l) != asDouble(r)));
    else {
        throw std::runtime_error("Unknown binary operator: " + op);
    }
}