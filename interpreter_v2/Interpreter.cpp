#include "Interpreter.h"

void Interpreter::throwError(const std::string& msg, const RPNItem& item) const {
    std::cerr << "Ошибка выполнения (Строка: " << item.line 
              << ", Символ: " << item.column << "): " << msg << std::endl;
    throw std::runtime_error(msg);
}

StackItem Interpreter::popStack(const RPNItem& item) {
    if (stack.empty()) throwError("Магазин пуст. Недостаточно операндов.", item);
    StackItem top = stack.top();
    stack.pop();
    return top;
}

// Разыменование: если в магазине ссылка, получаем её числовое/строковое значение
Value Interpreter::getValue(const StackItem& item, const RPNItem& opItem) {
    if (item.type == StackItemType::VALUE) {
        return item.val;
    } else if (item.type == StackItemType::VAR_REF) {
        if (variables.find(item.varName) == variables.end()) {
            // Инициализация по умолчанию при первом чтении
            variables[item.varName] = Value{DataType::INT, 0, 0.0, "", false};
        }
        return variables[item.varName];
    } else if (item.type == StackItemType::ARRAY_ELEM_REF) {
        if (arrays.find(item.varName) == arrays.end()) throwError("Массив не существует", opItem);
        return arrays[item.varName].data[item.arrayIndex];
    }
    return Value{};
}

void Interpreter::castToCommon(Value& v1, Value& v2) {
    if (v1.type == DataType::FLOAT && v2.type == DataType::INT) {
        v2.type = DataType::FLOAT; v2.f_val = static_cast<double>(v2.i_val);
    } else if (v1.type == DataType::INT && v2.type == DataType::FLOAT) {
        v1.type = DataType::FLOAT; v1.f_val = static_cast<double>(v1.i_val);
    }
}

Value Interpreter::getVariableValue(const std::string& name) {
    return variables[name];
}

void Interpreter::execute(const std::vector<RPNItem>& rpn) {
    ip = 0;
    while (ip < rpn.size()) {
        const auto& item = rpn[ip];

        switch (item.type) {
            case RPNItemType::LITERAL: {
                Value val;
                if (item.value[0] == '\'') {
                    val.type = DataType::STRING;
                    val.s_val = item.value.substr(1, item.value.length() - 2);
                } else if (item.value.find('.') != std::string::npos) {
                    val.type = DataType::FLOAT;
                    val.f_val = std::stod(item.value);
                } else {
                    val.type = DataType::INT;
                    val.i_val = std::stoi(item.value);
                }
                stack.push(StackItem{StackItemType::VALUE, val, "", -1});
                ip++;
                break;
            }
            case RPNItemType::LABEL: {
                Value val; val.type = DataType::INT; val.i_val = std::stoi(item.value);
                stack.push(StackItem{StackItemType::VALUE, val, "", -1});
                ip++;
                break;
            }
            case RPNItemType::VARIABLE: {
                // Записываем ссылку на переменную в магазин
                stack.push(StackItem{StackItemType::VAR_REF, Value{}, item.value, -1});
                ip++;
                break;
            }
            case RPNItemType::ASSIGN: {
                Value rightVal = getValue(popStack(item), item);
                StackItem leftRef = popStack(item);
                
                if (leftRef.type == StackItemType::VAR_REF) {
                    variables[leftRef.varName] = rightVal;
                } else if (leftRef.type == StackItemType::ARRAY_ELEM_REF) {
                    arrays[leftRef.varName].data[leftRef.arrayIndex] = rightVal;
                } else {
                    throwError("Левый операнд присваивания должен быть ссылкой", item);
                }
                ip++;
                break;
            }
            case RPNItemType::OPERATOR: {
                if (item.value == "<" || item.value == ">" || item.value == "<=" || 
                    item.value == ">=" || item.value == "==" || item.value == "!=") {
                    evaluateLogic(item.value, item);
                } else {
                    evaluateMath(item.value, item);
                }
                ip++;
                break;
            }
            case RPNItemType::JMP: { // j
                Value label = getValue(popStack(item), item);
                ip = label.i_val;
                break;
            }
            case RPNItemType::JF: { // jf
                Value label = getValue(popStack(item), item);
                Value condition = getValue(popStack(item), item);
                
                if (!condition.b_val) ip = label.i_val; // Переход по лжи
                else ip++;
                break;
            }
            case RPNItemType::ALLOC_1D: { // m1
                Value size = getValue(popStack(item), item);
                StackItem arrayRef = popStack(item);
                if (size.i_val <= 0) throwError("Неверный размер массива", item);
                
                arrays[arrayRef.varName] = ArrayPassport{size.i_val, 0, false, std::vector<Value>(size.i_val)};
                ip++;
                break;
            }
            case RPNItemType::ALLOC_2D: { // m2
                Value cols = getValue(popStack(item), item);
                Value rows = getValue(popStack(item), item);
                StackItem arrayRef = popStack(item);
                
                int totalSize = rows.i_val * cols.i_val;
                if (totalSize <= 0) throwError("Неверные размеры 2D массива", item);
                
                arrays[arrayRef.varName] = ArrayPassport{rows.i_val, cols.i_val, true, std::vector<Value>(totalSize)};
                ip++;
                break;
            }
            case RPNItemType::ARRAY_IDX_1D: { // i
                Value index = getValue(popStack(item), item);
                StackItem arrayRef = popStack(item);
                
                if (arrays.find(arrayRef.varName) == arrays.end()) throwError("Массив не выделен", item);
                if (index.i_val < 0 || index.i_val >= arrays[arrayRef.varName].dim1) 
                    throwError("Выход за границы массива", item);
                
                // Записываем ссылку на элемент массива в магазин
                stack.push(StackItem{StackItemType::ARRAY_ELEM_REF, Value{}, arrayRef.varName, index.i_val});
                ip++;
                break;
            }
            case RPNItemType::ARRAY_IDX_2D: { // i2
                Value colIdx = getValue(popStack(item), item);
                Value rowIdx = getValue(popStack(item), item);
                StackItem arrayRef = popStack(item);
                
                ArrayPassport& p = arrays[arrayRef.varName];
                if (rowIdx.i_val < 0 || rowIdx.i_val >= p.dim1 || colIdx.i_val < 0 || colIdx.i_val >= p.dim2)
                    throwError("Выход за границы 2D массива", item);
                
                // Формула: k * m + j
                int flatIndex = rowIdx.i_val * p.dim2 + colIdx.i_val;
                stack.push(StackItem{StackItemType::ARRAY_ELEM_REF, Value{}, arrayRef.varName, flatIndex});
                ip++;
                break;
            }
            case RPNItemType::READ: { // r
                StackItem varRef = popStack(item);
                std::string input;
                std::cin >> input;
                Value val;
                if (input.find('.') != std::string::npos) { val.type = DataType::FLOAT; val.f_val = std::stod(input); }
                else { val.type = DataType::INT; val.i_val = std::stoi(input); }
                
                if (varRef.type == StackItemType::VAR_REF) variables[varRef.varName] = val;
                else if (varRef.type == StackItemType::ARRAY_ELEM_REF) arrays[varRef.varName].data[varRef.arrayIndex] = val;
                ip++;
                break;
            }
            case RPNItemType::WRITE: { // w
                Value val = getValue(popStack(item), item);
                if (val.type == DataType::INT) std::cout << val.i_val << " ";
                else if (val.type == DataType::FLOAT) std::cout << val.f_val << " ";
                else if (val.type == DataType::STRING) std::cout << val.s_val << " ";
                else if (val.type == DataType::BOOL) std::cout << (val.b_val ? "true" : "false") << " ";
                std::cout << "\n";
                ip++;
                break;
            }
            case RPNItemType::FUNCTION: {
                evaluateFunction(item.value, item);
                ip++;
                break;
            }
            default: ip++; break;
        }
    }
}

