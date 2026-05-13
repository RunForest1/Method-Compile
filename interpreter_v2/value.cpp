#include "Value.h"

Value::Value() : type(ValueType::INT) {}
Value::Value(int v) : type(ValueType::INT), i_val(v) {}
Value::Value(long long v) : type(ValueType::INT), i_val(v) {}
Value::Value(double v) : type(ValueType::FLOAT), f_val(v) {}
Value::Value(std::string v) : type(ValueType::STRING), s_val(v) {}
Value::Value(bool v) : type(ValueType::BOOL), b_val(v) {}

std::string Value::toString() const {
    if (type == ValueType::INT) return std::to_string(i_val);
    if (type == ValueType::FLOAT) return std::to_string(f_val);
    if (type == ValueType::STRING) return s_val;
    if (type == ValueType::BOOL) return b_val ? "true" : "false";
    return "ADDR(" + s_val + ")";
}

bool Value::toBool() const {
    if (type == ValueType::BOOL) return b_val;
    if (type == ValueType::INT) return i_val != 0;
    if (type == ValueType::FLOAT) return f_val != 0.0;
    return false;
}