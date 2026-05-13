#ifndef VALUE_H
#define VALUE_H

#include <string>

enum class ValueType { INT, FLOAT, STRING, BOOL, ADDRESS };

struct Value {
    ValueType type;
    long long i_val = 0;
    double f_val = 0.0;
    std::string s_val = "";
    bool b_val = false;

    Value();
    Value(int v);
    Value(long long v);
    Value(double v);
    Value(std::string v);
    Value(bool v);

    std::string toString() const;
    bool toBool() const;
};

#endif