#include "Interpreter.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

Interpreter::Interpreter(const Program& prog)
    : program(prog), pc(0) {}

Value Interpreter::stackPop() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value v = stack.back();
    stack.pop_back();
    return v;
}

void Interpreter::stackPush(const Value& v) {
    stack.push_back(v);
}

Value Interpreter::performOperation(OpType op, const Value& a, const Value& b) {
    double x = a.toNumber();
    double y = b.toNumber();

    switch (op) {
        case OpType::OP_ADD:
            return Value(x + y);
        case OpType::OP_SUB:
            return Value(x - y);
        case OpType::OP_MUL:
            return Value(x * y);
        case OpType::OP_DIV:
            if (y == 0) throw std::runtime_error("Division by zero");
            return Value(x / y);
        case OpType::OP_LT:
            return Value(x < y ? 1.0 : 0.0);
        case OpType::OP_GT:
            return Value(x > y ? 1.0 : 0.0);
        case OpType::OP_LE:
            return Value(x <= y ? 1.0 : 0.0);
        case OpType::OP_GE:
            return Value(x >= y ? 1.0 : 0.0);
        case OpType::OP_EQ:
            return Value(x == y ? 1.0 : 0.0);
        case OpType::OP_NE:
            return Value(x != y ? 1.0 : 0.0);
        case OpType::OP_AND:
            return Value((x != 0 && y != 0) ? 1.0 : 0.0);
        case OpType::OP_OR:
            return Value((x != 0 || y != 0) ? 1.0 : 0.0);
        default:
            throw std::runtime_error("Unknown binary operation");
    }
}

Value Interpreter::performUnaryOperation(OpType op, const Value& a) {
    double x = a.toNumber();

    switch (op) {
        case OpType::OP_NEG:
            return Value(-x);
        case OpType::OP_NOT:
            return Value(x == 0 ? 1.0 : 0.0);
        default:
            throw std::runtime_error("Unknown unary operation");
    }
}

void Interpreter::execute() {
    pc = 0;

    while (pc < static_cast<int>(program.size())) {
        const Instruction& instr = program[pc];

        switch (instr.op) {
            case OpType::OP_PUSH_INT:
                stackPush(Value(std::stoi(instr.arg)));
                break;

            case OpType::OP_PUSH_FLOAT:
                stackPush(Value(std::stod(instr.arg)));
                break;

            case OpType::OP_PUSH_STRING:
                stackPush(Value(instr.arg));
                break;

            case OpType::OP_PUSH_VAR: {
                if (variables.find(instr.arg) != variables.end()) {
                    stackPush(variables[instr.arg]);
                } else {
                    stackPush(Value(0.0));  // неинициализированная переменная = 0
                }
                break;
            }

            case OpType::OP_POP_VAR: {
                Value v = stackPop();
                variables[instr.arg] = v;
                break;
            }

            case OpType::OP_ADD: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_ADD, a, b));
                break;
            }

            case OpType::OP_SUB: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_SUB, a, b));
                break;
            }

            case OpType::OP_MUL: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_MUL, a, b));
                break;
            }

            case OpType::OP_DIV: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_DIV, a, b));
                break;
            }

            case OpType::OP_NEG: {
                Value a = stackPop();
                stackPush(performUnaryOperation(OpType::OP_NEG, a));
                break;
            }

            case OpType::OP_LT: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_LT, a, b));
                break;
            }

            case OpType::OP_GT: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_GT, a, b));
                break;
            }

            case OpType::OP_LE: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_LE, a, b));
                break;
            }

            case OpType::OP_GE: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_GE, a, b));
                break;
            }

            case OpType::OP_EQ: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_EQ, a, b));
                break;
            }

            case OpType::OP_NE: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_NE, a, b));
                break;
            }

            case OpType::OP_AND: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_AND, a, b));
                break;
            }

            case OpType::OP_OR: {
                Value b = stackPop();
                Value a = stackPop();
                stackPush(performOperation(OpType::OP_OR, a, b));
                break;
            }

            case OpType::OP_NOT: {
                Value a = stackPop();
                stackPush(performUnaryOperation(OpType::OP_NOT, a));
                break;
            }

            case OpType::OP_PRINT: {
                Value v = stackPop();
                std::cout << v.toString();
                break;
            }

            case OpType::OP_READ: {
                double val;
                std::cin >> val;
                stackPush(Value(val));
                break;
            }

            case OpType::OP_JMP:
                pc = instr.jumpTarget - 1;  // -1 потому что в конце pc++
                break;

            case OpType::OP_JMP_FALSE: {
                Value cond = stackPop();
                if (cond.toNumber() == 0) {
                    pc = instr.jumpTarget - 1;
                }
                break;
            }

            case OpType::OP_ARRAY_DECL: {
                Value size = stackPop();
                arrayInfo[instr.arg] = {static_cast<int>(size.toNumber()), 0};
                variables[instr.arg].arrayValue.resize(static_cast<int>(size.toNumber()));
                break;
            }

            case OpType::OP_ARRAY_SET: {
                Value val = stackPop();
                Value idx = stackPop();
                int index = static_cast<int>(idx.toNumber());
                if (variables[instr.arg].arrayValue.size() > static_cast<size_t>(index)) {
                    variables[instr.arg].arrayValue[index] = val.toNumber();
                }
                break;
            }

            case OpType::OP_ARRAY_GET: {
                Value idx = stackPop();
                int index = static_cast<int>(idx.toNumber());
                if (variables[instr.arg].arrayValue.size() > static_cast<size_t>(index)) {
                    stackPush(Value(variables[instr.arg].arrayValue[index]));
                } else {
                    stackPush(Value(0.0));
                }
                break;
            }

            case OpType::OP_END:
            case OpType::OP_NOP:
                break;

            default:
                break;
        }

        pc++;
    }
}

void Interpreter::printVariable(const std::string& name) {
    if (variables.find(name) != variables.end()) {
        std::cout << variables[name].toString();
    }
}