void Interpreter::evaluateMath(const std::string& op, const RPNItem& item) {
    Value right = getValue(popStack(item), item);
    Value left = getValue(popStack(item), item);
    castToCommon(left, right);
    
    Value res;
    bool isFloat = (left.type == DataType::FLOAT);
    
    if (op == "+") {
        if (left.type == DataType::STRING) { res.type = DataType::STRING; res.s_val = left.s_val + right.s_val; }
        else if (isFloat) { res.type = DataType::FLOAT; res.f_val = left.f_val + right.f_val; }
        else { res.type = DataType::INT; res.i_val = left.i_val + right.i_val; }
    } else if (op == "-") {
        if (isFloat) { res.type = DataType::FLOAT; res.f_val = left.f_val - right.f_val; }
        else { res.type = DataType::INT; res.i_val = left.i_val - right.i_val; }
    } else if (op == "*") {
        if (isFloat) { res.type = DataType::FLOAT; res.f_val = left.f_val * right.f_val; }
        else { res.type = DataType::INT; res.i_val = left.i_val * right.i_val; }
    } else if (op == "/") {
        if ((isFloat && right.f_val == 0.0) || (!isFloat && right.i_val == 0)) throwError("Деление на 0", item);
        if (isFloat) { res.type = DataType::FLOAT; res.f_val = left.f_val / right.f_val; }
        else { res.type = DataType::INT; res.i_val = left.i_val / right.i_val; }
    }
    stack.push(StackItem{StackItemType::VALUE, res, "", -1});
}

void Interpreter::evaluateLogic(const std::string& op, const RPNItem& item) {
    Value right = getValue(popStack(item), item);
    Value left = getValue(popStack(item), item);
    castToCommon(left, right);
    
    Value res; res.type = DataType::BOOL;
    bool isFloat = (left.type == DataType::FLOAT);
    
    if (op == "<") res.b_val = isFloat ? (left.f_val < right.f_val) : (left.i_val < right.i_val);
    else if (op == ">") res.b_val = isFloat ? (left.f_val > right.f_val) : (left.i_val > right.i_val);
    else if (op == "<=") res.b_val = isFloat ? (left.f_val <= right.f_val) : (left.i_val <= right.i_val);
    else if (op == ">=") res.b_val = isFloat ? (left.f_val >= right.f_val) : (left.i_val >= right.i_val);
    else if (op == "==") res.b_val = isFloat ? (left.f_val == right.f_val) : (left.i_val == right.i_val);
    else if (op == "!=") res.b_val = isFloat ? (left.f_val != right.f_val) : (left.i_val != right.i_val);
    
    stack.push(StackItem{StackItemType::VALUE, res, "", -1});
}

void Interpreter::evaluateFunction(const std::string& func, const RPNItem& item) {
    Value arg = getValue(popStack(item), item);
    double val = (arg.type == DataType::FLOAT) ? arg.f_val : static_cast<double>(arg.i_val);
    
    Value res; res.type = DataType::FLOAT;
    if (func == "sqrt") {
        if (val < 0) throwError("Корень из отрицательного числа", item);
        res.f_val = std::sqrt(val);
    } else if (func == "exp") {
        res.f_val = std::exp(val);
    } else if (func == "log") {
        if (val <= 0) throwError("Логарифм <= 0", item);
        res.f_val = std::log(val);
    }
    stack.push(StackItem{StackItemType::VALUE, res, "", -1});
}