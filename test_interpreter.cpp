#include <iostream>
#include <vector>
#include <string>
#include "interpreter/Interpreter.h"
#include "lexer/Token.h"

Token makeToken(TokenType type, const std::string& value) {
    return Token(type, value, 0, 0);
}

int main() {
    std::cout << "--- ULTRA HARD TEST: Chain of dependencies ---" << std::endl;
    
    std::vector<Token> rpn_tokens;

    // 1. a := 10
    rpn_tokens.push_back(makeToken(T_ID, "a"));
    rpn_tokens.push_back(makeToken(T_INT, "10"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, ":="));

    // 2. b := 5
    rpn_tokens.push_back(makeToken(T_ID, "b"));
    rpn_tokens.push_back(makeToken(T_INT, "5"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, ":="));

    // 3. c := a - b * 4  => В ОПС: c, a, b, 4, *, -, :=
    // Ожидаем: 10 - (5 * 4) = 10 - 20 = -10
    rpn_tokens.push_back(makeToken(T_ID, "c"));
    rpn_tokens.push_back(makeToken(T_ID, "a"));
    rpn_tokens.push_back(makeToken(T_ID, "b"));
    rpn_tokens.push_back(makeToken(T_INT, "4"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, "*"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, "-"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, ":="));

    // 4. PRINT c
    rpn_tokens.push_back(makeToken(T_ID, "c"));
    rpn_tokens.push_back(makeToken(T_KEYWORD, "PRINT"));

    // 5. a := c (перезаписываем 'a' значением -10)
    rpn_tokens.push_back(makeToken(T_ID, "a"));
    rpn_tokens.push_back(makeToken(T_ID, "c"));
    rpn_tokens.push_back(makeToken(T_OPERATOR, ":="));

    // 6. PRINT a
    rpn_tokens.push_back(makeToken(T_ID, "a"));
    rpn_tokens.push_back(makeToken(T_KEYWORD, "PRINT"));

    Interpreter interpreter(rpn_tokens);
    try {
        interpreter.run();
        std::cout << "\n--- FINAL STATE ---" << std::endl;
        interpreter.dumpVariables();
    } catch (const std::exception& e) {
        std::cerr << "RUNTIME ERROR: " << e.what() << std::endl;
    }

    return 0;
}