#include "Token.h"

Token::Token() : type(T_ERROR), value(""), line(0), column(0) {}

Token::Token(TokenType t, const std::string& v, int l, int c)
    : type(t), value(v), line(l), column(c) {}